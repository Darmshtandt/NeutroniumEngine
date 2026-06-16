#pragma once

#include <InputContext.h>
#include <MultiStream.h>
#include <Nt/Core/EventBus.h>
#include <ResourceManager.h>
#include <Editor/WorldEditor.h>

class Application : private Nt::Window {
	using SharedInputContext = std::shared_ptr<InputContext>;
	using InputContextHandler = std::function<void(const SharedInputContext& context)>;

public:
	explicit Application(NotNull<ProjectManager*> projectManagerPtr) :
		m_ProjectManagerPtr(projectManagerPtr),
		m_InputContext(new InputContext)
	{
		SetEventBus(m_EventBus);

		m_InputContextMap["Application"] = m_InputContext;

		m_EventBus->Subscribe<AddInputContextEvent>([this] (const AddInputContextEvent& e) {
			if (m_InputContextMap.contains(e.Name)) {
				Raise("An input context with this name already exists: " + e.Name);
				return;
			}
			if (e.Context.expired()) {
				Raise("InputContext pointer is null");
				return;
			}

			m_InputContextMap[e.Name] = e.Context;
			});

		m_EventBus->Subscribe<KeyDownEvent>([this] (const KeyDownEvent& e) {
			ForEachInputContexts([e] (const SharedInputContext& context) {
				context->AddActiveKey(e.Key);
				});
			});
		m_EventBus->Subscribe<KeyUpEvent>([this] (const KeyUpEvent& e) {
			ForEachInputContexts([e] (const SharedInputContext& context) {
				context->RemoveActiveKey(e.Key);
				});
			});
		m_EventBus->Subscribe<ExceptionEvent>([this] (const ExceptionEvent& e) {
			(void)e;
			ForEachInputContexts([] (const SharedInputContext& context) {
				context->ClearActiveKeys();
				});
			});
		m_EventBus->Subscribe<KillFocusEvent>([this] (const KillFocusEvent& e) {
			(void)e;
			ForEachInputContexts([] (const SharedInputContext& context) {
				context->ClearActiveKeys();
				});
			});

		m_ObjectsTree.reset(new ObjectsTree(m_EventBus));
		m_FileExplorer.reset(new FileExplorer());
		m_Engine.reset(new Engine(m_EventBus));
	}
	~Application() noexcept override {
		m_EventBus->Clear();
		ResourceManager::Instance().Clear();
	}
	
