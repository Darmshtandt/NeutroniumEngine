#pragma once

#include <ProjectManager/ProjectMainPage.h>
#include <ProjectManager/ProjectCreationPage.h>

class ProjectManager {
	friend class ProjectMainPage;
	friend class ProjectCreationPage;

public:
	ProjectManager();

	void Initialize(const Settings& settings);
	void Run();

	Project* GetProjectPtr() const noexcept;

private:
	ProjectMainPage m_MainPage;
	ProjectCreationPage m_CreationPage;
	Nt::Window m_MainWindow;
	Project* m_pProject;
	Settings m_Settings;
};