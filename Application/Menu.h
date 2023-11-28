#pragma once

class ProgramMenu {
public:
	enum MenuItems {
		MENU_NULL = 0x10000000,

		MENU_BUILD,
		MENU_NEW,
		MENU_OPEN,
		MENU_SAVE,
		MENU_SAVEAS,
		MENU_CLOSE,

		MENU_ENABLE_OBJECTS_TREE,
		MENU_ENABLE_FILE_EXPLORER,
		MENU_ENABLE_PROPERTY,

		MENU_CREATE_PRIMITIV_CUBE,
		MENU_CREATE_PRIMITIV_QUAD,
		MENU_CREATE_PRIMITIV_PLANE,
		MENU_CREATE_PRIMITIV_PYRAMID,
		MENU_CREATE_ENTITY_CAMERA,
		MENU_CREATE_ENTITY_SOUND,
		MENU_CREATE_ENTITY_MODEL,

		MENU_THEME_SOLARIZED_DARK,
		MENU_THEME_SOLARIZED_GREEN_DARK,
		MENU_THEME_SOLARIZED_RED_DARK,
		MENU_THEME_DARK,
		MENU_THEME_BLACK,
		MENU_THEME_WHITE,

		MENU_LANGUAGE_ENGLISH,
		MENU_LANGUAGE_RUSSIAN,
		MENU_LANGUAGE_SLOVAK,
	};

public:
	ProgramMenu() = default;

	void Initialize(const Language& language) {
		_InitializeItem_File(language);
		_InitializeItem_View(language);
		_InitializeItem_Create(language);
		_InitializeItem_Settings(language);

		m_Menu.AddSubMenu(m_Menu_File, m_FlagStringPopupDraw, language.Menu.File);
		m_Menu.AddSubMenu(m_Menu_View, m_FlagStringPopupDraw, language.Menu.View);
		m_Menu.AddSubMenu(m_Menu_Create, m_FlagStringPopupDraw, language.Menu.Create);
		m_Menu.AddSubMenu(m_Menu_Settings, m_FlagStringPopupDraw, language.Menu.Settings);
	}

	void SetLanguage(Language& language) {
		m_Menu.ChangeItemText(m_Menu_File, language.Menu.File);
		m_Menu.ChangeItemText(m_Menu_View, language.Menu.View);

		m_Menu.ChangeItemText(m_Menu_Create, language.Menu.Create);
		m_Menu.ChangeItemText(m_Menu_Create_Primitive, language.Menu.Create_Primitive);
		m_Menu.ChangeItemText(m_Menu_Create_Entity, language.Menu.Create_Entity);

		m_Menu.ChangeItemText(m_Menu_Settings, language.Menu.Settings);
		m_Menu.ChangeItemText(m_Menu_Settings_Theme, language.Menu.Settings_Theme);
		m_Menu.ChangeItemText(m_Menu_Settings_Language, language.Menu.Settings_Language);

		m_Menu.ChangeItemText(MENU_BUILD, language.Menu.File_Build);
		m_Menu.ChangeItemText(MENU_NEW, language.Menu.File_New);
		m_Menu.ChangeItemText(MENU_OPEN, language.Menu.File_Open);
		m_Menu.ChangeItemText(MENU_SAVE, language.Menu.File_Save);
		m_Menu.ChangeItemText(MENU_SAVEAS, language.Menu.File_SaveAs);
		m_Menu.ChangeItemText(MENU_CLOSE, language.Menu.File_Close);
		
		m_Menu.ChangeItemText(MENU_ENABLE_OBJECTS_TREE, language.Menu.View_ObjectsTree);
		m_Menu.ChangeItemText(MENU_ENABLE_FILE_EXPLORER, language.Menu.View_FileExplorer);
		m_Menu.ChangeItemText(MENU_ENABLE_PROPERTY, language.Menu.View_Property);

		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_CUBE, language.Menu.Create_Primitive_Cube);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_PYRAMID, language.Menu.Create_Primitive_Pyramid);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_QUAD, language.Menu.Create_Primitive_Quad);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_PLANE, language.Menu.Create_Primitive_Plane);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_CAMERA, language.Menu.Create_Entity_Camera);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_SOUND, language.Menu.Create_Entity_Sound);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_MODEL, language.Menu.Create_Entity_Model);
	}

	Nt::Menu& GetNtMenu() noexcept {
		return m_Menu;
	}

