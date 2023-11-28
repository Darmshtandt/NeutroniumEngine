#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Project.h>
#include <ProjectMenager.h>


void ProjectMainPage::_OpenProject(const uInt& index) {
	if (index != -1) {
		m_ProjectMenagerPtr->m_pProject = reinterpret_cast<Project*>(
			m_ProjectList.GetItemData(index));
		m_ProjectMenagerPtr->m_pProject->Load();
		m_ProjectMenagerPtr->m_MainWindow.Close();
	}
}

void ProjectMainPage::_OnButtonEvent(const uInt& command, const uInt& id, const HWND& hButton) {
	if (command == BN_CLICKED) {
		if (m_ProjectMenagerPtr == nullptr)
			Raise("ProjectMenager pointer is nullptr");

		const Int selectedIndex = m_ProjectList.GetSelectionIndex();
		switch (id - ID) {
		case ActionWindow::BUTTON_OPEN:
			_OpenProject(selectedIndex);
			break;
		case ActionWindow::BUTTON_DELETE:
		{
			const uInt result = MessageBox(nullptr, L"Are you sure you want to delete the project?",
				L"Warning", MB_ICONWARNING | MB_YESNO);
			if (result == IDYES && selectedIndex != -1) {
				std::path rootPath = m_Projects[selectedIndex]->GetRootPath().c_str();
				std::remove_all(rootPath);
				_RemoveFromProjectList(selectedIndex);
			}
			m_ActionWindow.DisableButtons();
		}
			break;
		case ActionWindow::BUTTON_CREATE:
			Hide();
			m_ProjectMenagerPtr->m_CreationPage.Show();
			break;
		case ActionWindow::BUTTON_REMOVE_FROM_LIST:
			if (selectedIndex != -1)
				_RemoveFromProjectList(selectedIndex);
			m_ActionWindow.DisableButtons();
			break;
		case ActionWindow::BUTTON_ADD_TO_LIST:
		{
			Nt::String defaultPath = 
				m_ProjectMenagerPtr->m_CreationPage.m_DefaultProjectPath;
			if (*(defaultPath.end() - 1) == '\\')
				defaultPath.erase(defaultPath.end() - 1);
			
			const Nt::String newPath = Nt::OpenFileDialog(
				defaultPath.wstr().c_str(), L"Project file (*.nteproj)\0*.nteproj");
			if (newPath.length() > 0) {
				Bool IsExists = false;
				for (Project* pProject : m_Projects) {
					if (pProject->GetPath() == newPath) {
						IsExists = true;
						break;
					}
				}

				if (!IsExists) {
					Project* pProject = new Project;
					pProject->Load(newPath);
					m_Projects.push_back(pProject);
					_AddProjectToList(pProject);
				}
			}
		}
			break;
		}
	}
}

void ProjectCreationPage::_CreateProject() {
	try {
		Nt::String name = m_InputProjectName.GetText();
		if (name.length() == 0)
			Raise("Incorrect project name.");

		Nt::String path = m_InputProjectPath.GetText();
		if (GetFileAttributes(path.wstr().c_str()) == INVALID_FILE_ATTRIBUTES)
			Raise("Incorrect project path.");

		m_ProjectMenagerPtr->m_pProject = new Project;
		m_ProjectMenagerPtr->m_pProject->Create(name, path);
		m_ProjectMenagerPtr->m_MainPage.m_Projects.push_back(m_ProjectMenagerPtr->m_pProject);
		m_ProjectMenagerPtr->m_MainWindow.Close();
		_ClearInputs();
	}
	catch (const Nt::Error& except) {
		ErrorBoxA(except.what(), "Error");
	}
}

void ProjectCreationPage::_OnButtonPressed(const uInt& command, const uInt& id, const HWND& hButton) {
	if (command == BN_CLICKED) {
		if (m_ProjectMenagerPtr == nullptr)
			Raise("ProjectMenager pointer is nullptr");
		
		switch (id - ID) {
		case ProjectCreationPage::BUTTON_BROWSE:
			m_InputProjectPath.SetText(
				Nt::OpenFileDialog(m_InputProjectPath.GetText(), FOS_PICKFOLDERS));
			break;
		case ProjectCreationPage::BUTTON_BACK:
			m_ProjectMenagerPtr->m_MainPage.Show();
			Hide();
			_ClearInputs();
			break;
		case ProjectCreationPage::BUTTON_CREATE:
			_CreateProject();
			break;
		}
	}
}