	void Initialize(const Settings& settings) {
		Nt::Log::Instance().Message("Initialize Application");

		m_Settings = settings;

		m_InitialPath = m_Settings.Language.InitialPath;
		m_ProgramMenu = std::make_unique<ProgramMenu>(m_Settings.Language);

		EnableMenu();
		AddExStyles(WS_EX_CONTROLPARENT);
		Create(m_Settings.MainWindowRect, APPLICATION_NAME);
		SetMenu(m_ProgramMenu.get());
		SetIcon(m_InitialPath + "Images\\Logo.ico");

		m_Engine->Initialize(m_Settings, m_InitialPath);
		m_Engine->SetParent(*this);
		m_Engine->Show();

		m_ObjectsTree->Initialize(m_Settings, m_InitialPath);
		m_ObjectsTree->SetParent(*this);
		m_ObjectsTree->Show();

		m_pProject = RequireNotNull(m_ProjectManagerPtr->GetProjectPtr());

		m_RootPath = m_pProject->GetRootPath();
		std::current_path(std::path(m_RootPath));

		m_PropertyWindow.reset(new PropertyWindow(m_Engine->GetSelector(), m_Engine->GetScene()));
		m_PropertyWindow->Initialize(m_EventBus, m_Settings, m_RootPath);
		m_PropertyWindow->SetParent(*this);
		m_PropertyWindow->Show();

		m_FileExplorer->Initialize(m_Settings);
		m_FileExplorer->SetParent(*this);
		m_FileExplorer->SetRootPath(m_RootPath);
		m_FileExplorer->Show();

		m_pWorldEditor = m_Engine->GetWorldEditor();
		
		m_ProgramMenu->RegisterAction("Menu.File.Build", [this] () { Build(); });
		m_ProgramMenu->RegisterAction("Menu.File.New", [this] () { m_pWorldEditor->New(); });
		m_ProgramMenu->RegisterAction("Menu.File.Open", [this] () { m_pWorldEditor->Open(); });
		m_ProgramMenu->RegisterAction("Menu.File.Save", [this] () { m_pWorldEditor->Save(); });
		m_ProgramMenu->RegisterAction("Menu.File.SaveAs", [this] () { m_pWorldEditor->SaveAs(); });

		m_ProgramMenu->AttachEditor(m_pWorldEditor);
		m_ProgramMenu->AttachStyle(&m_Settings.Style);

		Style::MessageBus::Instance().Subscribe(TOPIC_STYLE_CHANGE, [this] (Style* pStyle) {
			SetTheme(*pStyle);
			});

		SetTheme(m_Settings.Style);


		m_InputContext->AddHotKey(
			{ Nt::KEY_CONTROL, Nt::KEY_SHIFT, Nt::KEY_S }, [this] () { m_pWorldEditor->SaveAs(); });

		m_InputContext->AddHotKey(
			{ Nt::KEY_CONTROL, Nt::KEY_O }, [this] () { m_pWorldEditor->Open(); });

		m_InputContext->AddHotKey(
			{ Nt::KEY_CONTROL, Nt::KEY_S }, [this] () { m_pWorldEditor->Save(); });

		m_InputContext->AddHotKey(
			{ Nt::KEY_CONTROL, Nt::KEY_F4 }, [this] () { Close(); });

		m_IsInitialized = true;

		Nt::Log::Instance().Successful("Application initialized");
	}
	void Run() {
		Nt::Log::Instance().Message("Run Application");

		ShowMaximized();

		while (IsOpened()) {
			PeekMessages();

			if (GetActiveWindow() != nullptr) {
				m_Keyboard.Update();
				m_PropertyWindow->Update();
				m_Engine->Update();
				m_Engine->Render();

				if (m_pWorldEditor->IsChanged() != m_IsChanged) {
					m_IsChanged = m_pWorldEditor->IsChanged();
					SetName((m_IsChanged) 
						? (L'*' + m_Name)
						: m_Name.substr(0, 1));
				}
			}
			else {
				m_Engine->DeactivateWindow();

				Sleep(10);
			}
		}

		std::current_path(m_InitialPath);
	}

