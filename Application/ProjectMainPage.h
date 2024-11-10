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

	using LanguageData = Language::_ProjectManagerWindow::_Action;

public:
	ActionWindow() = default;

	void Initialize(Settings settings, const uInt& id) {
		m_LanguageData = settings.CurrentLanguage.ProjectManagerWindow.Action;

		const Style::ProjectManagerWindow::Handle styles =
			settings.Styles.ProjectManager.ActionWindow;

		const auto buttonProc = [&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			if (uMsg == WM_COMMAND && HIWORD(param_1) == BN_CLICKED && m_hParent != nullptr)
				SendMessage(m_hParent, uMsg, param_1, param_2);
			return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
		};

		SetProcedure(buttonProc);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetBackgroundColor(styles.BackgroundColor);
		Create(settings.ProjectManagerWindow.ActionWindowRect, "");
		Show();

		const Language languageStruct = settings.CurrentLanguage;

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
			button.Create(buttonRect, m_LanguageData.Texts[i]);

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
	LanguageData m_LanguageData;
	std::vector<Nt::Button> m_Buttons;
};

class ProjectCreationPage;
class ProjectManager;

class ProjectMainPage : private Nt::Window {
	friend class ProjectCreationPage;
	friend class ProjectManager;

public:
	const Int ID = 2000;

public:
	ProjectMainPage() :
		m_ProjectListItemPadding(5, 5),
		m_ProjectManagerPtr(nullptr)
	{ 
	}
	~ProjectMainPage() {
		for (Project* pProject : m_Projects)
			delete(pProject);
		m_Projects.clear();
	}

	void Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
		const Nt::IntRect rect({ }, windowSize);
		const Style::ProjectManagerWindow projectManager = settings.Styles.ProjectManager;

		SetBackgroundColor(projectManager.BackgroundColor);
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			return _Procedure(uMsg, param_1, param_2);
			});
		Create(rect, "Main");
		Show();

		m_ProjectList.SetParent(*this);
		m_ProjectList.SetID(ID);
		m_ProjectList.AddStyles(WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_OWNERDRAWVARIABLE);
		m_ProjectList.Create(settings.ProjectManagerWindow.ProjectListRect);
		m_ProjectList.SetBackgroundColor(projectManager.ProjectList.BackgroundColor);

		m_ActionWindow.SetParent(*this);
		m_ActionWindow.Initialize(settings, ID);
		m_ActionWindow.Show();

		m_ProjectListTextHeader.SetColor(projectManager.Headers.Color);
		m_ProjectListTextHeader.SetWeight(projectManager.Headers.Weight);

		m_ProjectListTextDescription.SetColor(projectManager.Texts.Color);
		m_ProjectListTextDescription.SetWeight(projectManager.Texts.Weight);

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
	ProjectManager* m_ProjectManagerPtr;
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

	void _OnListItemEvent(const uInt& command, [[maybe_unused]] const uInt& id, const HWND& hListBox) {
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
	void _OnDrawItem([[maybe_unused]] const uInt& uMsg, [[maybe_unused]] const DWord& param_1, const DWord& param_2) {
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
			//_OpenProject(5);

			break;
		}
		return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
	}
};