#pragma once

#include <Nt/ModernMenu.h>

#include <Language.h>
#include <Objects/ObjectFactory.h>

struct Style;
class Engine;

class ProgramMenu : public ModernMenu {
public:
	explicit ProgramMenu(const Language& language);

	void AttachEditor(NotNull<WorldEditor*> pEditor) noexcept;
	void AttachStyle(NotNull<Style*> pStyle) noexcept;

	void SetLanguage(const Language& language);

private:
	ModernMenu* m_pMenuEntity = nullptr;
	ModernMenu* m_pMenuPrimitive = nullptr;
	ModernMenu* m_pMenuTheme = nullptr;
	ModernMenu* m_pMenuLanguage = nullptr;
	Language m_Language;

private:
	void _Localize(NotNull<ModernMenu*> pMenu);

	void _AddAllTo(ModernMenu& menu, const Flag& flags, const std::vector<std::string>& tokens);
	ModernMenu& _CreateSubMenuIn(ModernMenu& menu, const Flag& flags, const std::string& token);
};