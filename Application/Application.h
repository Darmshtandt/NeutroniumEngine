#pragma once

class Application : private Nt::Window {
public:
	explicit Application(ProjectManager* projectManagerPtr) :
		m_ProjectManagerPtr(projectManagerPtr),
		m_pProject(nullptr),
		m_pEngine(new Engine),
		m_pObjectsTree(new ObjectsTree),
		m_pPropertyWindow(new PropertyWindow),
		m_pFileExplorer(new FileExplorer),
		m_IsChanged(false)
	{ 
	}
	~Application() {
		Nt::ResourceManager::Clear();

		SAFE_DELETE(&m_pObjectsTree);
		SAFE_DELETE(&m_pPropertyWindow);
		SAFE_DELETE(&m_pFileExplorer);
		SAFE_DELETE(&m_pEngine);
	}
	
	void Initialize(const Settings& settings) {
		m_Settings = settings;
		m_ProgramMenu.Initialize(m_Settings.CurrentLanguage);

		EnableMenu();
		AddExStyles(WS_EX_CONTROLPARENT);
		Create(m_Settings.MainWindowRect, APPLICATION_NAME);
		SetMenu(m_ProgramMenu.GetNtMenu());

		m_DefaultInitialPath = std::current_path();
		for (Int i = m_DefaultInitialPath.string().length() - 1; i >= 0; --i) {
			std::string path = m_DefaultInitialPath.string();
			if (path[i] == '\\') {
				path.erase(i + 1, path.length());
				m_DefaultInitialPath = path;
				break;
			}
		}

		SetIcon(m_DefaultInitialPath.string() + "Images\\Logo.ico");

		m_pObjectsTree->Initialize(m_Settings, m_DefaultInitialPath.string());
		m_pObjectsTree->SetParent(*this);
		m_pObjectsTree->Show();

		m_pEngine->Initialize(m_Settings, m_DefaultInitialPath.string());
		m_pEngine->SetParent(*this);
		m_pEngine->BindObjectsTree(m_pObjectsTree);
		m_pEngine->Show();

		m_pProject = m_ProjectManagerPtr->GetProjectPtr();
		if (m_pProject == nullptr)
			Raise("Project pointer is nullptr");

		m_RootPath = m_pProject->GetRootPath();
		std::current_path(std::path(m_RootPath));

		m_pPropertyWindow->Initialize(m_Settings, m_RootPath, m_pEngine->GetSelectorPtr(), m_pEngine->GetScencePtr());
		m_pPropertyWindow->SetParent(*this);
		m_pPropertyWindow->Show();

		m_pFileExplorer->Initialize(m_Settings);
		m_pFileExplorer->SetParent(*this);
		m_pFileExplorer->SetRootPath(m_RootPath);
		m_pFileExplorer->Show();
	}
	void Run() {
		ShowMaximized();

		Nt::Event event;
		while (IsOpened()) {
			while (PeekMessages(&event) || m_pObjectsTree->PeekMessages(&event)
				|| m_pPropertyWindow->PeekMessages(&event) || m_pFileExplorer->PeekMessages(&event)
				|| m_pEngine->PeekMessages(&event))
			{
				switch (event.Type) {
				case Nt::Event::WINDOW_RESIZE:
					m_Settings.ComputeWindowRect(GetWindowRect());

					m_pObjectsTree->SetRect(m_Settings.ObjectsTreeWindowRect);
					m_pPropertyWindow->SetWindowRect(m_Settings.PropertyWindowRect);
					m_pEngine->SetRect(m_Settings.EngineWindowRect);
					m_pFileExplorer->SetWindowRect(m_Settings.FileExplorerWindowRect);
					break;
				case Nt::Event::KEY_UP:
					switch (event.Value) {
					case Nt::Keyboard::KEY_F4:
						Close();
						goto PopingExit;
					}
					break;
				}
				m_pEngine->HandleEvent(event);
			}
		PopingExit:

			if (GetActiveWindow() != nullptr) {
				_Control();
				m_pPropertyWindow->Update();
				m_pEngine->Update();
				m_pEngine->Render();

				if (m_pEngine->IsChanged() != m_IsChanged) {
					m_IsChanged = m_pEngine->IsChanged();
					SetName((m_IsChanged) 
						? (L'*' + m_Name)
						: m_Name.substr(0, 1));
				}
			}
			else {
				if (m_pEngine->IsFly())
					m_pEngine->DisableFly();
				Sleep(10);
			}
		}
		std::current_path(m_DefaultInitialPath);
	}

