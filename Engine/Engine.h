#pragma once

class Engine {
private:
	enum class View {
		PERSPECTIVE,
		TOP, BOTTOM,
		LEFT, RIGHT,
		BACK, FORWARD
	};

public:
	Engine() :
		m_pGame(nullptr),
		m_pScence(nullptr),
		m_Window(true),
		m_IsFly(false),
		m_CameraSpeed(0.05f),
		m_IsChanged(false)
	{ 
	}

	void Initialize(const Settings& settings) {
		m_Settings = settings;

		m_Window.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_Window.AddStyles(WS_DLGFRAME);
		m_Window.Create(m_Settings.EngineWindowRect, settings.CurrentLanguage.Window.EngineWindow);
		m_Window.SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) -> Long {
			return _Procedure(uMsg, param_1, param_2);
			});

		m_ComboBoxView.AddStyles(CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SIMPLE | WS_VISIBLE);
		m_ComboBoxView.SetParent(m_Window);
		m_ComboBoxView.Create({ 25, 5, 110, 140 });
		m_ComboBoxView.AddElement("Perspective");
		m_ComboBoxView.AddElement("Top");
		m_ComboBoxView.AddElement("Bottom");
		m_ComboBoxView.AddElement("Left");
		m_ComboBoxView.AddElement("Right");
		m_ComboBoxView.AddElement("Back");
		m_ComboBoxView.AddElement("Forward");
		m_ComboBoxView.SetCurrentElement(0);

		m_pScence = new Scence;

		m_pGame = new Game;
		m_pGame->InitializeTestGame(&m_Window, m_pScence);

		m_Shader.Initialize();
		m_Shader.CompileFromFile(Nt::Shader::Types::VERTEX, "..\\Shaders\\Vert.glsl");
		m_Shader.CompileFromFile(Nt::Shader::Types::FRAGMENT, "..\\Shaders\\Frag.glsl");
		m_Shader.Link();
		m_Shader.DisableStrict();
		m_Shader.SetUniform("IsObjectSelected", GL_INT, false);
		m_Window.SetCurrentShader(&m_Shader);

		_ResertCamera();
		m_Window.SetCurrentCamera(&m_Camera);
		
		const Nt::Float3D clearColor = m_Settings.Styles.Engine.BackgroundColor;
		m_Window.SetClearColor(Nt::Float4D(clearColor, 255.f) / 255.f);
	}
	
	void CreateCube() {
		constexpr Nt::Float3D size(1.f, 1.f, 1.f);
		Object* pObject = new Primitive("Cube", PrimitiveTypes::CUBE, size);
		m_pScence->AddObject(pObject);
		m_IsChanged = true;
	}
	void CreatePyramid() {
		constexpr Nt::Float3D size(1.f, 1.f, 1.f);
		m_pScence->AddObject(new Primitive("Pyramid", PrimitiveTypes::PYRAMID, size));
		m_IsChanged = true;
	}
	void CreateQuad() {
		constexpr Nt::Float3D size(1.f, 1.f, 1.f);
		m_pScence->AddObject(new Primitive("Quad", PrimitiveTypes::QUAD, size));
		m_IsChanged = true;
	}
	void CreatePlane() {
		constexpr Nt::Float3D size(1.f, 1.f, 1.f);
		m_pScence->AddObject(new Primitive("Plane", PrimitiveTypes::PLANE, size));
		m_IsChanged = true;
	}
	void CreateCamera() {
		m_pScence->AddObject(new GameCamera("Camera"));
		m_IsChanged = true;
	}
	void CreateSound() {
		m_pScence->AddObject(new GameSound("Sound"));
		m_IsChanged = true;
	}
	void CreateModel() {
		m_pScence->AddObject(new GameModel("Model"));
		m_IsChanged = true;
	}

	void StartTestGame() {
		if (!m_pGame->IsLaunched()) {
			if (m_pGame == nullptr)
				Raise("Game pointer is nullptr");

			m_pGame->Start();;
			if (m_pGame->IsLaunched())
				m_Selector.AllDeselect();
		}
	}
	void CloseTestGame() {
		if (m_pGame->IsLaunched()) {
			m_Window.SetCurrentCamera(&m_Camera);
			m_pGame->End();
		}
	}

	Bool PopEvent(Nt::Event* pEvent) {
		return m_Window.PopEvent(pEvent);
	}
	void HandleEvent(const Nt::Event& event) {
		switch (event.Type) {
		case Nt::Event::WINDOW_RESIZE:
			m_Window.SetPerspectiveProjection(
				m_Settings.Projection_FOV, 
				m_Settings.Projection_Near, 
				m_Settings.Projection_Far);
			break;
		case Nt::Event::MOUSE_WHEEL:
			if (event.Value < 0.f && m_CameraSpeed > 0.001f)
				m_CameraSpeed /= 1.25f;
			else if (event.Value > 0.f && m_CameraSpeed < 1.f)
				m_CameraSpeed *= 1.25f;
			break;
		case Nt::Event::KEY_UP:
			switch (event.Value) {
			case Nt::Keyboard::KEY_SUBTRACT:
				CreateCamera();
				break;
			case Nt::Keyboard::KEY_ADD:
				CreateCube();
				break;
			}
			break;
		}
	}

	void Update() {
		const Float time = m_Window.GetFPS() / 1000.f;
		if (m_pGame->IsLaunched()) {
			m_pGame->Update(time);
		}
		else {
			_Control(time);
			m_pScence->Update(time);
			m_Window.Update();
		}
	}

	void Render() {
		if (m_pGame->IsLaunched()) {
			m_pGame->Render();
		}
		else {
			m_Window.Clear();
			m_pScence->Render(&m_Window);
			m_Window.Display();
		}
	}

	void Show() {
		m_Window.Show();
	}

	void BindObjectsTree(ObjectsTree* objectsTreePtr) noexcept {
		m_ObjectsTreePtr = objectsTreePtr;
		m_pScence->BindObjectsTree(objectsTreePtr);
		objectsTreePtr->SetSelector(&m_Selector);
	}

	void EnableFly() noexcept {
		m_IsFly = true;
		SetCursorPos(Nt::GetMonitorSize().x / 2, Nt::GetMonitorSize().y / 2);
		Nt::_ShowCursor(false);
	}
	void DisableFly() noexcept {
		m_IsFly = false;
		Nt::_ShowCursor(true);
	}
	void ToggleFly() noexcept {
		::SetFocus(m_Window.GetHandle());
		if (m_IsFly)
			DisableFly();
		else
			EnableFly();
	}

	void New() {
		m_pScence->Clear();
		_ResertCamera();
		m_IsChanged = false;
	}
	void Open() {
		m_Selector.AllDeselect();
		m_Selector.UnmarkChanged();

		const Nt::String path = Nt::OpenFileDialog(L"", L"Scence (*.ntascn)\0*.ntascn");
		if (path != "") {
			if (m_pScence->Load(path)) {
				m_FilePath = path;
				m_IsChanged = false;
			}
		}
	}
	void Save(const Nt::String& startPath) {
		if (m_FilePath != "")
			m_pScence->Save(m_FilePath);
		else
			SaveAs(startPath);
	}
	void SaveAs(const Nt::String& startPath) {
		const Nt::String path = Nt::SaveAsFileDialog(startPath.wstr().c_str(), L"Scence (*.ntascn)\0*.ntascn");
		if (path != "") {
			m_FilePath = path;
			m_pScence->Save(path);
		}
	}

	Selector* GetSelectorPtr() noexcept {
		return &m_Selector;
	}
	Scence* GetScencePtr() const noexcept {
		return m_pScence;
	}
	Bool IsFly() const noexcept {
		return m_IsFly;
	}
	Bool IsChanged() const noexcept {
		return m_IsChanged;
	}

	void SetRect(const Nt::IntRect& rect) noexcept {
		m_Window.SetWindowRect(rect);
	}
	void SetParent(Nt::HandleWindow& window) noexcept {
		m_Window.SetParent(window);
	}
	void SetTheme(const Style& style) {
		const Nt::Float3D clearColor = style.Engine.BackgroundColor;
		m_Window.SetClearColor(Nt::Float4D(clearColor, 255.f) / 255.f);
	}
	void SetLanguage(const Language& language) {
		m_Window.SetName(language.Window.EngineWindow);
	}

