// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Editor/WorldEditor.h>
#include <ProgramMenu.h>

#include <Style.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Quad.h>
#include <Objects/Primitives/Pyramid.h>
#include <Objects/Primitives/Plane.h>

ProgramMenu::ProgramMenu(const Language& language) :
	ModernMenu("Menu"),
	m_Language(language)
{
	const Flag popupFlag = Flag(STRING | POPUP);

	ModernMenu& menuFile = _CreateSubMenuIn(*this, popupFlag, "Menu.File");
	_AddAllTo(menuFile, STRING, {
		"Menu.File.Build",
		"Menu.File.New",
		"Menu.File.Open",
		"Menu.File.Save",
		"Menu.File.SaveAs",
		"Menu.File.Close"
		});

	ModernMenu& menuView = _CreateSubMenuIn(*this, popupFlag, "Menu.View");
	_AddAllTo(menuView, Flag(STRING | CHECKED), {
		"Menu.View.ObjectTree",
		"Menu.View.FileExplorer",
		"Menu.View.Property"
		});

	ModernMenu& menuCreate = _CreateSubMenuIn(*this, popupFlag, "Menu.Create");
	m_pMenuPrimitive = &_CreateSubMenuIn(menuCreate, popupFlag, "Menu.Create.Primitive");
	m_pMenuEntity = &_CreateSubMenuIn(menuCreate, popupFlag, "Menu.Create.Entity");

	ModernMenu& menuSettings = _CreateSubMenuIn(*this, popupFlag, "Menu.Settings");
	m_pMenuTheme = &_CreateSubMenuIn(menuSettings, popupFlag, "Menu.Settings.Theme");
	m_pMenuLanguage = &_CreateSubMenuIn(menuSettings, popupFlag, "Menu.Settings.Language");
}

void ProgramMenu::AttachEditor(NotNull<WorldEditor*> pEditor) noexcept {
	const std::vector<std::string>& entityNames = EntityFactory::Instance().GetRegisteredNames();
	for (const std::string& name : entityNames) {
		const std::string token = m_pMenuEntity->GetToken() + '.' + name;

		m_pMenuEntity->Add(Nt::Menu::STRING, token, name);
		RegisterAction(token, [=] () { pEditor->CreateEntity(name); });
	}

	const std::vector<std::string>& primitiveNames = PrimitiveFactory::Instance().GetRegisteredNames();
	for (const std::string& name : primitiveNames) {
		const std::string token = m_pMenuPrimitive->GetToken() + '.' + name;

		m_pMenuPrimitive->Add(Nt::Menu::STRING, token, name);
		RegisterAction(token, [=] () { pEditor->CreatePrimitive(name); });
	}
}

void ProgramMenu::AttachStyle(NotNull<Style*> pStyle) noexcept {
	for (const std::string& name : pStyle->Themes) {
		const std::string token = m_pMenuTheme->GetToken() + '.' + name;

		m_pMenuTheme->Add(Flag(STRING | CHECKED), token, name);
		RegisterAction(token, [=] () { pStyle->LoadFromFile(name + ".json"); });
	}
}

void ProgramMenu::SetLanguage(const Language& language) {
	m_Language = language;
	_Localize(this);
}

void ProgramMenu::_Localize(NotNull<ModernMenu*> pMenu) {
	for (const auto& [token, id] : GetMenuMap())
		pMenu->ChangeItemText(id, m_Language[token]);

	for (SharedModernMenu& menu : GetAllSubMenu()) {
		pMenu->ChangeItemText(*menu, m_Language[menu->GetToken()]);
		_Localize(menu.get());
	}
}

void ProgramMenu::_AddAllTo(ModernMenu& menu, const Flag& flags, const std::vector<std::string>& tokens) {
	for (const std::string& token : tokens)
		menu.Add(flags, token, m_Language[token]);
}

ModernMenu& ProgramMenu::_CreateSubMenuIn(ModernMenu& menu, const Flag& flags, const std::string& token) {
	ModernMenu newSubMenu(token);
	menu.AddSubMenu(newSubMenu, flags, m_Language[token]);
	return menu.GetLastSubMenu();
}
