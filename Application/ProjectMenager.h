#pragma once

class ActionWindow : public Nt::Window {
public:
	enum Buttons {
		BUTTON_OPEN,
		BUTTON_DELETE,
		BUTTON_CREATE,
		BUTTON_REMOVE_FROM_LIST,
		BUTTON_ADD_TO_LIST,
		BUTTON_COUNT,
	};

public:
	ActionWindow() = default;

	void Initialize(Settings settings, const uInt& id) {
		const Style::ProjectMenagerWindow::Handle styles =
			settings.Styles.ProjectMenager.ActionWindow;

		const auto buttonProc = [&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			if (uMsg == WM_COMMAND && HIWORD(param_1) == BN_CLICKED && m_hParent != nullptr)
				SendMessage(m_hParent, uMsg, param_1, param_2);
			return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
		};

		SetProcedure(buttonProc);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetBackgroundColor(styles.BackgroundColor);
		Create(settings.ProjectMenagerWindow.ActionWindowRect, "");
		Show();

		const Language languageStruct = settings.CurrentLanguage;
		const std::wstring buttonTexts[BUTTON_COUNT] = {
			languageStruct.ProjectMenagerWindow.ActionWindow.Open.wstr(),
			languageStruct.ProjectMenagerWindow.ActionWindow.Delete.wstr(),
			languageStruct.ProjectMenagerWindow.ActionWindow.Create.wstr(),
			languageStruct.ProjectMenagerWindow.ActionWindow.RemoveFromList.wstr(),
			languageStruct.ProjectMenagerWindow.ActionWindow.AddToList.wstr(),
		};

		constexpr Nt::IntRect paddingRect = { 10, 10, 10, 10 };
		constexpr uInt buttonGap = 5;
		Nt::IntRect buttonRect;
		buttonRect.LeftTop = paddingRect.LeftTop;
		buttonRect.Right = m_ClientRect.Right - buttonRect.Left - paddingRect.Right;
		buttonRect.Bottom = 25;
		for (uInt i = 0; i < BUTTON_COUNT; ++i) {
			Nt::Button button;
			button.SetParent(*this);
			button.SetID(id + i);
			button.AddStyles(BS_CENTER | BS_VCENTER | WS_VISIBLE);
			button.Create(buttonRect, buttonTexts[i]);

			if (i < BUTTON_CREATE || i == BUTTON_REMOVE_FROM_LIST)
				button.DisableWindow();
			m_Buttons.push_back(button);

			buttonRect.Top += buttonRect.Bottom + buttonGap;
		}
	}

	void EnableButtons() {
		for (uInt i = 0; i < BUTTON_CREATE; ++i)
			m_Buttons[i].EnableWindow();
		m_Buttons[BUTTON_REMOVE_FROM_LIST].EnableWindow();
	}
	void DisableButtons() {
		for (uInt i = 0; i < BUTTON_CREATE; ++i)
			m_Buttons[i].DisableWindow();
		m_Buttons[BUTTON_REMOVE_FROM_LIST].DisableWindow();
	}

private:
	using Window::Create;
	using Window::Destroy;

private:
	std::vector<Nt::Button> m_Buttons;
};

class ProjectCreationPage;
class ProjectMenager;

class ProjectMainPage : private Nt::Window {
	friend class ProjectCreationPage;
	friend class ProjectMenager;

public:
	const Int ID = 2000;

public:
	ProjectMainPage() :
		m_ProjectListItemPadding(5, 5),
		m_ProjectMenagerPtr(nullptr)
	{ 
	}
	~ProjectMainPage() {
		for (Project* pProject : m_Projects)
			delete(pProject);
		m_Projects.clear();
	}

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
		const Nt::IntRect rect({ }, windowSize);
		const Style::ProjectMenagerWindow projectMenager = settings.Styles.ProjectMenager;

