#pragma once

#include <Function.h>
#include <Nt/Graphics/System/Menu.h>

#include <unordered_map>
#include <cassert>

class ModernMenu : public Nt::Menu {
public:
	using Action = Function<void()>;
	using SharedModernMenu = std::shared_ptr<ModernMenu>;

public:
	ModernMenu(const std::string& token) :
		Menu(),
		m_Token(token)
	{
		if (m_Token.empty())
			Raise("Empty token");
	}
	ModernMenu(const ModernMenu& menu) :
		Menu(menu),
		m_ActionMap(menu.m_ActionMap),
		m_MenuMap(menu.m_MenuMap),
		m_AllSubMenu(menu.m_AllSubMenu),
		m_Token(menu.m_Token)
	{
	}
	~ModernMenu() noexcept = default;
	//ModernMenu(ModernMenu&& menu) noexcept :
	//	Menu(static_cast<Menu&&>(menu)),
	//	m_ActionMap(std::move(menu.m_ActionMap)),
	//	m_MenuMap(std::move(menu.m_MenuMap)),
	//	m_AllSubMenu(std::move(menu.m_AllSubMenu)),
	//	m_Token(std::move(menu.m_Token)) 
	//{
	//}

	void AddAll(const Flag& flags, const std::vector<std::string>& tokens) {
		for (const std::string& token : tokens)
			Add(flags, token, token);
	}
	void Add(const Flag& flags, const std::string& token, const std::string& text) {
		if (token.empty())
			Raise("Empty token");
		if (m_MenuMap.contains(token))
			Raise("Token already exists");

		const Long id = _GetNextID();
		m_MenuMap[token] = id;

		Menu::Add(flags, id, text);
	}
	void AddSubMenu(const ModernMenu& menu, const Flag& flags, const std::string& text) {
		m_AllSubMenu.push_back(std::make_shared<ModernMenu>(menu));
		Menu::AddSubMenu(*m_AllSubMenu.back(), flags, text);
	}

	void RegisterAction(const std::string& token, const Action& action) {
		if (!action)
			Raise("Empty action");

		ModernMenu* pMenu = RequireNotNull(_FindMenu(token), "Not found menu, token: " + token);

		const Long id = pMenu->m_MenuMap[token];
		m_ActionMap[id] = action;
	}
	void ExecuteAction(const Long& id) const {
		const ModernMenu* pMenu = _FindAction(id);
		if (pMenu != nullptr)
			pMenu->m_ActionMap.at(id)();
	}

	_NODISCARD ModernMenu& GetFirstSubMenu() {
		if (m_AllSubMenu.empty())
			Raise("Out of range");
		return *m_AllSubMenu.front();
	}
	_NODISCARD ModernMenu& GetLastSubMenu() {
		if (m_AllSubMenu.empty())
			Raise("Out of range");
		return *m_AllSubMenu.back();
	}

	_NODISCARD ModernMenu& GetSubMenu(const std::string& token) {
		if (token.empty())
			Raise("Token is empty");

		for (SharedModernMenu& subMenu : m_AllSubMenu) {
			if (subMenu->m_Token == token)
				return *subMenu;
		}

		Raise("Not found sub menu by token");
	}

	_NODISCARD const std::string& GetToken() const noexcept {
		return m_Token;
	}

protected:
	_NODISCARD const std::unordered_map<Long, Action>& GetActionMap() const noexcept {
		return m_ActionMap;
	}
	_NODISCARD const std::unordered_map<std::string, Long>& GetMenuMap() const noexcept {
		return m_MenuMap;
	}

	_NODISCARD std::vector<SharedModernMenu>& GetAllSubMenu() noexcept {
		return m_AllSubMenu;
	}

private:
	std::unordered_map<std::string, Long> m_MenuMap;
	std::unordered_map<Long, Action> m_ActionMap;
	std::vector<SharedModernMenu> m_AllSubMenu;
	std::string m_Token;

private:
	_NODISCARD ModernMenu* _FindMenu(const std::string& token) noexcept {
		if (m_MenuMap.contains(token))
			return this;

		for (SharedModernMenu& menu : m_AllSubMenu) {
			ModernMenu* pResult = menu->_FindMenu(token);
			if (pResult != nullptr)
				return pResult;
		}

		return nullptr;
	}

	_NODISCARD const ModernMenu* _FindAction(const uInt& id) const noexcept {
		if (m_ActionMap.contains(id))
			return this;

		for (const SharedModernMenu& menu : m_AllSubMenu) {
			const ModernMenu* pResult = menu->_FindAction(id);
			if (pResult != nullptr)
				return pResult;
		}

		return nullptr;
	}

	_NODISCARD Long _GetNextID() const noexcept {
		static Long id = 0x10000000;
		return ++id;
	}
};