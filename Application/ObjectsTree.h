#pragma once

#include <Main.h>
#include <Scene.h>
#include <Nt/Graphics/System/WindowElements.h>
#include <Settings.h>
#include <Core/Commands.h>
#include <Nt/Core/EventBus.h>

struct RenamedObjectEvent final {
	std::string NewName;
	std::string OldName;
	Object* pObject;
};

class ObjectsTree : public Nt::Window {
	struct TreeData final {
		WeakObjectPtr pObject;
	};

public:
	explicit ObjectsTree(const std::weak_ptr<Nt::EventBus>& pEventBus) {
		Assert(!pEventBus.expired(), "EventBus pointer is null");
		SetEventBus(pEventBus);

		auto sharedBus = m_EventBus.lock();
		sharedBus->Subscribe<Scene::EventAddObject>([this] (const Scene::EventAddObject& e) {
			Add(e.pObject);
			});
		sharedBus->Subscribe<Scene::EventRemoveObject>([this] (const Scene::EventRemoveObject& e) {
			Remove(e.pObject.get());
			});
		sharedBus->Subscribe<Scene::EventClear>([this] (const Scene::EventClear& e) {
			(void)e;
			Clear();
			});
	}

	void Initialize(const Settings& settings, const Nt::String& defaultInitialPath) {
		RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		AddStyles(STYLE_BORDER);
		Create(settings.ObjectsTreeWindowRect, settings.Language["Window.ObjectTree"]);

		m_TreeView.AddStyles(
			TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_EDITLABELS |
			TVS_TRACKSELECT | TVS_HASBUTTONS);
		m_TreeView.AddTreeExStyles(Nt::TreeView::EX_STYLE_MULTISELECT);
		m_TreeView.SetParent(*this);
		m_TreeView.SetEventBus(m_EventBus);
		
		m_ObjectIcon.LoadFromFile(defaultInitialPath + "Images\\Primitives.bmp");

		static Nt::ImageList imageList;
		imageList.Create(Nt::Int2D(16, 16), Nt::ImageList::CREATEFLAG_COLOR, 1, 0);
		imageList.Add(&m_ObjectIcon);
		
		ImageList_SetBkColor(imageList.GetHandle(), CLR_NONE);

		Nt::uIntRect treeViewRect;
		treeViewRect.RightBottom = settings.ObjectsTreeWindowRect.RightBottom;
		m_TreeView.Create(treeViewRect, "");
		m_TreeView.SetBackgroundColor(settings.Style["ObjectsTree.BackgroundColor"]);
		m_TreeView.SetTextColor(settings.Style["ObjectsTree.TreeView.Text.Color"]);
		//m_TreeView.SetTextWeight(settings.Style["ObjectsTree.TreeView.Text.Weight"]);
		m_TreeView.SetImageList(imageList, false);
		m_TreeView.Show();
		
		SetProcedure(std::bind(&ObjectsTree::_Procedure, this, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	[[nodiscard]] Bool IsRenamingEnabled() const noexcept {
		return m_IsRenamingEnabled;
	}

	void SetTheme(const Style& style) {
		SetBorderColor(style["ObjectsTree.BorderColor"]);

		if (m_TreeView.GetHandle()) {
			m_TreeView.SetBackgroundColor(style["ObjectsTree.BackgroundColor"]);
			m_TreeView.SetTextColor(style["ObjectsTree.TreeView.Text.Color"]);
		}
	}
	void SetLanguage(const Language& language) {
		SetName(language["Window.ObjectTree"]);
	}
	void SetRect(const Nt::IntRect& rect) noexcept {
		Window::SetWindowRect(rect);
		m_TreeView.SetSize(rect.RightBottom);
	}

public:
	using Window::Show;
	using Window::SetParent;
	using Window::SetWindowRect;
	using Window::PeekMessages;

private:
	static WNDPROC m_BaseWndProc;
	std::vector<Nt::TreeView::ItemID> m_SelectedTreeItems;
	std::unordered_map<Object*, Nt::TreeView::ItemID> m_IDMap;
	std::unordered_map<Nt::TreeView::ItemID, Object*> m_ObjectMap;
	Nt::GDI::Bitmap m_ObjectIcon;
	Nt::TreeView m_TreeView;
	Bool m_IsRenamingEnabled = false;

private:
	void Add(const WeakObjectPtr& weakObject) {
		assert(!weakObject.expired());

		const auto object = weakObject.lock();
		if (!object)
			return;

		if (m_IDMap.contains(object.get()))
			Raise("Object \"" + object->GetName() + "\" already exist");

		const Nt::TreeView::ItemID id =
			m_TreeView.Add(_ObjectToItem(weakObject));

		m_IDMap[object.get()] = id;
		m_ObjectMap[id] = object.get();
	}
	void Remove(Object* pObject) {
		if (!m_IDMap.contains(pObject))
			Raise("Object \"" + pObject->GetName() + "\" not exist");

		const Nt::TreeView::ItemID id = m_IDMap[pObject];
		m_TreeView.Remove(id);
	}

	[[deprecated]] void Select(const uInt& id) {
		Nt::TreeView::ItemID itemID = m_TreeView.GetRootItem();
		uInt i = 0;
		for (; i < id; ++i)
			itemID = m_TreeView.GetNextItem(itemID);

		if (i == id)
			m_TreeView.Select(itemID);
	}
	[[deprecated]] void RemoveSelection() {
		for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems)
			m_TreeView.Remove(itemID);

		m_TreeView.Select(nullptr);
		m_SelectedTreeItems.clear();
	}

	void Clear() {
		if (m_ObjectMap.empty())
			return;

		m_ObjectMap.clear();
		m_IDMap.clear();
		m_TreeView.Clear();
	}

	static Nt::TreeView::Item _ObjectToItem(const WeakObjectPtr& weakObject) noexcept {
		assert(!weakObject.expired());

		const auto object = weakObject.lock();
		if (!object)
			return { };


		using Item = Nt::TreeView::Item;

		Item item = { };
		item.ImageID = 1;
		item.Text = object->GetName();
		item.Data = reinterpret_cast<Long>(new TreeData(weakObject));
		item.Mask = Item::Masks(Item::MASK_IMAGE | Item::MASK_TEXT | Item::MASK_DATA);
		return item;
	}

	void SelectionChanged() {
		auto sharedBus = m_EventBus.lock();
		sharedBus->Emmit<DeselectAllObjectsCommand>({ });

		Nt::TreeView::ItemID hSelectedItem = m_TreeView.GetSelection();
		if (hSelectedItem == nullptr)
			return;
		if (m_SelectedTreeItems.size() == 1 && m_SelectedTreeItems.front() == hSelectedItem)
			return;

		if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
			for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems)
				m_TreeView.Deselect(itemID);
			m_SelectedTreeItems.clear();
		}

		using Item = Nt::TreeView::Item;

		Item item = { };
		item.Mask = static_cast<Item::Masks>(Item::MASK_DATA | Item::MASK_STATE);
		item.StateMask = Item::STATE_SELECTED;

		auto iterator = std::find(
			m_SelectedTreeItems.begin(), m_SelectedTreeItems.end(), hSelectedItem);
		if (iterator != m_SelectedTreeItems.end()) {
			m_TreeView.Deselect(*iterator);
			m_SelectedTreeItems.erase(iterator);
		}
		else {
			m_SelectedTreeItems.push_back(hSelectedItem);
		}

		for (Nt::TreeView::ItemID& itemID : m_SelectedTreeItems) {
			item.ID = itemID;
			m_TreeView.GetItem(&item);

			if (item.Data != 0) {
				const auto data = reinterpret_cast<TreeData*>(item.Data);
				if (const auto object = data->pObject.lock())
					sharedBus->Emmit<MultiSelectObjectCommand>({ object });
			}

			item.State = Item::STATE_SELECTED;
			m_TreeView.SetItem(&item);
		}
	}