		SetBackgroundColor(projectMenager.BackgroundColor);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			return _Procedure(uMsg, param_1, param_2);
			});
		Create(rect, "Main");
		Show();

		m_ProjectList.SetParent(*this);
		m_ProjectList.SetID(ID);
		m_ProjectList.AddStyles(WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_OWNERDRAWVARIABLE);
		m_ProjectList.Create(settings.ProjectMenagerWindow.ProjectListRect);
		m_ProjectList.SetBackgroundColor(projectMenager.ProjectList.BackgroundColor);

		m_ActionWindow.SetParent(*this);
		m_ActionWindow.Initialize(settings, ID);
		m_ActionWindow.Show();

		m_ProjectListTextHeader.SetColor(projectMenager.Headers.Color);
		m_ProjectListTextHeader.SetWeight(projectMenager.Headers.Weight);

		m_ProjectListTextDescription.SetColor(projectMenager.Texts.Color);
		m_ProjectListTextDescription.SetWeight(projectMenager.Texts.Weight);

		std::ifstream file(".nteprojsdat", std::ios::binary);
		if (file.is_open()) {
			Nt::Serialization::ReadAll(file, m_Projects);
			for (uInt i = 0; i < m_Projects.size(); ++i) {
				m_ProjectList.AddItem(m_Projects[i]->GetName());
				m_ProjectList.SetItemData(i, m_Projects[i]);
				m_ProjectList.SetItemHeight(i, 50);
			}
			file.close();
		}
	}

	void SaveData() {
		std::ofstream file(".nteprojsdat", std::ios::binary);
		if (file.is_open()) {
			Nt::Serialization::WriteAll(file, m_Projects);
			file.close();
		}
		else {
			Raise("Failed to save file.");
		}
	}

private:
	ProjectMenager* m_ProjectMenagerPtr;
	ActionWindow m_ActionWindow;
	Nt::ListBox m_ProjectList;
	Nt::Text m_ProjectListTextHeader;
	Nt::Text m_ProjectListTextDescription;
	Nt::Int2D m_ProjectListItemPadding;
	std::vector<Project*> m_Projects;

private:
	void _AddProjectToList(const Project* pProject) {
		if (pProject == nullptr)
			Raise("Project is nullptr");

		const uInt index = m_ProjectList.AddItem(pProject->GetName());
		m_ProjectList.SetItemData(index, pProject);
		m_ProjectList.SetItemHeight(index, 50);
		SaveData();
	}
	void _OpenProject(const uInt& index);
	void _RemoveFromProjectList(const uInt& index) {
		if (index != -1) {
			m_ProjectList.RemoveItem(index);
			delete(m_Projects[index]);
			m_Projects.erase(m_Projects.begin() + index);
			SaveData();
		}
	}

	void _OnButtonEvent(const uInt& command, const uInt& id, const HWND& hButton);

	void _OnListItemEvent(const uInt& command, const uInt& id, const HWND& hListBox) {
		switch (command) {
		case LBN_DBLCLK:
			_OpenProject(m_ProjectList.GetSelectionIndex());
			break;
		case LBN_SELCHANGE:
			if (ListBox_GetCurSel(hListBox) == -1)
				m_ActionWindow.DisableButtons();
			else
				m_ActionWindow.EnableButtons();
			break;
		}
	}
	void _OnDrawItem(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		DRAWITEMSTRUCT* pDrawItem = reinterpret_cast<DRAWITEMSTRUCT*>(param_2);
		if (pDrawItem && Int(pDrawItem->itemID) != -1 && pDrawItem->CtlType == ODT_LISTBOX) {
			HDC hdc = pDrawItem->hDC;
			HBRUSH hBrush;
			if (pDrawItem->itemState & ODS_SELECTED)
				hBrush = CreateSolidBrush(m_ProjectList.GetBackgroundColor() * 3);
			else
				hBrush = CreateSolidBrush(m_ProjectList.GetBackgroundColor());
			FillRect(hdc, &pDrawItem->rcItem, hBrush);
			DeleteObject(hBrush);

			const Project* pProject = reinterpret_cast<Project*>(pDrawItem->itemData);
			if (pProject == nullptr) {
				Nt::Log::Warning("Data in ProjectList is nullptr");
				return;
			}

			const RECT textRect = pDrawItem->rcItem;
			Nt::Int2D textPosition = 
				m_ProjectListItemPadding + Nt::Int2D(textRect.left, textRect.top);
			m_ProjectListTextHeader.SetPosition(textPosition);
			m_ProjectListTextHeader.SetText(pProject->GetName());
			m_ProjectListTextHeader.Draw(hdc);

			textPosition.y += 20;
			m_ProjectListTextDescription.SetPosition(textPosition);
			m_ProjectListTextDescription.SetText(pProject->GetRootPath());
			m_ProjectListTextDescription.Draw(hdc);
		}
	}

	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		switch (uMsg) {
		case WM_COMMAND:
			_OnListItemEvent(HIWORD(param_1), LOWORD(param_1),
				reinterpret_cast<HWND>(param_2));
			_OnButtonEvent(HIWORD(param_1), LOWORD(param_1),
				reinterpret_cast<HWND>(param_2));
			break;
		case WM_KEYUP:
			if (param_1 == VK_ESCAPE)
				ListBox_SetCurSel(m_ProjectList.GetHandle(), -1);
			break;
		case WM_CTLCOLORLISTBOX:
			if (reinterpret_cast<HWND>(param_2) == m_ProjectList.GetHandle())
				return reinterpret_cast<LRESULT>(
					CreateSolidBrush(m_ProjectList.GetBackgroundColor()));
			break;
		case WM_DRAWITEM:
			_OnDrawItem(uMsg, param_1, param_2);
			break;
		}
		return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
	}
};

