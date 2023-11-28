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
		m_TreeView.AddTreeExStyles(TVS_EX_MULTISELECT);
		m_TreeView.SetParent(*this);
		m_TreeView.Create({ { }, settings.ObjectsTreeWindowRect.RightBottom }, "");
		m_TreeView.SetBackgroundColor(settings.Styles.ObjectsTree.BackgroundColor);
		m_TreeView.SetTextColor(settings.Styles.ObjectsTree.TreeView.Text.Color);
		//m_TreeView.SetTextWeight(settings.Styles.ObjectsTree.TreeView.Text.Weight);
		m_TreeView.Show();

		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (Long)this);
		m_BaseWndProc = (WNDPROC)GetWindowLongPtr(m_hwnd, GWLP_WNDPROC);
		SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (Long)_Proc);
	}

	void Add(Object* pObject) {
		m_TreeView.Add(
			pObject->GetName(), pObject, 0, 
			TVIF_TEXT | TVIF_PARAM | TVIF_STATE, TVIS_EXPANDED);
	}
	void Select(const uInt& id) {
		HWND hTreeView = m_TreeView.GetHandle();
		HTREEITEM hItem = TreeView_GetRoot(hTreeView);

		uInt i = 0;
		while (i < id) {
			hItem = TreeView_GetNextSibling(hTreeView, hItem);
			if (hItem == nullptr)
				return;
			++i;
		}

		if (i == id) {
			TVITEM item = { };
			item.mask = TVIF_STATE;
			item.stateMask = TVIS_SELECTED;
			item.hItem = hItem;
			TreeView_GetItem(hTreeView, &item);

			if (item.state & TVIS_SELECTED)
				item.state &= ~TVIS_SELECTED;
			else
				item.state |= TVIS_SELECTED;
			TreeView_SetItem(hTreeView, &item);
		}
	}
	void RemoveSelectedItems() {
		m_TreeView.RemoveSelectedItems();
	}

	void Clear() {
		m_TreeView.Clear();
	}

	Bool IsRenamingEnabled() const noexcept {
		return m_TreeView.IsRenamingEnabled();
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
		Window::SetRect(rect);
		m_TreeView.SetSize(rect.RightBottom);
	}

public:
	using Window::Show;
	using Window::SetParent;
	using Window::SetRect;
	using Window::PopEvent;

private:
	static WNDPROC m_BaseWndProc;
	Selector* m_SelectorPtr = nullptr;
	Nt::TreeView m_TreeView;

private:
	static LRESULT CALLBACK _Proc(HWND hwnd, uInt uMsg, WPARAM wParam, LPARAM lParam) {
		ObjectsTree* pThis = (ObjectsTree*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		const HWND hTreeView = pThis->m_TreeView.GetHandle();

		switch (uMsg) {
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
			case TVN_SELCHANGED:
				if (pThis->m_SelectorPtr != nullptr) {
					pThis->m_SelectorPtr->AllDeselect();

					TVITEM tvItem = { };
					tvItem.mask = TVIF_PARAM;

					for (const HTREEITEM& hTreeItem : pThis->m_TreeView.GetSelectedItems()) {
						TreeView_GetItem(hTreeView, &tvItem);
						if (tvItem.lParam > 0)
							pThis->m_SelectorPtr->AddSelect(reinterpret_cast<Object*>(tvItem.lParam));
					}
				}
				break;
			case TVN_DELETEITEM:
			{
				TVITEM tvItem = { };
				tvItem.mask = TVIF_PARAM;

				for (const HTREEITEM& hTreeItem : pThis->m_TreeView.GetSelectedItems()) {
					tvItem.hItem = hTreeItem;
					TreeView_GetItem(hTreeView, &tvItem);
					if (tvItem.lParam > 0) {
						Object* pObject = reinterpret_cast<Object*>(tvItem.lParam);
						pThis->m_SelectorPtr->Deselect(pObject);
						tvItem.lParam = 0;
					}
				}
			}
				break;
			}
		case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				SetActiveWindow(hTreeView);
			break;
		}
		return CallWindowProc(m_BaseWndProc, hwnd, uMsg, wParam, lParam);
	}
};

inline WNDPROC ObjectsTree::m_BaseWndProc = nullptr;