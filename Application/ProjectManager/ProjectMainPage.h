#pragma once

#include <Nt/Graphics/System/WindowElements/ListBox.h>
#include <Nt/Graphics/System/WindowElements/Text.h>
#include <ProjectManager/Project.h>
#include <ProjectManager/ActionWindow.h>

class ProjectCreationPage;
class ProjectManager;

class ProjectMainPage : private Nt::Window {
	friend class ProjectCreationPage;
	friend class ProjectManager;

public:
	const Int ID = 2000;

public:
	ProjectMainPage(NotNull<ProjectManager*> projectManagerPtr);
	~ProjectMainPage();

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings);

	void SaveData();

	void OpenProject(const uInt& index);
	void RemoveProject(const uInt& index);

private:
	ProjectManager* m_ProjectManagerPtr;
	ActionWindow m_ActionWindow;
	Nt::ListBox m_ProjectList;
	Nt::Text m_ProjectListTextHeader;
	Nt::Text m_ProjectListTextDescription;
	Nt::Int2D m_ProjectListItemPadding;
	std::vector<Project*> m_Projects;

private:
	void _AddProjectToList(NotNull<const Project*> pProject);
	void _RemoveProjectFromList(const uInt& index);
	void _AddProjectToList(const Nt::String& path);

	void _OnButtonEvent(const uInt& command, const uInt& id, const HWND& hButton);

	void _OnListItemEvent(const uInt& command, [[maybe_unused]] const uInt& id, const HWND& hListBox);
	void _OnDrawItem([[maybe_unused]] const uInt& uMsg, [[maybe_unused]] const DWord& param_1, const DWord& param_2);

	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2);
};