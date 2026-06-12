// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ProjectManager/ProjectManager.h>


ProjectCreationPage::ProjectCreationPage(NotNull<ProjectManager*> pProjectManager) :
	m_ProjectManagerPtr(pProjectManager)
{
}

void ProjectCreationPage::Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
	const Nt::IntRect rect(Nt::Int2D(), windowSize);

	SetBackgroundColor(settings.Style["ProjectManager.CreationWindow.BackgroundColor"]);
	RemoveStyles(WS_OVERLAPPEDWINDOW);
	SetProcedure([&] (const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		return _Procedure(uMsg, param_1, param_2);
		});
	Create(rect, "Create");

	Nt::Int2D textPosition;
	textPosition.x = 10;
	textPosition.y = rect.Bottom / 4;

	m_ProjectNameText.AddFormat(DT_SINGLELINE);
	m_ProjectNameText.SetColor(settings.Style["ProjectManager.Headers.Color"]);
	m_ProjectNameText.SetWeight(settings.Style["ProjectManager.Headers.Weight"]);
	m_ProjectNameText.SetPosition(textPosition);
	m_ProjectNameText.SetText(settings.Language["ProjectManager.ProjectName"]);

	Nt::IntRect textEditRect(textPosition, Nt::Int2D());
	textEditRect.Top += 20;
	textEditRect.Right = rect.Right / 3;
	textEditRect.Bottom = 20;

	m_DefaultProjectName = "New project";
	m_InputProjectName.SetID(TEXTEDIT_PROJECTNAME);
	m_InputProjectName.SetParent(*this);
	m_InputProjectName.Create(textEditRect, m_DefaultProjectName, true);
	m_InputProjectName.Show();

	textEditRect.Top += 50;
	textPosition.y += 50;

	m_ProjectPathText.AddFormat(DT_SINGLELINE);
	m_ProjectPathText.SetColor(settings.Style["ProjectManager.Headers.Color"]);
	m_ProjectPathText.SetWeight(settings.Style["ProjectManager.Headers.Weight"]);
	m_ProjectPathText.SetPosition(textPosition);
	m_ProjectPathText.SetText(settings.Language["ProjectManager.ProjectPath"]);

	Nt::IntRect buttonRect;
	buttonRect.Right = 30;
	buttonRect.Bottom = textEditRect.Bottom;
	buttonRect.Left = textEditRect.Left + textEditRect.Right - buttonRect.Right;
	buttonRect.Top = textEditRect.Top;

	wChar* pPath;
	SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pPath);
	if (pPath == nullptr) {
		pPath = const_cast<wChar*>(L"");
	}
	else {
		m_DefaultProjectPath = pPath;
		m_DefaultProjectPath += "\\Neutronium Engine";

		std::path rootPath(m_DefaultProjectPath);
		if (!std::create_directory(rootPath)) {
			const std::wstring wPath = m_DefaultProjectPath;
			if (GetFileAttributes(wPath.c_str()) == INVALID_FILE_ATTRIBUTES)
				m_DefaultProjectPath = pPath;
		}
		m_DefaultProjectPath += '\\';
	}

	textEditRect.Right -= buttonRect.Right + 5;
	m_InputProjectPath.SetID(TEXTEDIT_PROJECTPATH);
	m_InputProjectPath.SetParent(*this);
	m_InputProjectPath.Create(textEditRect, m_DefaultProjectPath, true);
	m_InputProjectPath.Show();

	m_BrowseButton.SetBackgroundColor(settings.Style["ProjectManager.Buttons.BackgroundColor"]);
	m_BrowseButton.SetParent(*this);
	m_BrowseButton.SetID(ID + BUTTON_BROWSE);
	m_BrowseButton.Create(buttonRect, "...");
	m_BrowseButton.Show();

	buttonRect.RightBottom = { 75, 30 };
	buttonRect.Left = textEditRect.Left;
	buttonRect.Top = rect.Bottom - 16 - 20;
	buttonRect.Top -= buttonRect.Left + buttonRect.Bottom;

	m_BackButton.SetBackgroundColor(settings.Style["ProjectManager.Buttons.BackgroundColor"]);
	m_BackButton.SetParent(*this);
	m_BackButton.SetID(ID + BUTTON_BACK);
	m_BackButton.Create(buttonRect, "Back");
	m_BackButton.Show();

	buttonRect.Left = rect.Right - 16;
	buttonRect.Left -= textEditRect.Left + buttonRect.Right;

	m_CreateButton.SetBackgroundColor(settings.Style["ProjectManager.Buttons.BackgroundColor"]);
	m_CreateButton.SetParent(*this);
	m_CreateButton.SetID(ID + BUTTON_CREATE);
	m_CreateButton.Create(buttonRect, "Create");
	m_CreateButton.Show();
}

void ProjectCreationPage::_ClearInputs() {
	m_InputProjectName.SetText(m_DefaultProjectName);
	m_InputProjectPath.SetText(m_DefaultProjectPath);
}

void ProjectCreationPage::_CreateProject() {
	try {
		const Nt::String name = m_InputProjectName.GetText();
		Assert(!name.empty(), "Incorrect project name.");

		const std::wstring path = m_InputProjectPath.GetText();
		if (GetFileAttributes(path.c_str()) == INVALID_FILE_ATTRIBUTES)
			Raise("Incorrect project path.");

		m_ProjectManagerPtr->m_pProject = new Project;
		m_ProjectManagerPtr->m_pProject->Create(name, path);

		m_ProjectManagerPtr->m_MainPage.m_Projects.push_back(m_ProjectManagerPtr->m_pProject);
		m_ProjectManagerPtr->m_MainWindow.Close();

		_ClearInputs();
	}
	catch (const Nt::Error& except) {
		except.Show();
	}
}

void ProjectCreationPage::_OnButtonPressed(const uInt& command, const uInt& id, [[maybe_unused]] const HWND& hButton) {
	if (command != BN_CLICKED)
		return;

	switch (id - ID) {
	case ProjectCreationPage::BUTTON_BROWSE:
		m_InputProjectPath.SetText(
			Nt::OpenFileDialog(m_InputProjectPath.GetText(), FOS_PICKFOLDERS));
		break;

	case ProjectCreationPage::BUTTON_BACK:
		m_ProjectManagerPtr->m_MainPage.Show();
		Hide();
		_ClearInputs();
		break;

	case ProjectCreationPage::BUTTON_CREATE:
		_CreateProject();
		break;
	}
}

Long ProjectCreationPage::_Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
	switch (uMsg) {
	case WM_COMMAND:
		_OnButtonPressed(HIWORD(param_1), LOWORD(param_1),
			reinterpret_cast<HWND>(param_2));
		break;
	case WM_PAINT:
		m_ProjectNameText.Draw(m_hdc);
		m_ProjectPathText.Draw(m_hdc);
		break;
	}
	return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
}
