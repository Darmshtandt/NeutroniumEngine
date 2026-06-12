// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Nt/Core/MessageWindow.h>
#include <ProjectManager/ProjectManager.h>


ProjectMainPage::ProjectMainPage(NotNull<ProjectManager*> projectManagerPtr) :
	m_ProjectListItemPadding(5, 5),
	m_ProjectManagerPtr(projectManagerPtr)
{
}

ProjectMainPage::~ProjectMainPage() {
	for (Project* pProject : m_Projects)
		delete(pProject);
	m_Projects.clear();
}

void ProjectMainPage::Initialize(const Nt::Int2D& windowSize, const Settings& settings) {
	const Nt::IntRect rect({ }, windowSize);

	SetBackgroundColor(settings.Style["ProjectManager.BackgroundColor"]);
	RemoveStyles(WS_OVERLAPPEDWINDOW);
	SetProcedure([&] (const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		return _Procedure(uMsg, param_1, param_2);
		});
	Create(rect, "Main");
	Show();

	m_ProjectList.SetParent(*this);
	m_ProjectList.SetID(ID);
	m_ProjectList.AddStyles(WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_OWNERDRAWVARIABLE);
	m_ProjectList.Create(settings.ProjectManagerWindow.ProjectListRect);
	m_ProjectList.SetBackgroundColor(settings.Style["ProjectManager.ProjectList.BackgroundColor"]);

	m_ActionWindow.SetParent(*this);
	m_ActionWindow.Initialize(settings, ID);
	m_ActionWindow.Show();

	m_ProjectListTextHeader.SetColor(settings.Style["ProjectManager.Headers.Color"]);
	m_ProjectListTextHeader.SetWeight(settings.Style["ProjectManager.Headers.Weight"]);

	m_ProjectListTextDescription.SetColor(settings.Style["ProjectManager.Texts.Color"]);
	m_ProjectListTextDescription.SetWeight(settings.Style["ProjectManager.Texts.Weight"]);

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

void ProjectMainPage::SaveData() {
	std::ofstream file(".nteprojsdat", std::ios::binary);
	if (file.is_open()) {
		Nt::Serialization::WriteAll(file, m_Projects);
		file.close();
	}
	else {
		Raise("Failed to save file.");
	}
}

void ProjectMainPage::OpenProject(const uInt& index) {
	if (index == uInt(-1))
		return;

	try {
		m_ProjectManagerPtr->m_pProject = m_ProjectList.GetItemData<Project>(index);
		m_ProjectManagerPtr->m_pProject->Load();
		m_ProjectManagerPtr->m_MainWindow.Close();
	}
	catch (const Nt::Error& error) {
		m_ProjectManagerPtr->m_pProject = nullptr;
		error.Show();
	}
}
void ProjectMainPage::RemoveProject(const uInt& index) {
	if (index != uInt(-1))
		return;

	std::remove_all(std::path(m_Projects[index]->GetRootPath().c_str()));
	_RemoveProjectFromList(index);
}

void ProjectMainPage::_AddProjectToList(const Nt::String& path) {
	if (path.empty())
		return;

	Bool isExists = false;
	for (Project* pProject : m_Projects) {
		if (pProject->GetPath() == path) {
			isExists = true;
			break;
		}
	}

	if (!isExists) {
		Project* pProject = new Project;
		pProject->Load(path);
		m_Projects.push_back(pProject);
		_AddProjectToList(pProject);
	}
}

void ProjectMainPage::_AddProjectToList(NotNull<const Project*> pProject) {
	const uInt index = m_ProjectList.AddItem(pProject->GetName());

	m_ProjectList.SetItemData(index, pProject);
	m_ProjectList.SetItemHeight(index, 50);

	SaveData();
}

void ProjectMainPage::_RemoveProjectFromList(const uInt& index) {
	if (index != -1) {
		m_ProjectList.RemoveItem(index);
		delete(m_Projects[index]);
		m_Projects.erase(m_Projects.begin() + index);
		SaveData();
	}
}

void ProjectMainPage::_OnButtonEvent(const uInt& command, const uInt& id, [[maybe_unused]] const HWND& hButton) {
	if (command == BN_CLICKED) {
		const Int selectedIndex = m_ProjectList.GetSelectionIndex();

		switch (id - ID) {
		case ActionWindow::BUTTON_OPEN:
			OpenProject(selectedIndex);
			break;

		case ActionWindow::BUTTON_DELETE:
		{
			constexpr std::wstring_view message = L"Are you sure you want to delete the project?";
			constexpr std::wstring_view caption = L"Warning";

			if (Nt::MessageWindow(message, caption).ShowConfirmed(Nt::MessageIcon::WARNING))
				RemoveProject(selectedIndex);

			m_ActionWindow.DisableButtons();
			break;
		}

		case ActionWindow::BUTTON_CREATE:
			Hide();
			m_ProjectManagerPtr->m_CreationPage.Show();
			break;

		case ActionWindow::BUTTON_REMOVE_FROM_LIST:
			_RemoveProjectFromList(selectedIndex);
			m_ActionWindow.DisableButtons();
			break;

		case ActionWindow::BUTTON_ADD_TO_LIST:
		{
			std::wstring defaultPath = m_ProjectManagerPtr->m_CreationPage.m_DefaultProjectPath;

			if (defaultPath.back() == '\\')
				defaultPath.erase(defaultPath.end() - 1);

			constexpr std::wstring_view filters = L"Project file (*.nteproj)\0*.nteproj";
			_AddProjectToList(Nt::OpenFileDialog(defaultPath.c_str(), filters.data()));
		}
		break;
		}
	}
}

void ProjectMainPage::_OnListItemEvent(const uInt& command, [[maybe_unused]] const uInt& id, const HWND& hListBox) {
	switch (command) {
	case LBN_DBLCLK:
		OpenProject(m_ProjectList.GetSelectionIndex());
		break;
	case LBN_SELCHANGE:
		if (ListBox_GetCurSel(hListBox) == -1)
			m_ActionWindow.DisableButtons();
		else
			m_ActionWindow.EnableButtons();
		break;
	}
}

void ProjectMainPage::_OnDrawItem([[maybe_unused]] const uInt& uMsg, [[maybe_unused]] const DWord& param_1, const DWord& param_2) {
	DRAWITEMSTRUCT* pDrawItem = reinterpret_cast<DRAWITEMSTRUCT*>(param_2);
	if (pDrawItem != nullptr && Int(pDrawItem->itemID) != -1 && pDrawItem->CtlType == ODT_LISTBOX) {
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
			Nt::Log::Instance().Warning("Project pointer is null");
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

Long ProjectMainPage::_Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
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
		OpenProject(0);

		break;
	}
	return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
}
