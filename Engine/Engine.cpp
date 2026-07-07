// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Engine.h>

#include <InputContext.h>
#include <RenderEngine.h>
#include <ResourceManager.h>
#include <windowsx.h>
#include <Editor/WorldEditor.h>
#include <Editor/WorldDocument.h>
#include <Controllers/CameraController.h>
#include <Core/Grid.h>
#include <Objects/Entities/GameLight.h>

Engine::Engine(const std::weak_ptr<Nt::EventBus>& pBus) :
	m_pEventBus(pBus),
	m_InputContext(new InputContext),
	m_Window(true)
{
	Assert(!m_pEventBus.expired(), "EventBus pointer is null");
	m_Window.SetEventBus(m_pEventBus);
	m_pEventBus.lock()->Emmit<AddInputContextEvent>({ m_InputContext, "Engine" });
}

Engine::~Engine() {
	m_Window.GetEventBus().lock()->Clear();

	m_pComboBoxView.release();
	m_pGridButton.release();
	m_pGridTextEdit.release();
}

void Engine::Initialize(const Settings& settings, const Nt::String& defaultInitialPath) {
	const uInt pathType = GetFileAttributesA(defaultInitialPath);
	if (pathType == INVALID_FILE_ATTRIBUTES)
		Raise(std::string("A non-existent path was specified: ") + defaultInitialPath);
	if (!(pathType & FILE_ATTRIBUTE_DIRECTORY))
		Raise(std::string("This path is not a directory: ") + defaultInitialPath);

	m_Settings = settings;

	m_Window.RemoveStyles(Nt::Window::STYLE_OVERLAPPEDWINDOW);
	m_Window.AddStyles(Nt::Window::STYLE_BORDER);
	m_Window.Create(m_Settings.EngineWindowRect, settings.Language["Window.Engine"]);
	m_Window.SetProcedure([&] (const uInt& uMsg, const DWord& param_1, const DWord& param_2) -> Long {
		return _Procedure(uMsg, param_1, param_2);
		});

	m_Listener = std::make_shared<Listener>(this);

	m_Window.RegisterWindowListener(m_Listener.get());
	m_Window.RegisterMouseListener(m_Listener.get());

	//{
	//	std::unique_ptr<Nt::Texture> pTexture = std::make_unique<Nt::Texture>();
	//	Byte* pPixel = new Byte[4];
	//	for (uInt i = 0; i < 4; ++i)
	//		pPixel[i] = 255;

	//	pTexture->Create(4, { 1, 1 }, pPixel);
	//	Nt::ResourceManager::Instance().Add(std::move(pTexture));
	//}

	ResourceManager::Instance().SetInitialPath(defaultInitialPath);
	ResourceManager::Instance().LoadAllFromQueue();

	_InitializeUI(defaultInitialPath);

	m_WorldEditor = std::make_unique<WorldEditor>(m_pEventBus, defaultInitialPath);
	m_WorldEditor->ResetCamera();

	m_pScene = m_WorldEditor->GetScene();
	m_pSelector = m_WorldEditor->GetSelector();

	NtEx::Camera3D* pCamera = &m_WorldEditor->GetCamera();
	m_CameraController.reset(new CameraController(pCamera));

	const auto scene = m_pScene.lock();
	assert(scene);

	m_InputContext->AddHotKey(
		{ Nt::KEY_ESCAPE }, [this] () { m_pSelector->AllDeselect(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_DEL }, [this, scene] () { scene->RemoveSelected(m_pSelector); });

	m_InputContext->AddHotKey(
		{ Nt::KEY_T }, [this] () { if (!m_IsFly) m_pSelector->SetTransformMode(Manipulator::TRANSLATE); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_S }, [this] () { if (!m_IsFly) m_pSelector->SetTransformMode(Manipulator::SCALE); });

	m_InputContext->AddHotKey(
		{ Nt::KEY_MULTIPLY }, [this] () { m_WorldEditor->CreateEntity("Light"); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_SUBTRACT }, [this] () { m_WorldEditor->CreateEntity("Camera"); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_ADD }, [this] () { m_WorldEditor->CreatePrimitive("Cube"); });

	m_InputContext->AddHotKey(
		{ Nt::KEY_F5 }, [this] () { StartTestGame(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_SHIFT, Nt::KEY_F5 }, [this] () { CloseTestGame(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_SHIFT, Nt::KEY_F5 }, [this] () {
			if (m_Game->IsLaunched()) {
				CloseTestGame();
				StartTestGame();
			}
		});

	m_Game.reset(new Game(&m_Window, scene.get()));

	auto shader = new Nt::Shader;
	shader->Initialize();
	shader->Create();
	shader->CompileFromFile(Nt::Shader::VERTEX, "..\\Shaders\\Vert.glsl");
	shader->CompileFromFile(Nt::Shader::FRAGMENT, "..\\Shaders\\Frag.glsl");
	shader->Link();

	shader->DisableStrict();
	shader->SetUniform("fFullBright", false);
	shader->SetUniform("fLight", false);
	shader->SetUniform("LightDirection", Nt::Float3D(0.f, -1.f, 0.f));
	shader->SetUniform("LightColor", Nt::Colors::White.rgb);

	shader->SetUniform("IsObjectSelected", false);
	shader->SetUniform("IsLightsEnabled", true);
	shader->SetUniform("NumLights", scene->GetLightsCount());

	shader->UniformBlockBinding("Lights", 0);
	shader->BindBufferBase(scene->GetLightBuffer(), 0);

	auto sharedBus = m_pEventBus.lock();
	sharedBus->Subscribe<Scene::EventAddObject>(
		[this, shader, scene] (const Scene::EventAddObject& e) {
			if (e.pObject->GetToken() == GameLight::GetClassToken())
				shader->SetUniform<Int>("NumLights", scene->GetLightsCount());
		});
	sharedBus->Subscribe<Scene::EventRemoveObject>(
		[this, shader, scene] (const Scene::EventRemoveObject& e) {
			if (e.pObject->GetToken() == GameLight::GetClassToken())
				shader->SetUniform<Int>("NumLights", scene->GetLightsCount() - 1);
		});
	sharedBus->Subscribe<Scene::EventClear>(
		[this, shader] (const Scene::EventClear& e) {
			(void)e;
			shader->SetUniform<Int>("NumLights", 0);
		});

	m_RenderEngine.reset(new RenderEngine(&m_Window, shader));
	m_RenderEngine->SetScene(scene.get());
	m_RenderEngine->SetCamera(&m_WorldEditor->GetCamera());

	m_Window.SetClearColor(m_Settings.Style["Engine.BackgroundColor"]);
}

void Engine::StartTestGame() const {
	if (m_Game->IsLaunched())
		return;

	m_pSelector->AllDeselect();
	m_Game->Start();
}

void Engine::CloseTestGame() {
	if (m_Game->IsLaunched()) {
		m_RenderEngine->SetCamera(&m_WorldEditor->GetCamera());
		m_Game->End();
	}
}
void Engine::Update() {
	const Float time = m_Window.GetFrameTimeMs();
	if (m_Game->IsLaunched()) {
		m_Game->Update(time);
	}
	else {
		_Control();
		m_WorldEditor->Update(time);
	}
}

void Engine::Render() {
	if (m_Game->IsLaunched()) {
		m_Game->Render();
	}
	else {
		m_Window.Clear();

		m_RenderEngine->Render();

		const Nt::Renderer::DrawingMode drawingMode = m_Window.GetDrawingMode();

		m_RenderEngine->RenderObject(m_WorldEditor->GetGrid());

		m_RenderEngine->GetShader()->SetUniform<Nt::Float3D>("LightDirection", Nt::Float3D(1.f, -1.f, 1.f).GetNormalize());
		m_RenderEngine->GetShader()->SetUniform("LightColor", Nt::Colors::White.rgb);
		m_RenderEngine->GetShader()->SetUniform("AmbientColor", Nt::Float3D(0.3f, 0.3f, 0.3f));
		m_RenderEngine->GetShader()->SetUniform("fLight", true);

		Selector* pSelector = m_WorldEditor->GetSelector();
		if (pSelector->EnabledDebug())
			m_Window.Render(pSelector->GetRayMesh());
		m_Window.SetDrawingMode(drawingMode);

		m_Window.DisableDepthBuffer();

		const Manipulator* pManipulator = pSelector->GetManipulator();
		if (pManipulator->IsVisible()) {
			for (uInt i = 0; i < 3; ++i) {
				const Manipulator::Arrow* arrow = pManipulator->GetArrow(i);
				if (arrow->IsShowedLine()) {
					auto color = m_Window.GetColor();

					m_Window.MatrixWorldPush();
					m_Window.SetWorld(arrow->LocalToWorld());
					m_Window.SetColor(arrow->GetColor());
					m_Window.SetDrawingMode(Nt::Renderer::DrawingMode::LINES);
					m_Window.Render(arrow->GetLineMesh());
					m_Window.SetDrawingMode(drawingMode);
					m_Window.SetColor(color);
					m_Window.MatrixWorldPop();
				}

				m_RenderEngine->RenderObject(arrow);
			}
		}

		m_Window.EnableDepthBuffer();

		m_Window.Display();
	}
}

void Engine::Show() {
	m_Window.Show();
}

void Engine::DeactivateWindow() {
	if (m_IsFly)
		DisableFly();
	m_InputContext->ClearActiveKeys();
}

void Engine::EnableFly() noexcept {
	m_IsFly = true;
	Nt::_ShowCursor(false);
	SetCursorPos(Nt::GetMonitorSize().x / 2, Nt::GetMonitorSize().y / 2);
}

void Engine::DisableFly() noexcept {
	m_IsFly = false;
	Nt::_ShowCursor(true);
}

void Engine::ToggleFly() noexcept {
	::SetFocus(m_Window.GetHandle());
	if (m_IsFly)
		DisableFly();
	else
		EnableFly();
}

WorldEditor* Engine::GetWorldEditor() const noexcept {
	return m_WorldEditor.get();
}

Selector* Engine::GetSelector() const noexcept {
	return m_pSelector;
}

std::weak_ptr<Scene> Engine::GetScene() const noexcept {
	return m_pScene;
}

Bool Engine::IsFly() const noexcept {
	return m_IsFly;
}

void Engine::SetRect(const Nt::IntRect& rect) noexcept {
	m_Window.SetWindowRect(rect);
}

void Engine::SetParent(Nt::HandleWindow& window) noexcept {
	m_Window.SetParent(window);
}

void Engine::SetTheme(const Style& style) {
	m_Settings.Style = style;
	m_Window.SetClearColor(style["Engine.BackgroundColor"]);
	m_Window.SetBorderColor(style["Engine.BorderColor"]);
}

void Engine::SetLanguage(const Language& language) {
	m_Window.SetName(language["Window.Engine"]);
}

void Engine::_InitializeUI(const std::string& defaultInitialPath) {
	m_pComboBoxView = std::make_unique<Nt::ComboBox>();
	m_pComboBoxView->AddStyles(CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SIMPLE);
	m_pComboBoxView->SetID(CONTROL_COMBOBOX_PROJECTION);
	m_pComboBoxView->SetParent(m_Window);
	m_pComboBoxView->Create({ 25, 5, 110, 140 });
	m_pComboBoxView->AddElement("Perspective");
	m_pComboBoxView->AddElement("Top");
	m_pComboBoxView->AddElement("Bottom");
	m_pComboBoxView->AddElement("Left");
	m_pComboBoxView->AddElement("Right");
	m_pComboBoxView->AddElement("Back");
	m_pComboBoxView->AddElement("Forward");
	m_pComboBoxView->SetCurrentElement(0);
	m_pComboBoxView->Show();

	Nt::IntRect gridTextEditRect;
	gridTextEditRect.RightBottom = { 100, 24 };
	gridTextEditRect.Left = m_Window.GetWindowRect().Right - gridTextEditRect.Right - 25;
	gridTextEditRect.Top = 5;

	m_pGridTextEdit = std::make_unique<Nt::TextEdit>();
	m_pGridTextEdit->SetID(CONTROL_TEXTEDIT_GRIDCELLSIZE);
	m_pGridTextEdit->SetParent(m_Window);
	m_pGridTextEdit->Create(gridTextEditRect, "1.0", true);
	m_pGridTextEdit->Show();

	Nt::IntRect gridButtonRect;
	gridButtonRect.RightBottom = Nt::Int2D(gridTextEditRect.Bottom, gridTextEditRect.Bottom);
	gridButtonRect.LeftTop = gridTextEditRect.LeftTop;
	gridButtonRect.Left -= gridButtonRect.Right;

	Nt::GDI::Bitmap gridIcon;
	gridIcon.LoadFromFile(defaultInitialPath + "Images\\GridIcon.bmp");

	m_pGridButton = std::make_unique<Nt::Button>();
	m_pGridButton->AddStyles(BS_LEFT | BS_BITMAP | BS_VCENTER | BS_AUTOCHECKBOX | BS_PUSHLIKE);
	m_pGridButton->SetID(CONTROL_BUTTON_TOGGLEGRID);
	m_pGridButton->SetParent(m_Window);
	m_pGridButton->Create(gridButtonRect, "");
	m_pGridButton->SetImage(gridIcon);
	m_pGridButton->Show();
}

void Engine::_SetPerspective() noexcept {
	const Nt::Float2D windowSize(m_Window.GetSize());

	m_Window.SetProjection(Nt::ComputePerspectiveRH(
		m_Settings.Projection_FOV,
		windowSize.x / windowSize.y,
		m_Settings.Projection_Near,
		m_Settings.Projection_Far
		));
}

void Engine::_SetOrtho() noexcept {
	Nt::Float2D windowSize = Nt::Float2D(m_Window.GetClientRect().RightBottom);
	if (windowSize.x < windowSize.y) {
		windowSize.y /= windowSize.x;
		windowSize.x = 1.f;
	}
	else if (windowSize.y < windowSize.x) {
		windowSize.x /= windowSize.y;
		windowSize.y = 1.f;
	}

	Nt::FloatRect orthoRect = {
		-windowSize.x / 2.f,
		windowSize.y / 2.f,
		windowSize.x / 2.f,
		-windowSize.y / 2.f
	};

	m_Window.SetProjection(Nt::ComputeOrthoLH(orthoRect / m_Zoom, -1000.f, 1000.f));
}

void Engine::_Control() {
	m_Keyboard.Update();
	m_Mouse.Update();

	const Bool isPressedControl = m_Keyboard.IsKeyPressed(Nt::KEY_CONTROL, false);

	if (m_Mouse.IsButtonPressed(Nt::BUTTON_RIGHT, true))
		ToggleFly();

	if (m_IsFly) {
		m_CameraController->Update();
	}
	else {
		m_pSelector->Control(&m_Window, *m_RenderEngine->GetCamera(), m_Keyboard, m_Mouse);

		if (isPressedControl && m_Keyboard.IsKeyPressed(Nt::KEY_J, true))
			_CSG();
	}
}

void Engine::_CSG() {
	assert(0);
	if (m_pSelector->GetObjectCount() <= 1)
		return;
}

Long Engine::_Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
	const uInt id = LOWORD(param_1);
	const uInt command = HIWORD(param_1);

	switch (uMsg) {
	case WM_COMMAND:
		switch (command) {
		case CBN_SELCHANGE:
			switch (id) {
			case CONTROL_COMBOBOX_PROJECTION:
				m_Projection = (ViewMode)ComboBox_GetCurSel(reinterpret_cast<HWND>(param_2));
				_UpdateProjection();
				break;
			}
			break;
		case EN_UPDATE:
			switch (id) {
			case CONTROL_TEXTEDIT_GRIDCELLSIZE:
				Grid* pGrid = m_WorldEditor->GetGrid();
				pGrid->SetCellSize(m_pGridTextEdit->GetText());
				break;
			}
			break;
		case BN_CLICKED:
			switch (id) {
			case CONTROL_BUTTON_TOGGLEGRID:
				m_pSelector->ToggleSnapToGrid(m_pGridButton->IsChecked());
				break;
			}
			break;
		}
		break;
	}
	return DefWindowProc(m_Window.GetHandle(), uMsg, param_1, param_2);
}

void Engine::_UpdateProjection() {
	if (m_Projection == ViewMode::PERSPECTIVE) {
		_SetPerspective();
		m_RenderEngine->SetCamera(&m_WorldEditor->GetCamera());
	}
	else {
		_SetOrtho();
		m_RenderEngine->SetCamera(&m_OrthoCamera);

		const Float scaledHeight = 0.5f;

		Nt::Float3D cameraPosition;
		switch (m_Projection) {
		case ViewMode::BOTTOM:
			cameraPosition.y = -scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ -PIf * 0.5f, 0.f, 0.f });
			break;
		case ViewMode::TOP:
			cameraPosition.y = scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ PIf * 0.5f, 0.f, 0.f });
			break;
		case ViewMode::LEFT:
			cameraPosition.x = -scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ 0.f, PIf * 0.5f, 0.f });
			break;
		case ViewMode::RIGHT:
			cameraPosition.x = scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ 0.f, -PIf * 0.5f, 0.f });
			break;
		case ViewMode::BACK:
			cameraPosition.z = -scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ 0.f, 0.f, 0.f });
			break;
		case ViewMode::FORWARD:
			cameraPosition.z = scaledHeight / m_Zoom;
			m_OrthoCamera.Position(cameraPosition);
			m_OrthoCamera.RotationEuler({ 0.f, PIf, 0.f });
			break;
		}
	}
}