private:
	Nt::Menu m_Menu;
	Nt::Menu m_Menu_File;
	Nt::Menu m_Menu_View;
	Nt::Menu m_Menu_Create;
	Nt::Menu m_Menu_Settings;
	Nt::Menu m_Menu_Create_Primitive;
	Nt::Menu m_Menu_Create_Entity;
	Nt::Menu m_Menu_Settings_Theme;
	Nt::Menu m_Menu_Settings_Language;

	const Nt::Menu::Flags m_FlagStringDraw = Nt::Menu::Flags(Nt::Menu::FLAG_STRING);
	const Nt::Menu::Flags m_FlagStringPopupDraw = Nt::Menu::Flags(m_FlagStringDraw | Nt::Menu::FLAG_POPUP);
	const Nt::Menu::Flags m_FlagStringCheckedDraw = Nt::Menu::Flags(Nt::Menu::FLAG_STRING | Nt::Menu::FLAG_CHECKED);
	const Nt::Menu::Flags m_FlagStringUncheckedDraw = Nt::Menu::Flags(Nt::Menu::FLAG_STRING | Nt::Menu::FLAG_UNCHECKED);

private:
	void _InitializeItem_File(const Language& language) {
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_BUILD, language.Menu.File_Build);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_NEW, language.Menu.File_New);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_OPEN, language.Menu.File_Open);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_SAVE, language.Menu.File_Save);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_SAVEAS, language.Menu.File_SaveAs);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_CLOSE, language.Menu.File_Close);
	}
	void _InitializeItem_View(const Language& language) {
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_OBJECTS_TREE, language.Menu.View_ObjectsTree);
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_FILE_EXPLORER, language.Menu.View_FileExplorer);
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_PROPERTY, language.Menu.View_Property);
	}
	void _InitializeItem_Create(const Language& language) {
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_CUBE, language.Menu.Create_Primitive_Cube);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_QUAD, language.Menu.Create_Primitive_Quad);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_PLANE, language.Menu.Create_Primitive_Plane);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_PYRAMID, language.Menu.Create_Primitive_Pyramid);

		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_CAMERA, language.Menu.Create_Entity_Camera);
		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_SOUND, language.Menu.Create_Entity_Sound);
		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_MODEL, language.Menu.Create_Entity_Model);

		m_Menu_Create.AddSubMenu(m_Menu_Create_Primitive, m_FlagStringPopupDraw, language.Menu.Create_Primitive);
		m_Menu_Create.AddSubMenu(m_Menu_Create_Entity, m_FlagStringPopupDraw, language.Menu.Create_Entity);
	}
	void _InitializeItem_Settings(const Language& language) {
		m_Menu_Settings_Theme.Add(m_FlagStringCheckedDraw, MENU_THEME_SOLARIZED_DARK, L"Solarized Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_SOLARIZED_GREEN_DARK, L"Solarized Green-Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_SOLARIZED_RED_DARK, L"Solarized Red-Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_DARK, L"Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_BLACK, L"Black");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_WHITE, L"White");

		m_Menu_Settings_Language.Add(m_FlagStringCheckedDraw, MENU_LANGUAGE_ENGLISH, language.Menu.Settings_Language_English);
		m_Menu_Settings_Language.Add(m_FlagStringUncheckedDraw, MENU_LANGUAGE_RUSSIAN, language.Menu.Settings_Language_Russian);
		m_Menu_Settings_Language.Add(m_FlagStringUncheckedDraw, MENU_LANGUAGE_SLOVAK, language.Menu.Settings_Language_Slovak);

		m_Menu_Settings.AddSubMenu(m_Menu_Settings_Theme, m_FlagStringPopupDraw, language.Menu.Settings_Theme);
		m_Menu_Settings.AddSubMenu(m_Menu_Settings_Language, m_FlagStringPopupDraw, language.Menu.Settings_Language);
	}
};