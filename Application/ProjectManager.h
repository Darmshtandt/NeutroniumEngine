#pragma once

class ProjectManager {
	friend class ProjectMainPage;
	friend class ProjectCreationPage;

public:
	ProjectManager() = default;

	void Initialize(const Settings& settings) {
		m_Settings = settings;
		const Style::ProjectManagerWindow projectManager = m_Settings.Styles.ProjectManager;

		const Nt::Int2D mainWindowSize = m_Settings.ProjectManagerWindow.Size;
		m_MainWindow.RemoveStyles(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		m_MainWindow.Create(mainWindowSize, "");

		m_MainPage.SetParent(m_MainWindow);
		m_MainPage.Initialize(mainWindowSize, settings);
		m_MainPage.Show();
		
		m_CreationPage.SetParent(m_MainWindow);
		m_CreationPage.Initialize(mainWindowSize, m_Settings);

		m_MainPage.m_ProjectManagerPtr = this;
		m_CreationPage.m_ProjectManagerPtr = this;
	}
	void Run() {
		m_MainWindow.Show();

		Nt::Event event;
		while (m_MainWindow.IsOpened()) {
			while (m_MainWindow.PeekMessages(&event)) {
				switch (event.Type) {
				case Nt::Event::WINDOW_CLOSE:
					m_CreationPage.Hide();
					m_MainPage.Show();
					break;
				}
			}
			Sleep(10);
		}
	}

	Project* GetProjectPtr() const noexcept {
		return m_pProject;
	}

private:
	ProjectMainPage m_MainPage;
	ProjectCreationPage m_CreationPage;
	Nt::Window m_MainWindow;
	Project* m_pProject;
	Settings m_Settings;
};