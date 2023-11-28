#pragma once

#include <Nt/Graphics/WindowElements.h>

class ObjectsTree : private Nt::Window {
public:
	ObjectsTree() = default;

	void Initialize(const Settings& settings) {
		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_DLGFRAME);
		Create(settings.ObjectsTreeWindowRect, settings.CurrentLanguage.Window.ObjectsTreeWindow);

		m_TreeView.AddStyles(
			TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_EDITLABELS |
			TVS_TRACKSELECT | TVS_HASBUTTONS);
		m_TreeView.AddTreeExStyles(Nt::TreeView::EX_STYLE_MULTISELECT);
		m_TreeView.SetParent(*this);

		Nt::uIntRect treeViewRect;
		treeViewRect.RightBottom = settings.ObjectsTreeWindowRect.RightBottom;
		m_TreeView.Create(treeViewRect, "");
		m_TreeView.SetBackgroundColor(settings.Styles.ObjectsTree.BackgroundColor);
		m_TreeView.SetTextColor(settings.Styles.ObjectsTree.TreeView.Text.Color);
		//m_TreeView.SetTextWeight(settings.Styles.ObjectsTree.TreeView.Text.Weight);
		m_TreeView.Show();

		SetProcedure(std::bind(&ObjectsTree::_Procedure, this, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void Add(Object* pObject) {
		Nt::TreeView::Item item = { };
		item.Mask = Nt::TreeView::Item::Masks(
			Nt::TreeView::Item::MASK_TEXT | Nt::TreeView::Item::MASK_DATA);
		item.Text = pObject->GetName();
		item.Data = reinterpret_cast<Long>(pObject);
		m_TreeView.Add(item);
	}
	void RemoveSelection() {
		for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems)
			m_TreeView.Remove(itemID);

		m_TreeView.Select(nullptr);
		m_SelectedTreeItems.clear();
	}
	void Clear() {
		m_TreeView.Clear();
	}

	void Select(const uInt& id) {
		Nt::TreeView::ItemID itemID = m_TreeView.GetRootItem();
		uInt i = 0;
		for (; i < id; ++i)
			itemID = m_TreeView.GetNextItem(itemID);

		if (i == id)
			m_TreeView.Select(itemID);
	}

	Bool IsRenamingEnabled() const noexcept {
		return m_IsRenamingEnabled;
	}

	void SetSelector(Selector* selectorPtr) noexcept {
		m_SelectorPtr = selectorPtr;
	}
	void SetTheme(const Style& style) {
		if (m_TreeView.GetHandle()) {
			m_TreeView.SetBackgroundColor(style.ObjectsTree.BackgroundColor);
			m_TreeView.SetTextColor(style.ObjectsTree.TreeView.Text.Color);
		}
	}
	void SetLanguage(const Language& language) {
		SetName(language.Window.ObjectsTreeWindow);
	}
	void SetRect(const Nt::IntRect& rect) noexcept {
		Window::SetWindowRect(rect);
		m_TreeView.SetSize(rect.RightBottom);
	}

public:
	using Window::Show;
	using Window::SetParent;
	using Window::SetWindowRect;
	using Window::PopEvent;

private:
	static WNDPROC m_BaseWndProc;
	std::vector<Nt::TreeView::ItemID> m_SelectedTreeItems;
	Selector* m_SelectorPtr = nullptr;
	Nt::TreeView m_TreeView;
	Bool m_IsRenamingEnabled = false;

private:
	Long _Procedure(const uInt& uMsg, const uInt& wParam, const Long& lParam) {
		const HWND hTreeView = m_TreeView.GetHandle();

		switch (uMsg) {
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
			case TVN_SELCHANGED:
			{
				if (m_SelectorPtr == nullptr)
					break;
				m_SelectorPtr->AllDeselect();
				
				Nt::TreeView::ItemID hSelectedItem = m_TreeView.GetSelection();
				if (hSelectedItem == nullptr)
					break;

				if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
					for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems)
						m_TreeView.Deselect(itemID);
					m_SelectedTreeItems.clear();
				}

				Nt::TreeView::Item item = { };
				item.Mask = Nt::TreeView::Item::Masks(Nt::TreeView::Item::MASK_DATA | Nt::TreeView::Item::MASK_STATE);
				item.StateMask = Nt::TreeView::Item::STATE_SELECTED;

				auto iterator = std::find(
					m_SelectedTreeItems.begin(), 
					m_SelectedTreeItems.end(), hSelectedItem);
				if (iterator != m_SelectedTreeItems.end()) {
					m_SelectedTreeItems.erase(iterator);
					m_TreeView.Deselect(*iterator);
				}
				else {
					m_SelectedTreeItems.push_back(hSelectedItem);
				}

				for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems) {
					item.ID = itemID;
					m_TreeView.GetItem(&item);

					if (item.Data != 0)
						m_SelectorPtr->AddSelect(reinterpret_cast<Object*>(item.Data));

					item.State = Nt::TreeView::Item::STATE_SELECTED;
					m_TreeView.SetItem(&item);
				}
			}
				break;
			case NM_KILLFOCUS:
				SetActiveWindow(hTreeView);
				return 0;
			case TVN_BEGINLABELEDIT:
				m_IsRenamingEnabled = true;
				break;
			case TVN_ENDLABELEDIT:
			{
				m_IsRenamingEnabled = false;
				LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)lParam;
				if (dispInfo && dispInfo->item.pszText && dispInfo->item.pszText[0] != '\0') {
					((Object*)dispInfo->item.lParam)->SetName(dispInfo->item.pszText);
					return TRUE;
				}
			}
				break;
			case TVN_DELETEITEM:
			{
				Nt::TreeView::Item item = { };
				item.Mask = Nt::TreeView::Item::Masks(Nt::TreeView::Item::MASK_DATA | Nt::TreeView::Item::MASK_STATE);
				item.StateMask = Nt::TreeView::Item::STATE_SELECTED;

				for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems) {
					item.ID = itemID;
					m_TreeView.GetItem(&item);

					if (item.Data != 0) {
						Object* pObject = reinterpret_cast<Object*>(item.Data);
						m_SelectorPtr->Deselect(pObject);
						item.Data = 0;
					}

					item.State = Nt::TreeView::Item::STATE_SELECTED;
					m_TreeView.SetItem(&item);
				}
			}
			break;
			}
		case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				SetActiveWindow(hTreeView);
			break;
		}
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
};

inline WNDPROC ObjectsTree::m_BaseWndProc = nullptr;