	Long _Procedure(const uInt& uMsg, const uInt& wParam, const Long& lParam) {
		Assert(!m_EventBus.expired(), "EventBus pointer is expired");

		const HWND hTreeView = m_TreeView.GetHandle();

		switch (uMsg) {
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
			case TVN_SELCHANGED:
				SelectionChanged();
				break;

			case NM_KILLFOCUS:
				SetActiveWindow(hTreeView);
				return 0;

			case TVN_BEGINLABELEDIT:
				m_IsRenamingEnabled = true;
				break;

			case TVN_ENDLABELEDIT: {
				m_IsRenamingEnabled = false;

				LPNMTVDISPINFO dispInfo = reinterpret_cast<LPNMTVDISPINFOW>(lParam);
				if (dispInfo && dispInfo->item.pszText && dispInfo->item.pszText[0] != '\0') {
					const auto data = reinterpret_cast<TreeData*>(dispInfo->item.lParam);
					assert(data);

					const auto object = data->pObject.lock();
					assert(object);

					RenamedObjectEvent e = { };
					e.pObject = object.get();
					e.OldName = e.pObject->GetName();
					e.NewName = Nt::String(dispInfo->item.pszText);
					e.pObject->SetName(e.NewName);

					m_EventBus.lock()->Emmit<RenamedObjectEvent>(e);
					return TRUE;
				}
			}
				break;

			case TVN_DELETEITEM: {
				LPNMTREEVIEW pInfo = reinterpret_cast<LPNMTREEVIEW>(lParam);
				assert(pInfo);
				assert(pInfo->itemOld.hItem);

				const auto data = reinterpret_cast<TreeData*>(pInfo->itemOld.lParam);
				assert(data);
				if (data)
					delete(data);

				const Nt::TreeView::ItemID id = pInfo->itemOld.hItem;
				if (m_ObjectMap.contains(id)) {
					Object* pObject = m_ObjectMap[id];
					m_IDMap.erase(pObject);
					m_ObjectMap.erase(id);
				}

				auto iterator = std::find(
					m_SelectedTreeItems.begin(), m_SelectedTreeItems.end(), id);
				if (iterator != m_SelectedTreeItems.end())
					m_SelectedTreeItems.erase(iterator);
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