	void Build() {
		Nt::Log::Instance().Message("Build started");

		try {
			Game::Config config;
			config.WindowName = m_pProject->GetName();
			config.ScenePath = Nt::OpenFileDialog(L"", L"Scene (*.ntascn)\0*.ntascn");

			if (!config.ScenePath.empty())
				Raise("Scene path is empty");

			if (!IsValidPath(m_RootPath, config.ScenePath))
				Raise("To add a file, place it in the project's root folder");

			config.ScenePath.erase(config.ScenePath.begin(), config.ScenePath.begin() + m_RootPath.length() + 1);

			const std::path binPath(m_RootPath + "\\Bin\\");
			if ((!std::directory_entry(binPath).exists()) && (!std::create_directory(binPath)))
				Raise("Failed to create Bin directory");

			std::ofstream configFile(m_RootPath + "\\Bin\\.gameconf");
			if (!configFile.is_open())
				Raise("Failed to create game config file");

			if (m_IsChanged)
				m_pWorldEditor->Save();

			config.Write(configFile);
			configFile.close();

			std::copy_file(std::path(m_InitialPath + "Launcher.exe"), std::path(m_RootPath + "\\Bin\\Game.exe"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(m_InitialPath + "OpenAL32.dll"), std::path(m_RootPath + "\\Bin\\OpenAL32.dll"), std::copy_options::overwrite_existing);

#ifdef _DEBUG
			std::copy_file(std::path(m_InitialPath + "NeutroniumCore32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumCore32d.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(m_InitialPath + "NeutroniumGraphics32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumGraphics32d.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(m_InitialPath + "NeutroniumPhysics32d.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumPhysics32d.dll"), std::copy_options::overwrite_existing);
#else
			std::copy_file(std::path(m_InitialPath + "NeutroniumCore32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumCore32.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(m_InitialPath + "NeutroniumGraphics32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumGraphics32.dll"), std::copy_options::overwrite_existing);
			std::copy_file(std::path(m_InitialPath + "NeutroniumPhysics32.dll"), std::path(m_RootPath + "\\Bin\\NeutroniumPhysics32.dll"), std::copy_options::overwrite_existing);
#endif

			Nt::Log::Instance().Successful("Build completed");
			Nt::MessageWindow("Build was successful!", "Build").ShowInfo();
		}
		catch (const Nt::Error& error) {
			Nt::Log::Instance().Error(error.what());
			Nt::Log::Instance().Message("Build failed");
			Nt::MessageWindow(error.what(), "Error").ShowError();
		}
	}

	void SetTheme(const Style& style) {
		m_Settings.Style = style;

		m_ObjectsTree->SetTheme(m_Settings.Style);
		m_PropertyWindow->SetTheme(m_Settings.Style);
		m_Engine->SetTheme(m_Settings.Style);
		m_FileExplorer->SetTheme(m_Settings.Style);
	}
	void SetLanguage(const std::string& fileName) {
		m_Settings.Language.LoadFromFile(fileName);

		m_ObjectsTree->SetLanguage(m_Settings.Language);
		m_PropertyWindow->SetLanguage(m_Settings.Language);
		m_Engine->SetLanguage(m_Settings.Language);
		m_FileExplorer->SetLanguage(m_Settings.Language);
		m_ProgramMenu->SetLanguage(m_Settings.Language);
		
		DrawMenuBar(m_hwnd);
	}

private:
	std::shared_ptr<Nt::EventBus> m_EventBus = std::make_shared<Nt::EventBus>();
	std::unique_ptr<ProgramMenu> m_ProgramMenu;
	std::shared_ptr<InputContext> m_InputContext;
	std::unordered_map<std::string, std::weak_ptr<InputContext>> m_InputContextMap;
	Nt::String m_RootPath;
	std::string m_InitialPath;
	ProjectManager* m_ProjectManagerPtr;
	Project* m_pProject = nullptr;

	std::unique_ptr<FileExplorer> m_FileExplorer;
	std::unique_ptr<ObjectsTree> m_ObjectsTree;
	std::unique_ptr<Engine> m_Engine;
	std::unique_ptr<PropertyWindow> m_PropertyWindow;
	WorldEditor* m_pWorldEditor = nullptr;

	Nt::Keyboard m_Keyboard;
	Settings m_Settings;
	Bool m_IsChanged = false;
	Bool m_IsInitialized = false;

private:
	void _Resize(const Nt::uInt2D& windowSize) override {
		(void)windowSize;
		if (!m_IsInitialized)
			return;

		m_Settings.ComputeWindowRect(GetWindowRect());

		m_ObjectsTree->SetRect(m_Settings.ObjectsTreeWindowRect);
		m_PropertyWindow->SetWindowRect(m_Settings.PropertyWindowRect);
		m_Engine->SetRect(m_Settings.EngineWindowRect);
		m_FileExplorer->SetWindowRect(m_Settings.FileExplorerWindowRect);
	}
	void _Command(const Long& param_1, const Long& param_2) override {
		(void)param_2;
		m_ProgramMenu->ExecuteAction(param_1);
	}
	void ForEachInputContexts(const InputContextHandler& handler) {
		Assert(handler, "Invalid handler");

		for (auto it = m_InputContextMap.begin(); it != m_InputContextMap.end();) {
			if (it->second.expired()) {
				it = m_InputContextMap.erase(it);
				continue;
			}

			handler(it->second.lock());
			++it;
		}
	}
};