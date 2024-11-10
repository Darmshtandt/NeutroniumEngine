#pragma once

class Engine {
private:
	enum class ViewMode {
		PERSPECTIVE,
		TOP, BOTTOM,
		LEFT, RIGHT,
		BACK, FORWARD
	};

	enum ControlElements {
		CONTROL_COMBOBOX_PROJECTION,
		CONTROL_TEXTEDIT_GRIDCELLSIZE,
		CONTROL_BUTTON_TOGGLEGRID,
	};

	enum ImageIDs {
		TEXTURE_DEFAULT,
		TEXTURE_CAMERA,
		TEXTURE_SOUND,
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

	void Initialize(const Settings& settings, const Nt::String& defaultInitialPath) {
		const uInt pathType = GetFileAttributesA(defaultInitialPath);
		if (pathType == INVALID_FILE_ATTRIBUTES)
			Raise(std::string("A non-existent path was specified: ") + defaultInitialPath);
		else if (!(pathType & FILE_ATTRIBUTE_DIRECTORY))
			Raise(std::string("This path is not a directory: ") + defaultInitialPath);

		m_Settings = settings;

		m_Window.RemoveStyles(WS_OVERLAPPEDWINDOW);
		m_Window.AddStyles(WS_DLGFRAME);
		m_Window.Create(m_Settings.EngineWindowRect, settings.CurrentLanguage.Window.Texts[Language::_WindowNames::TEXT_ENGINEWINDOW]);
		m_Window.SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) -> Long {
			return _Procedure(uMsg, param_1, param_2);
			});

		Nt::ResourceManager::Add(new Nt::Texture(defaultInitialPath + "Images\\Tiles.tga"));
		Nt::ResourceManager::Add(new Nt::Texture(defaultInitialPath + "Images\\Camera.tga"));
		Nt::ResourceManager::Add(new Nt::Texture(defaultInitialPath + "Images\\Sound.tga"));

		{
			m_ComboBoxView.AddStyles(CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SIMPLE);
			m_ComboBoxView.SetID(CONTROL_COMBOBOX_PROJECTION);
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
			m_ComboBoxView.Show();

			Nt::IntRect gridTextEditRect;
			gridTextEditRect.RightBottom = { 100, 24 };
			gridTextEditRect.Left = m_Window.GetWindowRect().Right - gridTextEditRect.Right - 25;
			gridTextEditRect.Top = 5;

			m_GridTextEdit.SetID(CONTROL_TEXTEDIT_GRIDCELLSIZE);
			m_GridTextEdit.SetParent(m_Window);
			m_GridTextEdit.Create(gridTextEditRect, "1.0", true);
			m_GridTextEdit.Show();

			Nt::IntRect gridButtonRect;
			gridButtonRect.RightBottom = Nt::Int2D(gridTextEditRect.Bottom, gridTextEditRect.Bottom);
			gridButtonRect.LeftTop = gridTextEditRect.LeftTop;
			gridButtonRect.Left -= gridButtonRect.Right;

			Nt::GDI::Bitmap gridIcon;
			gridIcon.LoadFromFile(defaultInitialPath + "Images\\GridIcon.bmp");

			m_GridButton.AddStyles(BS_LEFT | BS_BITMAP | BS_VCENTER | BS_AUTOCHECKBOX | BS_PUSHLIKE);
			m_GridButton.SetID(CONTROL_BUTTON_TOGGLEGRID);
			m_GridButton.SetParent(m_Window);
			m_GridButton.Create(gridButtonRect, "");
			m_GridButton.SetImage(gridIcon);
			m_GridButton.Show();
		}

		m_pScence = new Scence;
		m_Selector.Initialize(m_pScence, defaultInitialPath);

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

