#pragma once

#include <Main.h>
#include <Settings.h>

class ProjectCreationPage : private Nt::Window {
	friend class ProjectMainPage;
	friend class ProjectManager;

public:
	const Int ID = 2010;

	enum Elements {
		TEXTEDIT_PROJECTNAME,
		TEXTEDIT_PROJECTPATH,
		BUTTON_BROWSE,
		BUTTON_BACK,
		BUTTON_CREATE,
	};

public:
	ProjectCreationPage(NotNull<ProjectManager*> pProjectManager);

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings);

private:
	ProjectManager* m_ProjectManagerPtr = nullptr;
	Nt::Text m_ProjectNameText;
	Nt::Text m_ProjectPathText;
	Nt::Button m_BrowseButton;
	Nt::Button m_BackButton;
	Nt::Button m_CreateButton;
	Nt::TextEdit m_InputProjectName;
	Nt::TextEdit m_InputProjectPath;
	Nt::String m_DefaultProjectName;
	Nt::String m_DefaultProjectPath;

private:
	void _ClearInputs();
	void _CreateProject();
	void _OnButtonPressed(const uInt& command, const uInt& id, const HWND& hButton);

	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2);
};