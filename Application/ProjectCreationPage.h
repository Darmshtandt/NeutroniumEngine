#pragma once

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
	ProjectCreationPage() :
		m_ProjectManagerPtr(nullptr)
	{ 
	}

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
		const Nt::IntRect rect(Nt::Zero2D<Int>, windowSize);
		const Style::ProjectManagerWindow projectManager = settings.Styles.ProjectManager;

		SetBackgroundColor(projectManager.CreationWindow.BackgroundColor);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			return _Procedure(uMsg, param_1, param_2);
			});
		Create(rect, "Create");

		Nt::Int2D textPosition;
		textPosition.x = 10;
		textPosition.y = rect.Bottom / 4;

		m_ProjectNameText.AddFormat(DT_SINGLELINE);
		m_ProjectNameText.SetColor(projectManager.Headers.Color);
		m_ProjectNameText.SetWeight(projectManager.Headers.Weight);
		m_ProjectNameText.SetPosition(textPosition);
		m_ProjectNameText.SetText(
			settings.CurrentLanguage.ProjectManagerWindow.Creation.Texts[Language::_ProjectManagerWindow::_Creation::TEXT_PROJECTNAME]);

		Nt::IntRect textEditRect(textPosition, Nt::Zero2D<Int>);
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
		m_ProjectPathText.SetColor(projectManager.Headers.Color);
		m_ProjectPathText.SetWeight(projectManager.Headers.Weight);
		m_ProjectPathText.SetPosition(textPosition);
		m_ProjectPathText.SetText(
			settings.CurrentLanguage.ProjectManagerWindow.Creation.Texts[Language::_ProjectManagerWindow::_Creation::TEXT_PROJECTPATH]);

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
				const std::wstring wPath = m_DefaultProjectPath.wstr();
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

		m_BrowseButton.SetBackgroundColor(settings.Styles.ProjectManager.Buttons.BackgroundColor);
		m_BrowseButton.SetParent(*this);
		m_BrowseButton.SetID(ID + BUTTON_BROWSE);
		m_BrowseButton.Create(buttonRect, "...");
		m_BrowseButton.Show();
		
		buttonRect.RightBottom = { 75, 30 };
		buttonRect.Left = textEditRect.Left;
		buttonRect.Top = rect.Bottom - 16 - 20;
		buttonRect.Top -= buttonRect.Left + buttonRect.Bottom;

		m_BackButton.SetBackgroundColor(settings.Styles.ProjectManager.Buttons.BackgroundColor);
		m_BackButton.SetParent(*this);
		m_BackButton.SetID(ID + BUTTON_BACK);
		m_BackButton.Create(buttonRect, "Back");
		m_BackButton.Show();

		buttonRect.Left = rect.Right - 16;
		buttonRect.Left -= textEditRect.Left + buttonRect.Right;

		m_CreateButton.SetBackgroundColor(settings.Styles.ProjectManager.Buttons.BackgroundColor);
		m_CreateButton.SetParent(*this);
		m_CreateButton.SetID(ID + BUTTON_CREATE);
		m_CreateButton.Create(buttonRect, "Create");
		m_CreateButton.Show();
	}

private:
	ProjectManager* m_ProjectManagerPtr;
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
	void _ClearInputs() {
		m_InputProjectName.SetText(m_DefaultProjectName);
		m_InputProjectPath.SetText(m_DefaultProjectPath);
	}
	void _CreateProject();
	void _OnButtonPressed(const uInt& command, const uInt& id, const HWND& hButton);

	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
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
};