#pragma once

#include <Tools/PropertyComponent.h>

#include <unordered_map>

class PropertyFactory {
public:
	using CreateFunction = Function<PropertyComponent* (NotNull<Selector*> pSelector, NotNull<Scene*> pScene)>;

	[[nodiscard]]
	static PropertyFactory& Instance() noexcept {
		static PropertyFactory factory;
		return factory;
	}

	void Register(const std::string& className, const CreateFunction& func) {
		if (m_Compotents.contains(className))
			Raise("Component already exist");
		m_Compotents[className] = func;
	}

	[[nodiscard]]
	PropertyComponent* CreateComponent(const std::string& className, NotNull<Selector*> pSelector, NotNull<Scene*> pScene) {
		if (!m_Compotents.contains(className))
			Raise("The component does not exist");
		return m_Compotents[className](pSelector, pScene);
	}

	[[nodiscard]]
	std::vector<std::string> GetAllNames() const {
		std::vector<std::string> names;
		for (const auto& [name, _] : m_Compotents)
			names.push_back(name);
		return names;
	}

private:
	std::unordered_map<std::string, CreateFunction> m_Compotents;
};