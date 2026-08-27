// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Windows.h>
#include <ProjectManager/ProjectManager.h>


ProjectManager::ProjectManager() :
	m_MainPage(this),
	m_CreationPage(this)
{
}

void ProjectManager::Initialize(const Settings& settings) {
	m_Settings = settings;

	const Nt::Int2D mainWindowSize = m_Settings.ProjectManagerWindow.Size;
	m_MainWindow.RemoveStyles(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	m_MainWindow.Create(mainWindowSize, "");

	m_MainPage.SetParent(m_MainWindow);
	m_MainPage.Initialize(mainWindowSize, settings);
	m_MainPage.Show();

	m_CreationPage.SetParent(m_MainWindow);
	m_CreationPage.Initialize(mainWindowSize, m_Settings);
}

void ProjectManager::Run() {
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

Project* ProjectManager::GetProjectPtr() const noexcept {
	return m_pProject;
}