	void Build() {
		Game::Config config;
		config.WindowName = m_pProject->GetName();
		config.ScencePath = Nt::OpenFileDialog(L"", L"Scence (*.ntascn)\0*.ntascn");

		if (config.ScencePath != "") {
			if (!IsValidPath(m_RootPath, config.ScencePath)) {
				WarningBox(L"To add a file, place it in the project's root folder.", L"Warning");
				return;
			}
			config.ScencePath.erase(config.ScencePath.begin(), config.ScencePath.begin() + m_RootPath.length() + 1);

			const std::path binPath(m_RootPath + "\\Bin\\");
			if ((!std::directory_entry(binPath).exists()) && (!std::create_directory(binPath))) {
				ErrorBox(L"Failed to create Bin directory.", L"Error");
				return;
			}

			std::ofstream configFile(m_RootPath + "\\Bin\\.gameconf");
			if (!configFile.is_open()) {
				ErrorBox(L"Failed to create game config file.", L"Error");
				return;
			}

			if (m_IsChanged)
				Save();

			config.Write(configFile);
			configFile.close();

			const std::string initialPath = m_DefaultInitialPath.string() + "\\";
			std::copy_file(std::path(initialPath + "Launcher.exe"), std::path(m_RootPath + "\\Bin\\Game.exe"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(initialPath + "OpenAL32.dll"), std::path(m_RootPath + "\\Bin\\OpenAL32.dll"), std::copy_options::overwrite_existing);
#ifdef _DEBUG
			std::copy_file(std::path(initialPath + "NeutroniumCore32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumCore32d.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(initialPath + "NeutroniumGraphics32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumGraphics32d.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(initialPath + "NeutroniumPhysics32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumPhysics32d.dll"), std::copy_options::overwrite_existing);
#else
			std::copy_file(std::path(initialPath + "NeutroniumCore32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumCore32.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(initialPath + "NeutroniumGraphics32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumGraphics32.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(initialPath + "NeutroniumPhysics32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumPhysics32.dll"), std::copy_options::overwrite_existing);
#endif


			InfoBox(L"Build was successful!", L"Build");
		}
	}
	void New() {
		m_pEngine->New();
	}
	void Open() {
		m_pEngine->Open();
	}
	void Save() {
		m_pEngine->Save(m_pProject->GetRootPath());
	}
	void SaveAs() {
		m_pEngine->SaveAs(m_pProject->GetRootPath());
	}

	void SetTheme(const std::string& fileName) {
		m_Settings.Styles.Load(m_DefaultInitialPath.string() + "\\..\\Themes\\" + fileName);
		m_pObjectsTree->SetTheme(m_Settings.Styles);
		m_pPropertyWindow->SetTheme(m_Settings.Styles);
		m_pEngine->SetTheme(m_Settings.Styles);
		m_pFileExplorer->SetTheme(m_Settings.Styles);
	}
	void SetLanguage(const std::string& fileName) {
		if (fileName == "en.ntelc")
			m_Settings.CurrentLanguage = Language();

		//m_Settings.CurrentLanguage.Load(m_DefaultInitialPath.string() + "\\..\\Locales\\" + fileName);
		m_pObjectsTree->SetLanguage(m_Settings.CurrentLanguage);
		m_pPropertyWindow->SetLanguage(m_Settings.CurrentLanguage);
		m_pEngine->SetLanguage(m_Settings.CurrentLanguage);
		m_pFileExplorer->SetLanguage(m_Settings.CurrentLanguage);
		m_ProgramMenu.SetLanguage(m_Settings.CurrentLanguage);
		
		m_ProgramMenu.SetLanguage(m_Settings.CurrentLanguage);
		DrawMenuBar(m_hwnd);
	}

private:
	Nt::String m_RootPath;
	std::path m_DefaultInitialPath;
	ProjectManager* m_ProjectManagerPtr;
	Project* m_pProject;
	FileExplorer* m_pFileExplorer;
	PropertyWindow* m_pPropertyWindow;
	ObjectsTree* m_pObjectsTree;
	Engine* m_pEngine;
	ProgramMenu m_ProgramMenu;
	Nt::Keyboard m_Keyboard;
	Settings m_Settings;
	Bool m_IsChanged;

private:
	void _Control() {
		const Bool isControlPressed =
			m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL, false);
		const Bool isShiftPressed =
			m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_SHIFT, false);

		m_Keyboard.Update();
		if (isControlPressed) {
			if (isShiftPressed) {
				if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_S, true))
					SaveAs();
				else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_T, true))
					m_pEngine->CloseTestGame();
			}
			else {
				if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_O, true))
					Open();
				else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_S, true))
					Save();
				else if (m_Keyboard.IsKeyPressed(Nt::Keyboard::KEY_T, true))
					m_pEngine->StartTestGame();
			}
		}
	}
	void _WMCommand(const Long& param_1, [[maybe_unused]] const Long& param_2) override {
		switch (param_1) {
		case ProgramMenu::MENU_BUILD:
			Build();
			break;
		case ProgramMenu::MENU_NEW:
			New();
			break;
		case ProgramMenu::MENU_OPEN:
			Open();
			break;
		case ProgramMenu::MENU_SAVE:
			Save();
			break;
		case ProgramMenu::MENU_SAVEAS:
			SaveAs();
			break;
		case ProgramMenu::MENU_CLOSE:
			break;

		case ProgramMenu::MENU_ENABLE_OBJECTS_TREE:
			break;
		case ProgramMenu::MENU_ENABLE_FILE_EXPLORER:
			break;
		case ProgramMenu::MENU_ENABLE_PROPERTY:
			break;

		case ProgramMenu::MENU_CREATE_PRIMITIV_CUBE:
			m_pEngine->CreatePrimitive(PrimitiveTypes::CUBE);
			break;
		case ProgramMenu::MENU_CREATE_PRIMITIV_QUAD:
			m_pEngine->CreatePrimitive(PrimitiveTypes::QUAD);
			break;
		case ProgramMenu::MENU_CREATE_PRIMITIV_PLANE:
			m_pEngine->CreatePrimitive(PrimitiveTypes::PLANE);
			break;
		case ProgramMenu::MENU_CREATE_PRIMITIV_PYRAMID:
			m_pEngine->CreatePrimitive(PrimitiveTypes::PYRAMID);
			break;
		case ProgramMenu::MENU_CREATE_ENTITY_CAMERA:
			m_pEngine->CreateEntity(EntityTypes::CAMERA);
			break;
		case ProgramMenu::MENU_CREATE_ENTITY_SOUND:
			m_pEngine->CreateEntity(EntityTypes::SOUND);
			break;
		case ProgramMenu::MENU_CREATE_ENTITY_MODEL:
			m_pEngine->CreateEntity(EntityTypes::MODEL);
			break;

		case ProgramMenu::MENU_THEME_SOLARIZED_DARK:
		case ProgramMenu::MENU_THEME_SOLARIZED_GREEN_DARK:
		case ProgramMenu::MENU_THEME_SOLARIZED_RED_DARK:
		case ProgramMenu::MENU_THEME_DARK:
		case ProgramMenu::MENU_THEME_BLACK:
		case ProgramMenu::MENU_THEME_WHITE:
			{
				const std::string styleNames[] = {
					"Solarized Dark.ntethm", "Solarized Green-Dark.ntethm",
					"Solarized Red-Dark.ntethm", "Dark.ntethm",
					"Black.ntethm", "White.ntethm"
				};

				SetTheme(styleNames[param_1 - ProgramMenu::MENU_THEME_SOLARIZED_DARK]);
				for (uInt i = ProgramMenu::MENU_THEME_SOLARIZED_DARK; i <= ProgramMenu::MENU_THEME_WHITE; ++i)
					m_ProgramMenu.GetNtMenu().CheckItem(i, (Long(i) == param_1));
			}
			break;

		case ProgramMenu::MENU_LANGUAGE_ENGLISH:
			SetLanguage("en.ntelc");
			break;
		case ProgramMenu::MENU_LANGUAGE_RUSSIAN:
			SetLanguage("ru.ntelc");
			break;
		case ProgramMenu::MENU_LANGUAGE_SLOVAK:
			SetLanguage("sk.ntelc");
			break;
		}
	}
};