class ProjectCreationPage : private Nt::Window {
	friend class ProjectMainPage;
	friend class ProjectMenager;

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
		m_ProjectMenagerPtr(nullptr)
	{ 
	}

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
		const Nt::IntRect rect(Nt::Int2D::Zero, windowSize);
		const Style::ProjectMenagerWindow projectMenager = settings.Styles.ProjectMenager;

		SetBackgroundColor(projectMenager.CreationWindow.BackgroundColor);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			return _Procedure(uMsg, param_1, param_2);
			});
		Create(rect, "Create");

		Nt::Int2D textPosition;
		textPosition.x = 10;
		textPosition.y = rect.Bottom / 4;

		m_ProjectNameText.AddFormat(DT_SINGLELINE);
		m_ProjectNameText.SetColor(projectMenager.Headers.Color);
		m_ProjectNameText.SetWeight(projectMenager.Headers.Weight);
		m_ProjectNameText.SetPosition(textPosition);
		m_ProjectNameText.SetText(settings.CurrentLanguage.ProjectMenagerWindow.CreationWindow.ProjectName);

		Nt::IntRect textEditRect(textPosition, Nt::Int2D::Zero);
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
		m_ProjectPathText.SetColor(projectMenager.Headers.Color);
		m_ProjectPathText.SetWeight(projectMenager.Headers.Weight);
		m_ProjectPathText.SetPosition(textPosition);
		m_ProjectPathText.SetText(settings.CurrentLanguage.ProjectMenagerWindow.CreationWindow.ProjectPath);

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

		m_BrowseButton.SetBackgroundColor(settings.Styles.ProjectMenager.Buttons.BackgroundColor);
		m_BrowseButton.SetParent(*this);
		m_BrowseButton.SetID(ID + BUTTON_BROWSE);
		m_BrowseButton.Create(buttonRect, "...");
		m_BrowseButton.Show();
		
		buttonRect.RightBottom = { 75, 30 };
		buttonRect.Left = textEditRect.Left;
		buttonRect.Top = rect.Bottom - 16 - 20;
		buttonRect.Top -= buttonRect.Left + buttonRect.Bottom;

		m_BackButton.SetBackgroundColor(settings.Styles.ProjectMenager.Buttons.BackgroundColor);
		m_BackButton.SetParent(*this);
		m_BackButton.SetID(ID + BUTTON_BACK);
		m_BackButton.Create(buttonRect, "Back");
		m_BackButton.Show();

		buttonRect.Left = rect.Right - 16;
		buttonRect.Left -= textEditRect.Left + buttonRect.Right;

		m_CreateButton.SetBackgroundColor(settings.Styles.ProjectMenager.Buttons.BackgroundColor);
		m_CreateButton.SetParent(*this);
		m_CreateButton.SetID(ID + BUTTON_CREATE);
		m_CreateButton.Create(buttonRect, "Create");
		m_CreateButton.Show();
	}

private:
	ProjectMenager* m_ProjectMenagerPtr;
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

class ProjectMenager {
	friend class ProjectMainPage;
	friend class ProjectCreationPage;

public:
	ProjectMenager() = default;

	void Initialize(const Settings& settings) {
		m_Settings = settings;
		const Style::ProjectMenagerWindow projectMenager = m_Settings.Styles.ProjectMenager;

		const Nt::Int2D mainWindowSize = m_Settings.ProjectMenagerWindow.Size;
		m_MainWindow.RemoveStyles(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		m_MainWindow.Create(mainWindowSize, "");

		m_MainPage.SetParent(m_MainWindow);
		m_MainPage.Initialize(mainWindowSize, settings);
		m_MainPage.Show();
		
		m_CreationPage.SetParent(m_MainWindow);
		m_CreationPage.Initialize(mainWindowSize, m_Settings);

		m_MainPage.m_ProjectMenagerPtr = this;
		m_CreationPage.m_ProjectMenagerPtr = this;
	}
	void Run() {
		m_MainWindow.Show();

		Nt::Event event;
		while (m_MainWindow.IsOpened()) {
			m_MainWindow.PeekMessages();
			while (m_MainWindow.PopEvent(&event)) {
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