private:
	Nt::ComboBox m_ComboBoxView;
	Nt::RenderWindow m_Window;
	Nt::Shader m_Shader;
	Nt::Camera m_Camera;
	Nt::Camera m_OrthoCamera;
	Nt::Keyboard m_Keyboard;
	Nt::Mouse m_Mouse;

	Game* m_pGame;
	Selector m_Selector;
	ObjectsTree* m_ObjectsTreePtr;
	Scence* m_pScence;
	Settings m_Settings;
	Nt::String m_FilePath;
	View m_Projection;
	Float m_CameraSpeed;

	Bool m_IsFly;
	Bool m_IsChanged;
	Bool m_IsOrtoEnabled;

private:
	void _ResertCamera() {
		m_Camera.SetPosition({ 0.f, -3.f, -5.f });
		m_Camera.SetAngle({ -35.f, 0.f, 0.f });
	}
	void _Control(const Float& time) {
		m_Keyboard.Update();
		m_Mouse.Update();
		
		const Bool isPressedControl = m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL);

		if (m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_RIGHT, true))
			ToggleFly();

		if (m_IsFly) {
			Nt::Float3D move;
			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_W)) {
				move.x += sin(m_Camera.GetAngle().y * RAD);
				move.z += cos(m_Camera.GetAngle().y * RAD);
			}
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_S)) {
				move.x -= sin(m_Camera.GetAngle().y * RAD);
				move.z -= cos(m_Camera.GetAngle().y * RAD);
			}

			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_A)) {
				move.x += sin((m_Camera.GetAngle().y + 90.f) * RAD);
				move.z += cos((m_Camera.GetAngle().y + 90.f) * RAD);
			}
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_D)) {
				move.x += sin((m_Camera.GetAngle().y - 90.f) * RAD);
				move.z += cos((m_Camera.GetAngle().y - 90.f) * RAD);
			}

			if (!isPressedControl) {
				if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SPACE))
					move.y = -1.f;
				else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SHIFT))
					move.y = 1.f;
			}
			m_Camera.Translate(move * m_CameraSpeed);

			const Nt::Float2D cursorPosition = Nt::GetCursorPosition();
			const Nt::Float2D screenCenter = Nt::GetMonitorSize() / 2;
			const Nt::Float2D angleRotation = -((cursorPosition - screenCenter) / 6.f);
			m_Camera.Rotate({ angleRotation.y, angleRotation.x, 0.f });
			SetCursorPos(screenCenter.x, screenCenter.y);
		}
		else if (m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, true)) {
			Nt::Ray ray;
			ray.Start = m_Camera.GetPosition();
			ray.End = ray.Start;

			Nt::Float2D cursorPosition = m_Window.ScreenToClient(m_Mouse.GetCursorPosition());
			cursorPosition = cursorPosition * 2.f / m_Window.GetClientRect().RightBottom - 1.f;
			if (abs(cursorPosition.x) <= 1.f && abs(cursorPosition.y) <= 1.f) {
				ray.End.x += sin(m_Camera.GetAngle().y * RAD);
				ray.End.y -= tan(m_Camera.GetAngle().x * RAD);
				ray.End.z += cos(m_Camera.GetAngle().y * RAD);

				ray.End.x += tan(-cursorPosition.x * m_Settings.Projection_FOV * RAD) * cos(m_Camera.GetAngle().y * RAD);
				ray.End.y += tan(cursorPosition.y * m_Settings.Projection_FOV * RAD);
				ray.End.z += tan(cursorPosition.x * m_Settings.Projection_FOV * RAD) * sin(m_Camera.GetAngle().y * RAD);

				for (uInt i = 0; i < m_pScence->GetObjects().size(); ++i) {
					Object* pObject = m_pScence->GetObjects()[i];
					if (pObject == nullptr)
						Raise("Object pointer is null.");
					if (pObject->RayCastTest(ray)) {
						if (isPressedControl)
							m_Selector.AddSelect(pObject);
						else
							m_Selector.Select(pObject);
						break;
					}
				}
			}
		}

		_SelectionMoving();

		wChar className[12];
		GetClassName(GetFocus(), className, 12);
		if (lstrcmpW(MSFTEDIT_CLASS, className) != 0 && lstrcmpW(L"EDIT", className) != 0) {
			if (!m_ObjectsTreePtr->IsRenamingEnabled()) {
				if (isPressedControl) {
					if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_C, true)) {
						m_pScence->Copy(&m_Selector);
					}
					else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_X, true)) {
						m_pScence->Cut(&m_Selector);
					}
					else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_V, true)) {
						m_pScence->Paste(&m_Selector);
					}
				}
				else {
					if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_ESCAPE, true)) {
						m_Selector.AllDeselect();
					}
					else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_DEL, true)) {
						m_pScence->RemoveSelected(&m_Selector);
						m_IsChanged = true;
					}
				}
			}
		}
	}
	void _SelectionMoving() {
		constexpr Float movingSpeed = 0.025f;
		if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL)) {
			Nt::Float3D move;
			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_LEFT))
				move.x -= movingSpeed;
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_RIGHT))
				move.x += movingSpeed;

			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SPACE))
				move.y += movingSpeed;
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SHIFT))
				move.y -= movingSpeed;
			
			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_UP))
				move.z -= movingSpeed;
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_DOWN))
				move.z += movingSpeed;

			if (move != Nt::Float3D::Zero) {
				for (Object* pObject : m_Selector.GetObjects())
					pObject->Translate(move);
			}
		}
	}
	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		switch (uMsg) {
		case WM_COMMAND:
			switch (HIWORD(param_1)) {
			case CBN_SELCHANGE:
				m_Projection = (View)ComboBox_GetCurSel(reinterpret_cast<HWND>(param_2));
				_UpdateProjection();
				break;
			}
			break;
		}
		return DefWindowProc(m_Window.GetHandle(), uMsg, param_1, param_2);
	}
	void _UpdateProjection() {
		if (m_Projection == Engine::View::PERSPECTIVE) {
			m_Window.SetPerspectiveProjection(
				m_Settings.Projection_FOV,
				m_Settings.Projection_Near,
				m_Settings.Projection_Far);
			m_Window.SetCurrentCamera(&m_Camera);
		}
		else {
			Nt::Float3D cameraPorition = m_Camera.GetPosition();
			switch (m_Projection) {
			case Engine::View::TOP:
				cameraPorition.y = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ -90.f, 0.f, 0.f });
				break;
			case Engine::View::BOTTOM:
				cameraPorition.y = 100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 90.f, 0.f, 0.f });
				break;
			case Engine::View::LEFT:
				cameraPorition.x = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, 90.f, 0.f });
				break;
			case Engine::View::RIGHT:
				cameraPorition.x = 100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, -90.f, 0.f });
				break;
			case Engine::View::BACK:
				cameraPorition.z = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, 0.f, 0.f });
				break;
			case Engine::View::FORWARD:
				cameraPorition.z = 100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, 180.f, 0.f });
				break;
			}

			m_Window.SetOrthoProjection(m_Settings.Projection_Near, m_Settings.Projection_Far);
			m_Window.SetZoom(1);
			m_Window.SetCurrentCamera(&m_OrthoCamera);
		}
	}
};