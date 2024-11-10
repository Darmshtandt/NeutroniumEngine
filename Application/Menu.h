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

		m_Menu.AddSubMenu(m_Menu_File, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_FILE]);
		m_Menu.AddSubMenu(m_Menu_View, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_VIEW]);
		m_Menu.AddSubMenu(m_Menu_Create, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_CREATE]);
		m_Menu.AddSubMenu(m_Menu_Settings, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS]);
	}

	void SetLanguage(Language& language) {
		m_Menu.ChangeItemText(m_Menu_File, language.Menu.Texts[Language::_Menu::TEXT_FILE]);
		m_Menu.ChangeItemText(m_Menu_View, language.Menu.Texts[Language::_Menu::TEXT_VIEW]);

		m_Menu.ChangeItemText(m_Menu_Create, language.Menu.Texts[Language::_Menu::TEXT_CREATE]);
		m_Menu.ChangeItemText(m_Menu_Create_Primitive, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE]);
		m_Menu.ChangeItemText(m_Menu_Create_Entity, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY]);

		m_Menu.ChangeItemText(m_Menu_Settings, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS]);
		m_Menu.ChangeItemText(m_Menu_Settings_Theme, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_THEME]);
		m_Menu.ChangeItemText(m_Menu_Settings_Language, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_LANGUAGE]);

		m_Menu.ChangeItemText(MENU_BUILD, language.Menu.Texts[Language::_Menu::TEXT_FILE_BUILD]);
		m_Menu.ChangeItemText(MENU_NEW, language.Menu.Texts[Language::_Menu::TEXT_FILE_NEW]);
		m_Menu.ChangeItemText(MENU_OPEN, language.Menu.Texts[Language::_Menu::TEXT_FILE_OPEN]);
		m_Menu.ChangeItemText(MENU_SAVE, language.Menu.Texts[Language::_Menu::TEXT_FILE_SAVE]);
		m_Menu.ChangeItemText(MENU_SAVEAS, language.Menu.Texts[Language::_Menu::TEXT_FILE_SAVEAS]);
		m_Menu.ChangeItemText(MENU_CLOSE, language.Menu.Texts[Language::_Menu::TEXT_FILE_CLOSE]);
		
		m_Menu.ChangeItemText(MENU_ENABLE_OBJECTS_TREE, language.Menu.Texts[Language::_Menu::TEXT_VIEW_OBJECTSTREE]);
		m_Menu.ChangeItemText(MENU_ENABLE_FILE_EXPLORER, language.Menu.Texts[Language::_Menu::TEXT_VIEW_FILEEXPLORER]);
		m_Menu.ChangeItemText(MENU_ENABLE_PROPERTY, language.Menu.Texts[Language::_Menu::TEXT_VIEW_PROPERTY]);

		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_CUBE, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_CUBE]);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_PYRAMID, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_PYRAMID]);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_QUAD, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_QUAD]);
		m_Menu.ChangeItemText(MENU_CREATE_PRIMITIV_PLANE, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_PLANE]);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_CAMERA, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_CAMERA]);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_SOUND, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_SOUND]);
		m_Menu.ChangeItemText(MENU_CREATE_ENTITY_MODEL, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_MODEL]);
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
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_BUILD, language.Menu.Texts[Language::_Menu::TEXT_FILE_BUILD]);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_NEW, language.Menu.Texts[Language::_Menu::TEXT_FILE_NEW]);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_OPEN, language.Menu.Texts[Language::_Menu::TEXT_FILE_OPEN]);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_SAVE, language.Menu.Texts[Language::_Menu::TEXT_FILE_SAVE]);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_SAVEAS, language.Menu.Texts[Language::_Menu::TEXT_FILE_SAVEAS]);
		m_Menu_File.Add(Nt::Menu::FLAG_STRING, MENU_CLOSE, language.Menu.Texts[Language::_Menu::TEXT_FILE_CLOSE]);
	}
	void _InitializeItem_View(const Language& language) {
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_OBJECTS_TREE, language.Menu.Texts[Language::_Menu::TEXT_VIEW_OBJECTSTREE]);
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_FILE_EXPLORER, language.Menu.Texts[Language::_Menu::TEXT_VIEW_FILEEXPLORER]);
		m_Menu_View.Add(m_FlagStringCheckedDraw, MENU_ENABLE_PROPERTY, language.Menu.Texts[Language::_Menu::TEXT_VIEW_PROPERTY]);
	}
	void _InitializeItem_Create(const Language& language) {
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_CUBE, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_CUBE]);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_QUAD, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_QUAD]);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_PLANE, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_PLANE]);
		m_Menu_Create_Primitive.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_PRIMITIV_PYRAMID, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE_PYRAMID]);

		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_CAMERA, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_CAMERA]);
		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_SOUND, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_SOUND]);
		m_Menu_Create_Entity.Add(Nt::Menu::FLAG_STRING, MENU_CREATE_ENTITY_MODEL, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY_MODEL]);

		m_Menu_Create.AddSubMenu(m_Menu_Create_Primitive, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_CREATE_PRIMITIVE]);
		m_Menu_Create.AddSubMenu(m_Menu_Create_Entity, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_CREATE_ENTITY]);
	}
	void _InitializeItem_Settings(const Language& language) {
		m_Menu_Settings_Theme.Add(m_FlagStringCheckedDraw, MENU_THEME_SOLARIZED_DARK, L"Solarized Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_SOLARIZED_GREEN_DARK, L"Solarized Green-Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_SOLARIZED_RED_DARK, L"Solarized Red-Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_DARK, L"Dark");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_BLACK, L"Black");
		m_Menu_Settings_Theme.Add(m_FlagStringUncheckedDraw, MENU_THEME_WHITE, L"White");

		m_Menu_Settings_Language.Add(m_FlagStringCheckedDraw, MENU_LANGUAGE_ENGLISH, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_LANGUAGE_ENGLISH]);
		m_Menu_Settings_Language.Add(m_FlagStringUncheckedDraw, MENU_LANGUAGE_RUSSIAN, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_LANGUAGE_RUSSIAN]);
		m_Menu_Settings_Language.Add(m_FlagStringUncheckedDraw, MENU_LANGUAGE_SLOVAK, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_LANGUAGE_SLOVAK]);

		m_Menu_Settings.AddSubMenu(m_Menu_Settings_Theme, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_THEME]);
		m_Menu_Settings.AddSubMenu(m_Menu_Settings_Language, m_FlagStringPopupDraw, language.Menu.Texts[Language::_Menu::TEXT_SETTINGS_LANGUAGE]);
	}
};