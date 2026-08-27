#pragma once

#include <Function.h>
#include <unordered_map>

#include <Objects/Entities/Entity.h>
#include <Objects/Primitives/Primitive.h>

template <class _Object>
class ObjectFactory {
public:
	using CreateFunction = Function<_Object* (const std::string&)>;

public:
	[[nodiscard]]
	static ObjectFactory<_Object>& Instance() noexcept {
		static ObjectFactory<_Object> factory;
		return factory;
	}

	[[nodiscard]]
	_Object* Create(const std::string& className, const std::string& name) {
		if (!m_ObjectMap.contains(className))
			Raise(std::string("Object: " + className + " not found"));
		return m_ObjectMap[className](name);
	}

	void Register(const std::string& className, const CreateFunction& function) {
		if (m_ObjectMap.contains(className))
			Raise("Object already registered");
		if (!function)
			Raise("CreateFunction is missing");

		m_ObjectMap[className] = function;
	}

	[[nodiscard]]
	std::vector<std::string> GetRegisteredNames() const {
		std::vector<std::string> allNames;
		for (const auto& [name, _] : m_ObjectMap)
			allNames.push_back(name);

		return allNames;
	}

private:
	std::unordered_map<std::string, CreateFunction> m_ObjectMap;
};

using EntityFactory = ObjectFactory<Entity>;
using PrimitiveFactory = ObjectFactory<Primitive>;