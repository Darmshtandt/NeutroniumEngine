#pragma once


Int CALLBACK _ListCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	if (lParam1 == 0 || lParam2 == 0)
		Raise("Left or right item Data is nullptr");

	const std::string pathLeft = reinterpret_cast<std::path*>(lParam1)->string();
	const std::string pathRight = reinterpret_cast<std::path*>(lParam2)->string();

	const Bool leftIsDirectory = std::filesystem::is_directory(pathLeft);
	const Bool rightIsDirectory = std::filesystem::is_directory(pathRight);

	if (leftIsDirectory != rightIsDirectory)
		return rightIsDirectory;
	return pathLeft.compare(pathRight);
}
LRESULT CALLBACK SelectionProc(HWND hwnd, uInt uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CTLCOLORSTATIC:
		HDC hdcStatic = (HDC)wParam;
		SetBkColor(hdcStatic, RGB(230, 230, 230));
		return reinterpret_cast<LRESULT>(CreateSolidBrush(RGB(0, 0, 128)));
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

class FileExplorer : public Nt::Window {
public:
	FileExplorer() = default;
	~FileExplorer() {
		_ClearDirectoryTree();
		_ClearDirectoryView();
	}

	void Initialize(const Settings& settings) {
		m_Language = settings.CurrentLanguage;
		m_Style = settings.Styles;

		RemoveStyles(WS_OVERLAPPEDWINDOW);
		AddStyles(WS_DLGFRAME);
		SetBackgroundColor(settings.Styles.FileExplorer.BackgroundColor);
		SetProcedure([&](const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
			return _Procedure(uMsg, param_1, param_2);
			});
		Create(settings.FileExplorerWindowRect, settings.CurrentLanguage.Window.FileExplorerWindow);

		m_PathFieldRect.Left = m_ClientRect.Right / 5;
		m_PathFieldRect.Right = m_ClientRect.Right - m_PathFieldRect.Left;
		m_PathFieldRect.Bottom = 24;

		m_SearchFieldRect.Right = m_ClientRect.Right - m_PathFieldRect.Right;
		m_SearchFieldRect.Bottom = m_PathFieldRect.Bottom;

		m_DirectoryTreeRect.Top = m_PathFieldRect.Bottom;
		m_DirectoryTreeRect.Right = m_SearchFieldRect.Right;
		m_DirectoryTreeRect.Bottom = m_ClientRect.Bottom;

		m_DirectoryViewRect.Left = m_PathFieldRect.Left;
		m_DirectoryViewRect.Top = m_DirectoryTreeRect.Top;
		m_DirectoryViewRect.Right = m_PathFieldRect.Right;
		m_DirectoryViewRect.Bottom = m_ClientRect.Bottom - m_PathFieldRect.Bottom;


		m_DirectoryTree.SetParent(*this);
		m_DirectoryTree.AddStyles(
			WS_BORDER | WS_VSCROLL | TVS_LINESATROOT | TVS_HASBUTTONS |
			TVS_TRACKSELECT | TVS_SINGLEEXPAND | TVS_SHOWSELALWAYS);
		m_DirectoryTree.SetBackgroundColor(m_Style.FileExplorer.BackgroundColor);
		m_DirectoryTree.SetTextColor(m_Style.FileExplorer.Texts.Color);
		m_DirectoryTree.Create(m_DirectoryTreeRect);
		m_DirectoryTree.Show();

		m_PathField.SetParent(*this);
		m_PathField.SetBackgroundColor(m_Style.FileExplorer.TextEdits.BackgroundColor);
		m_PathField.SetTextColor(m_Style.FileExplorer.Texts.Color);
		m_PathField.SetTextWeight(m_Style.FileExplorer.Texts.Weight);
		m_PathField.AddStyles(ES_READONLY);
		m_PathField.Create(m_PathFieldRect, "", true);
		m_PathField.Show();

		m_SearchField.SetParent(*this);
		m_SearchField.SetBackgroundColor(m_Style.FileExplorer.TextEdits.BackgroundColor);
		m_SearchField.SetTextColor(m_Style.FileExplorer.Texts.Color);
		m_SearchField.SetTextWeight(m_Style.FileExplorer.Texts.Weight);
		m_SearchField.SetPlaceholder("Search...");
		m_SearchField.Create(m_SearchFieldRect, "Search...", true);
		m_SearchField.Show();

		m_DirectoryView.SetParent(*this);
		m_DirectoryView.SetBackgroundColor(m_Style.FileExplorer.BackgroundColor);
		m_DirectoryView.SetTextColor(m_Style.FileExplorer.Texts.Color);
		m_DirectoryView.AddStyles(WS_VSCROLL | WS_BORDER | LVS_ICON | LVS_EDITLABELS);

		const Nt::ListView::ExtendedStyles exStyles = Nt::ListView::ExtendedStyles(Nt::ListView::EX_STYLE_TWOCLICKACTIVATE | Nt::ListView::EX_STYLE_ONECLICKACTIVATE);
		m_DirectoryView.AddExtendedStyles(exStyles, exStyles);
		m_DirectoryView.DisableTextBackground();
		m_DirectoryView.Create(m_DirectoryViewRect);
		m_DirectoryView.Show();
		
		
		HIMAGELIST hImageList_SmallIcons;
		if (FAILED(SHGetImageList(SHIL_SMALL, IID_IImageList, reinterpret_cast<void**>(&hImageList_SmallIcons))))
			Raise("Failed to load shell icons");

		HIMAGELIST hImageList_LargeIcons;
		if (FAILED(SHGetImageList(SHIL_LARGE, IID_IImageList, reinterpret_cast<void**>(&hImageList_LargeIcons))))
			Raise("Failed to load shell icons");

		ImageList_SetBkColor(hImageList_SmallIcons, CLR_NONE);
		m_DirectoryTree.SetImageList(hImageList_SmallIcons, TVSIL_NORMAL);
		
		ImageList_SetBkColor(hImageList_LargeIcons, CLR_NONE);
		ListView_SetImageList(m_DirectoryView.GetHandle(), hImageList_LargeIcons, LVSIL_NORMAL);

		//m_hSelection = CreateWindowEx(
		//	WS_EX_LAYERED, WC_STATIC, L"", 
		//	WS_VISIBLE | WS_CHILD,
		//	0, 0, 50, 50,
		//	m_DirectoryView.GetHandle(),
		//	nullptr, nullptr, nullptr
		//);

		//SetLayeredWindowAttributes(m_hSelection, 0, 128, LWA_ALPHA);
		//SetWindowLongPtr(m_hSelection, GWLP_WNDPROC, reinterpret_cast<Long>(SelectionProc));
		//InvalidateRect(m_hSelection, nullptr, TRUE);
	}

	void SetTheme(const Style& style) {
		m_Style = style;
		SetBackgroundColor(m_Style.FileExplorer.BackgroundColor);

		m_DirectoryTree.SetBackgroundColor(m_Style.FileExplorer.BackgroundColor);
		m_DirectoryTree.SetTextColor(m_Style.FileExplorer.Texts.Color);

		m_DirectoryView.SetBackgroundColor(m_Style.FileExplorer.BackgroundColor);
		m_DirectoryView.SetTextColor(m_Style.FileExplorer.Texts.Color);

		m_PathField.SetBackgroundColor(m_Style.FileExplorer.TextEdits.BackgroundColor);
		m_PathField.SetTextColor(m_Style.FileExplorer.TextEdits.Text.Color);
		m_PathField.SetTextWeight(m_Style.FileExplorer.TextEdits.Text.Weight);

		m_SearchField.SetBackgroundColor(m_Style.FileExplorer.TextEdits.BackgroundColor);
		m_SearchField.SetTextColor(m_Style.FileExplorer.Texts.Color);
		m_SearchField.SetTextWeight(m_Style.FileExplorer.Texts.Weight);
	}
	void SetLanguage(const Language& language) {
		m_Language = language;
		SetName(m_Language.Window.FileExplorerWindow);
	}
	void SetRootPath(const Nt::String& rootPath) {
		m_RootPath = rootPath;
		m_PathField.SetText(rootPath);
		_Update();
	}

private:
	using Window::Create;
	using Window::Destroy;

private:
	std::path m_CurrentOpenedDirectory;
	std::path m_RootPath;
	Nt::TreeView m_DirectoryTree;
	Nt::ListView m_DirectoryView;
	Nt::TextEdit m_PathField;
	Nt::TextEdit m_SearchField;
	std::vector<Int> m_SelectedItems;
	std::vector<std::path*> m_DirectionTreeData;
	std::vector<std::path*> m_DirectionViewData;
	HWND m_hSelection;
	Language m_Language;
	Style m_Style;

	Nt::IntRect m_DirectoryTreeRect;
	Nt::IntRect m_DirectoryViewRect;
	Nt::IntRect m_PathFieldRect;
	Nt::IntRect m_SearchFieldRect;

private:
	void _Update() {
		_ClearDirectoryTree();
		m_DirectoryTree.Clear();

		std::path* pPath = new std::path(m_RootPath);
		m_DirectionTreeData.push_back(pPath);

		Nt::TreeView::Item item = { };
		item.Mask = Nt::TreeView::Item::Masks(
			Nt::TreeView::Item::MASK_TEXT | Nt::TreeView::Item::MASK_DATA |
			Nt::TreeView::Item::MASK_IMAGE | Nt::TreeView::Item::MASK_STATE);
		item.Text = Nt::StringTowString(pPath->filename().string());
		item.Data = reinterpret_cast<Long>(pPath);
		item.ImageID = _GetIconID(m_RootPath.string() + '\\');
		item.State = Nt::TreeView::Item::STATE_EXPANDED;

		const Nt::TreeView::ItemID rootID = m_DirectoryTree.Add(item);
		_AddDirectoryToTree(m_RootPath, rootID);
		_OpenDirectory(m_RootPath);
	}
	void _OpenDirectory(const std::path& path) {
		_ClearDirectoryView();
		m_DirectoryView.ClearItems();
		m_CurrentOpenedDirectory = path;

		std::directory_iterator directory(m_CurrentOpenedDirectory);
		for (auto dir : directory) {
			std::path* pPath = new std::path(dir.path());
			m_DirectionViewData.push_back(pPath);;

			Nt::ListView::Item item;
			item.Text = Nt::StringTowString(pPath->filename().string());
			item.Data = reinterpret_cast<Long>(pPath);
			item.Mask = Nt::ListView::Item::Masks(Nt::ListView::Item::MASK_TEXT | Nt::ListView::Item::MASK_DATA | Nt::ListView::Item::MASK_IMAGE);
			item.ImageIndex = _GetIconID(pPath->string());
			m_DirectoryView.AddItem(item);
		}
		m_DirectoryView.SortItems(_ListCompareFunc, 0);
	}
	void _AddDirectoryToTree(const std::path& path, const Nt::TreeView::ItemID& parentID) {
		std::directory_iterator directory(path);
		for (auto dir : directory) {
			if (dir.is_directory()) {
				std::path* pPath = new std::path(dir.path());
				m_DirectionTreeData.push_back(pPath);

				Nt::TreeView::Item item = { };
				item.Mask = Nt::TreeView::Item::Masks(
					Nt::TreeView::Item::MASK_TEXT | Nt::TreeView::Item::MASK_DATA |
					Nt::TreeView::Item::MASK_IMAGE | Nt::TreeView::Item::MASK_STATE);
				item.Text = Nt::StringTowString(pPath->filename().string());
				item.Data = reinterpret_cast<Long>(pPath);
				item.ImageID = _GetIconID(path.string());
				item.State = Nt::TreeView::Item::STATE_EXPANDED;

				const Nt::TreeView::ItemID itemID = m_DirectoryTree.Add(item, parentID);
				if (!std::filesystem::is_empty(dir))
					_AddDirectoryToTree(dir, itemID);
			}
		}

	}
	Int _GetIconID(const Nt::String& path) {
		SHFILEINFO sfi = { };
		const std::wstring wPath = path;
		
		int a = GetFileAttributes(wPath.c_str());
		SHGetFileInfo(wPath.c_str(), GetFileAttributes(wPath.c_str()), &sfi,
			sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
		return sfi.iIcon;
	}
	void _ClearDirectoryTree() {
		for (std::path* pPath : m_DirectionTreeData)
			delete(pPath);
		m_DirectionTreeData.clear();
	}
	void _ClearDirectoryView() {
		for (std::path* pPath : m_DirectionViewData)
			delete(pPath);
		m_DirectionViewData.clear();
	}

	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2) {
		switch (uMsg) {
		case WM_NOTIFY:
		{
			const NMHDR* pHdr = reinterpret_cast<NMHDR*>(param_2);
			switch (pHdr->code) {
			case LVN_ITEMACTIVATE:
			{
				NMLISTVIEW* pnmListView = reinterpret_cast<NMLISTVIEW*>(param_2);

				LVITEM lvItem = { };
				lvItem.mask = LVIF_TEXT | LVIF_PARAM;
				lvItem.iItem = pnmListView->iItem;
				ListView_GetItem(m_DirectoryView.GetHandle(), &lvItem);
			}
				break;
			case LVN_BEGINDRAG:
			{
				NMLISTVIEW* pnmListView = reinterpret_cast<NMLISTVIEW*>(param_2);
				Int index = ListView_GetNextItem(pnmListView->hdr.hwndFrom, -1, LVNI_SELECTED);
				if (index == -1)
					break;

				m_SelectedItems.clear();
				m_SelectedItems.push_back(index);
				while ((index = ListView_GetNextItem(pnmListView->hdr.hwndFrom, index, LVNI_SELECTED)) != -1)
					m_SelectedItems.push_back(index);

				if (m_SelectedItems.size() == 1) {
					const uInt mask = LVIS_SELECTED | LVIS_DROPHILITED;
					m_DirectoryView.SetItemState(-1, 0, mask);
					m_DirectoryView.SetItemState(pnmListView->iItem, mask, mask);
				}
			}
				break;
			case LVM_EDITLABEL:
				break;
			//case LVN_BEGINLABELEDIT:
			//	cout << "LVN_BEGINLABELEDIT" << endl;
			//	break;
			//case LVN_BEGINSCROLL:
			//	cout << "LVN_BEGINSCROLL" << endl;
			//	break;
			//case LVN_COLUMNCLICK:
			//	cout << "LVN_COLUMNCLICK" << endl;
			//	break;
			//case LVN_COLUMNDROPDOWN:
			//	cout << "LVN_COLUMNDROPDOWN" << endl;
			//	break;
			//case LVN_COLUMNOVERFLOWCLICK:
			//	cout << "LVN_COLUMNOVERFLOWCLICK" << endl;
			//	break;
			//case LVN_DELETEALLITEMS:
			//	cout << "LVN_DELETEALLITEMS" << endl;
			//	break;
			//case LVN_DELETEITEM:
			//	cout << "LVN_DELETEITEM" << endl;
			//	break;
			//case LVN_ENDLABELEDIT:
			//	cout << "LVN_ENDLABELEDIT" << endl;
			//	break;
			//case LVN_ENDSCROLL:
			//	cout << "LVN_ENDSCROLL" << endl;
			//	break;
			//case LVN_GETDISPINFO:
			//	cout << "LVN_GETDISPINFO" << endl;
			//	break;
			//case LVN_GETEMPTYMARKUP:
			//	cout << "LVN_GETEMPTYMARKUP" << endl;
			//	break;
			//case LVN_GETINFOTIP:
			//	cout << "LVN_GETINFOTIP" << endl;
			//	break;
			//case LVN_HOTTRACK:
			//	cout << "LVN_HOTTRACK" << endl;
			//	break;
			//case LVN_INCREMENTALSEARCH:
			//	cout << "LVN_INCREMENTALSEARCH" << endl;
			//	break;
			//case LVN_INSERTITEM:
			//	cout << "LVN_INSERTITEM" << endl;
			//	break;
			//case LVN_ITEMCHANGED:
			//	cout << "LVN_ITEMCHANGED" << endl;
			//	break;
			//case LVN_ITEMCHANGING:
			//	cout << "LVN_ITEMCHANGING" << endl;
			//	break;
			//case LVN_KEYDOWN:
			//	cout << "LVN_KEYDOWN" << endl;
			//	break;
			//case LVN_LINKCLICK:
			//	cout << "LVN_LINKCLICK" << endl;
			//	break;
			//case LVN_MARQUEEBEGIN:
			//	cout << "LVN_MARQUEEBEGIN" << endl;
			//	break;
			//case LVN_ODCACHEHINT:
			//	cout << "LVN_ODCACHEHINT" << endl;
			//	break;
			//case LVN_ODFINDITEM:
			//	cout << "LVN_ODFINDITEM" << endl;
			//	break;
			//case LVN_ODSTATECHANGED:
			//	cout << "LVN_ODSTATECHANGED" << endl;
			//	break;
			//case LVN_SETDISPINFO:
			//	cout << "LVN_SETDISPINFO" << endl;
			//	break;
			//case NM_CUSTOMDRAW:
			//	cout << "NM_CUSTOMDRAW" << endl;
			//	break;
			//case NM_DBLCLK:
			//	cout << "NM_DBLCLK" << endl;
			//	break;
			//case NM_HOVER:
			//	cout << "NM_HOVER" << endl;
			//	break;
			//case NM_KILLFOCUS:
			//	cout << "NM_KILLFOCUS" << endl;
			//	break;
			//case NM_RCLICK:
			//	cout << "NM_RCLICK" << endl;
			//	break;
			//case NM_RDBLCLK:
			//	cout << "NM_HOVER" << endl;
			//	break;
			//case NM_RELEASEDCAPTURE:
			//	cout << "NM_RELEASEDCAPTURE" << endl;
			//	break;
			//case NM_RETURN:
			//	cout << "NM_RETURN" << endl;
			//	break;
			//case NM_SETFOCUS:
			//	cout << "NM_SETFOCUS" << endl;
			//	break;
			//case NM_CLICK:
			//	cout << "NM_CLICK" << endl;
			//	break;
			case TVN_ITEMEXPANDING:
			case TVN_ITEMEXPANDED:
			{
				NMTREEVIEW* pTreeView = reinterpret_cast<NMTREEVIEW*>(param_2);
				if (pTreeView && pTreeView->action == TVE_COLLAPSE || pTreeView->action == TVE_EXPAND) {
					POINT cursorPosition;
					GetCursorPos(&cursorPosition);
					MapWindowPoints(HWND_DESKTOP, pTreeView->hdr.hwndFrom, &cursorPosition, 1);

					Nt::TreeView::HitTestInfo info = m_DirectoryTree.HitTest(cursorPosition);
					if (!(info.Flags & Nt::TreeView::HitTestInfo::HITTEST_ONITEMBUTTON))
						m_DirectoryTree.Expand(info.ItemID, Nt::TreeView::EXPAND_EXPAND);
				}
			}
			break;
			case TVN_SELCHANGED:
			{
				Nt::TreeView::Item item = { };
				item.Mask = Nt::TreeView::Item::MASK_DATA;
				item.ID = m_DirectoryTree.GetSelection();
				if (!m_DirectoryTree.GetItem(&item)) {
					Nt::String warningMessage = "Failed to get TreeViewS selected item. Error code: ";
					warningMessage += GetLastError();
					Nt::Log::Warning(warningMessage);
				}
				else if (item.Data != 0) {
					_OpenDirectory(*reinterpret_cast<std::path*>(item.Data));
				}
			}
			break;
			case TVN_ENDLABELEDIT:
				LPNMTVDISPINFO dispInfo = reinterpret_cast<LPNMTVDISPINFO>(param_2);
				if (dispInfo && dispInfo->item.pszText && dispInfo->item.pszText[0] != '\0') {
					((Object*)dispInfo->item.lParam)->SetName(dispInfo->item.pszText);
					return TRUE;
				}
				break;
			}
		}
		break;
		case WM_MOUSEMOVE:
			if (m_SelectedItems.size() == 1) {
				HWND hItem = GetDlgItem(m_DirectoryView.GetHandle(), m_SelectedItems[0]);
				ShowWindow(hItem, SW_HIDE);
			}
			else if (m_SelectedItems.size() > 1) {

			}
			break;
		case WM_LBUTTONUP:
		{
			const HWND hwndListView = m_DirectoryView.GetHandle();

			POINT cursorPosition;
			GetCursorPos(&cursorPosition);
			MapWindowPoints(HWND_DESKTOP, hwndListView, &cursorPosition, 1);

			LVHITTESTINFO info = { };
			info.pt = cursorPosition;
			ListView_HitTest(hwndListView, &info);

			LVITEM listItem = { };
			listItem.mask = LVIF_PARAM;
			listItem.iItem = info.iItem;
			ListView_GetItem(hwndListView, &listItem);

			if (listItem.lParam != 0) {
				const std::path* pPath = reinterpret_cast<std::path*>(listItem.lParam);
				if (pPath && std::is_directory(*pPath)) {
					for (Int i = m_SelectedItems.size() - 1; i >= 0; --i) {
						const uInt index = m_SelectedItems[i];

						listItem.iItem = index;
						ListView_GetItem(hwndListView, &listItem);
						const std::path* pFilePath = reinterpret_cast<std::path*>(listItem.lParam);

						try {
							std::filesystem::rename(*pFilePath, 
								pPath->string() + '\\' + pFilePath->filename().string());
							_Update();
						}
						catch (const std::filesystem::filesystem_error& error) {
							ErrorBoxA(error.what(), "Error");
							continue;
						}

						m_DirectoryView.RemoveItem(index);
						m_DirectionViewData.erase(m_DirectionViewData.begin() + index);
					}
				}
				m_DirectoryView.SortItems(_ListCompareFunc, 0);
			}
			m_SelectedItems.clear();
		}
		break;
		}
		return DefWindowProc(m_hwnd, uMsg, param_1, param_2);
	}
};