	void CreatePrimitive(const PrimitiveTypes& type) {
		constexpr Nt::Float3D size(1.f, 1.f, 1.f);
		Object* pObject = new Primitive("", type, size);
		pObject->SetTexture(TEXTURE_DEFAULT);

		switch (type) {
		case PrimitiveTypes::CUBE:
			pObject->SetName("Cube");
			break;
		case PrimitiveTypes::QUAD:
			pObject->SetName("Quad");
			break;
		case PrimitiveTypes::PLANE:
			pObject->SetName("Plane");
			break;
		case PrimitiveTypes::PYRAMID:
			pObject->SetName("Pyramid");
			break;
		}

		m_pScence->AddObject(pObject);
		m_IsChanged = true;
	}
	void CreateEntity(const EntityTypes& type) {
		Object* pObject = nullptr;

		switch (type) {
		case EntityTypes::CAMERA:
			pObject = new GameCamera("Camera");
			pObject->SetTexture(TEXTURE_CAMERA);
			break;
		case EntityTypes::SOUND:
			pObject = new GameSound("Sound");
			pObject->SetTexture(TEXTURE_SOUND);
			break;
		case EntityTypes::MODEL:
			pObject = new GameModel("Model");
			break;
		default:
			Raise("Error primitive type");
			break;
		}

		m_pScence->AddObject(pObject);
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
				CreateEntity(EntityTypes::CAMERA);
				break;
			case Nt::Keyboard::KEY_ADD:
				CreatePrimitive(PrimitiveTypes::CUBE);
				break;
			}
			break;
		}
	}

	void Update() {
		const Float time = Float(m_Window.GetFrameTime()) / 1000.f;
		if (m_pGame->IsLaunched()) {
			m_pGame->Update(time);
		}
		else {
			_Control();
			m_Selector.Update(m_Camera.GetPosition());
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
			m_Selector.Render(&m_Window);
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
		m_Window.SetName(language.Window.Texts[Language::_WindowNames::TEXT_ENGINEWINDOW]);
	}

private:
	Nt::RenderWindow m_Window;
	Nt::Shader m_Shader;
	Nt::Camera m_Camera;
	Nt::Camera m_OrthoCamera;
	Nt::Keyboard m_Keyboard;
	Nt::Mouse m_Mouse;

	Nt::ComboBox m_ComboBoxView;
	Nt::Button m_GridButton;
	Nt::TextEdit m_GridTextEdit;

	Game* m_pGame;
	Selector m_Selector;
	ObjectsTree* m_ObjectsTreePtr;
	Scence* m_pScence;
	Settings m_Settings;
	Nt::String m_FilePath;
	ViewMode m_Projection;
	Float m_CameraSpeed;

	Bool m_IsFly;
	Bool m_IsChanged;
	Bool m_IsOrtoEnabled;

private:
	void _ResertCamera() {
		m_Camera.SetPosition({ 0.f, -3.f, -5.f });
		m_Camera.SetAngle({ -35.f, 0.f, 0.f });
	}

	void _Control() {
		m_Keyboard.Update();
		m_Mouse.Update();
		
		const Bool isPressedControl = m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL, false);

		if (m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_RIGHT, true))
			ToggleFly();

		if (m_IsFly) {
			Nt::Float3D move;
			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_W, false)) {
				move.x += sinf(m_Camera.GetAngle().y * RADf);
				move.z += cosf(m_Camera.GetAngle().y * RADf);
			}
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_S, false)) {
				move.x -= sinf(m_Camera.GetAngle().y * RADf);
				move.z -= cosf(m_Camera.GetAngle().y * RADf);
			}

			if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_A, false)) {
				move.x += sinf((m_Camera.GetAngle().y + 90.f) * RADf);
				move.z += cosf((m_Camera.GetAngle().y + 90.f) * RADf);
			}
			else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_D, false)) {
				move.x += sinf((m_Camera.GetAngle().y - 90.f) * RADf);
				move.z += cosf((m_Camera.GetAngle().y - 90.f) * RADf);
			}

			if (!isPressedControl) {
				if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SPACE, false))
					move.y = -1.f;
				else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SHIFT, false))
					move.y = 1.f;
			}
			m_Camera.Translate(move * m_CameraSpeed);

			const Nt::Float2D cursorPosition = Nt::GetCursorPosition();
			const Nt::Float2D screenCenter = Nt::GetMonitorSize() / 2;
			const Nt::Float2D angleRotation = -((cursorPosition - screenCenter) / 6.f);
			m_Camera.Rotate({ angleRotation.y, angleRotation.x, 0.f });
			SetCursorPos(Int(screenCenter.x), Int(screenCenter.y));
		}
		else {
			m_Selector.Control(&m_Window, m_Camera.GetPosition(), m_Camera.GetAngle(), m_Keyboard, m_Mouse);

			if (isPressedControl && m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_J, true))
				_CSG();
		}

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
	void _CSG() {
		if (m_Selector.GetObjects().size() > 1) {
			//using Kernel = CGAL::Homogeneous<CGAL::Exact_integer>;
			//using NefPolyhedron = CGAL::Nef_polyhedron_3<Kernel>;
			//using Polyhedron = CGAL::Polyhedron_3<Kernel>;
			//using SurfaceMesh = CGAL::Surface_mesh<CGAL::Exact_predicates_exact_constructions_kernel::Point_3>;

			/*NefPolyhedron nefOperationResult;
			for (const Object* pObject : m_Selector.GetObjects()) {
				if (pObject->GetModel().GetMeshPtr() == nullptr)
					Raise("Model mesh pointer is null. Object name: " + pObject->GetName());

				const Nt::Shape shape = pObject->GetModel().GetMeshPtr()->GetShape();

				Nt::String stringData = "OFF\n";
				stringData += Nt::String(shape.Vertices.size()) + ' ' + Nt::String(shape.Indices.size() / 3) + " 0\n";

				for (const Nt::Vertex& vertex : shape.Vertices) {
					stringData += Nt::String(vertex.Position.x) + ' ';
					stringData += Nt::String(vertex.Position.y) + ' ';
					stringData += Nt::String(vertex.Position.z) + '\n';
				}
				for (uInt i = 2; i < shape.Indices.size(); i += 3) {
					stringData += "3  ";
					stringData += Nt::String(shape.Indices[i - 2]) + ' ';
					stringData += Nt::String(shape.Indices[i - 1]) + ' ';
					stringData += Nt::String(shape.Indices[i - 0]) + '\n';
				}

				NefPolyhedron nefPolyhedron;
				{
					std::stringstream stream(stringData);
					stream >> nefPolyhedron;
				}

				nefOperationResult += nefPolyhedron;
			}*/

			//SurfaceMesh surfaceMesh;
			////CGAL::convert_nef_polyhedron_to_polygon_mesh(nefOperationResult, surfaceMesh);
			//
			//Nt::Shape newShape;
			//for (const CGAL::SM_Vertex_index& cgalVertex : surfaceMesh.vertices()) {
			//	const CGAL::Epeck::Point_3 point = surfaceMesh.point(cgalVertex);

			//	Nt::Vertex ntVertex;
			//	ntVertex.Position.x = (Float)CGAL::to_double(point.x());
			//	ntVertex.Position.y = (Float)CGAL::to_double(point.y());
			//	ntVertex.Position.z = (Float)CGAL::to_double(point.z());
			//	newShape.Vertices.push_back(ntVertex);
			//}

			//for (const CGAL::SM_Face_index& face : surfaceMesh.faces()) {
			//	for (const CGAL::SM_Vertex_index& vertex : surfaceMesh.vertices_around_face(surfaceMesh.halfedge(face)))
			//		newShape.Indices.push_back(vertex.id());
			//}

			//Object* pNewObject = new Object(ObjectTypes::PRIMITIVE, m_Selector.GetObjects()[0]->GetName(), newShape);
			//m_pScence->RemoveSelected(&m_Selector);
			//m_pScence->AddObject(pNewObject);
			//m_Selector.Select(pNewObject);
		}
	}
	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
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
					m_Selector.SetGridCellSize(m_GridTextEdit.GetText());
					break;
				}
				break;
			case BN_CLICKED:
				switch (id) {
				case CONTROL_BUTTON_TOGGLEGRID:
					m_Selector.ToggleSnapToGrid(m_GridButton.IsChecked());
					break;
				}
				break;
			}
			break;
		}
		return DefWindowProc(m_Window.GetHandle(), uMsg, param_1, param_2);
	}
	void _UpdateProjection() {
		if (m_Projection == Engine::ViewMode::PERSPECTIVE) {
			m_Window.SetPerspectiveProjection(
				m_Settings.Projection_FOV,
				m_Settings.Projection_Near,
				m_Settings.Projection_Far);
			m_Window.SetCurrentCamera(&m_Camera);
		}
		else {
			Nt::Float3D cameraPorition = m_Camera.GetPosition();
			switch (m_Projection) {
			case Engine::ViewMode::TOP:
				cameraPorition.y = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ -90.f, 0.f, 0.f });
				break;
			case Engine::ViewMode::BOTTOM:
				cameraPorition.y = 100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 90.f, 0.f, 0.f });
				break;
			case Engine::ViewMode::LEFT:
				cameraPorition.x = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, 90.f, 0.f });
				break;
			case Engine::ViewMode::RIGHT:
				cameraPorition.x = 100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, -90.f, 0.f });
				break;
			case Engine::ViewMode::BACK:
				cameraPorition.z = -100.f;
				m_OrthoCamera.SetPosition(cameraPorition);
				m_OrthoCamera.SetAngle({ 0.f, 0.f, 0.f });
				break;
			case Engine::ViewMode::FORWARD:
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