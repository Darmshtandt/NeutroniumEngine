#include <Main.h>
#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Primitive.h>
#include <Entity.h>
#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>
#include <EnumWrapper.h>

namespace luabridge {
	template<>
	struct Stack<Script::Data::Types> : EnumWrapper<Script::Data::Types>
	{
	};
}

void Script::Initialize(Scence* pScence) {
	if (m_IsInitialized)
		Raise("The script has already been initialized.");

	m_pLua->GetGlobalNamespace()
		.beginNamespace("DataType")
		.addVariable<Int>("STRING", Script::Data::STRING)
		.addVariable<Int>("NUMBER", Script::Data::NUMBER)
		.addVariable<Int>("BOOL", Script::Data::BOOL)
		.endNamespace();
	m_pLua->GetGlobalNamespace()
		.beginClass<Lua>("Lua")
		.addConstructor<void(*)()>()
		.endClass();
	m_pLua->GetGlobalNamespace()
		.beginClass<Script>("Script")
		.addConstructor<void(*)(Lua*)>()
		.addFunction("AddData", &Script::AddData)
		.addFunction("GetDataValue", &Script::GetDataValue)
		.endClass();
	m_pLua->SetGlobal("g_ThisScript", this);
	
	m_IsInitialized = true;
}

Bool Script::_SetThisObject() {
	if (!m_pLua->SetGlobal("g_ThisScript", this))
		return false;

	switch (m_pObject->ObjectType) {
	case ObjectTypes::PRIMITIVE:
		return m_pLua->SetGlobal(
			"g_ThisObject",
			UpcastObjectToPrimitive(m_pObject));
		break;
	case ObjectTypes::ENTITY:
	{
		Entity* pEntity = UpcastObjectToEntity(m_pObject);
		switch (pEntity->GetEntityType()) {
		case EntityTypes::CAMERA:
			return m_pLua->SetGlobal(
				"g_ThisObject",
				UpcastEntityToGameCamera(pEntity));
			break;
		case EntityTypes::MODEL:
			return m_pLua->SetGlobal(
				"g_ThisObject",
				UpcastEntityToGameModel(pEntity));
			break;
		default:
			Raise("Invalid entity type");
		}
	}
	break;
	default:
		Raise("Invalid object type");
	}

	if (lua_pcall(m_pLua->GetState(), 0, LUA_MULTRET, 0) != LUA_OK)
		Raise(_GetLastError(), "Script error");
	return false;
}

void Script::Load(const Nt::String& filePath, Object* pObject) {
	if (pObject == nullptr)
		Raise("Object pointer is null.");

	if (m_IsLoaded)
		m_IsLoaded = false;

	m_FilePath = filePath;
	m_pObject = pObject;

	switch (m_pObject->ObjectType) {
	case ObjectTypes::PRIMITIVE:
		m_pLua->SetGlobal(
			"g_ThisObject",
			UpcastObjectToPrimitive(m_pObject));
		break;
	case ObjectTypes::ENTITY:
	{
		Entity* pEntity = UpcastObjectToEntity(m_pObject);
		switch (pEntity->GetEntityType()) {
		case EntityTypes::CAMERA:
			m_pLua->SetGlobal(
				"g_ThisObject",
				UpcastEntityToGameCamera(pEntity));
			break;
		case EntityTypes::MODEL:
			m_pLua->SetGlobal(
				"g_ThisObject",
				UpcastEntityToGameModel(pEntity));
			break;
		default:
			Raise("Invalid entity type");
		}
	}
		break;
	default:
		Raise("Invalid object type");
	}

	try {
		if (luaL_loadfile(m_pLua->GetState(), m_FilePath.c_str()) != LUA_OK)
			Raise(_GetLastError(), "Script error");

		std::string newPath = std::current_path().string() + '\\' + m_FilePath;
		while (*(newPath.end() - 1) != '\\' && *(newPath.end() - 1) != '/')
			newPath.erase(newPath.end() - 1);

		m_pLua->SetPath(newPath);

		if (lua_pcall(m_pLua->GetState(), 0, LUA_MULTRET, 0) != LUA_OK)
			Raise(_GetLastError(), "Script error");

		m_LuaStart = m_pLua->GetGlobal("Start");
		if (m_LuaStart.isNil())
			Raise("Add an \"Start\" function to your script.");
		if (!m_LuaStart.isFunction())
			Raise("\"Update\" must be a function.");

		m_LuaUpdate = m_pLua->GetGlobal("Update");
		if (m_LuaUpdate.isNil())
			Raise("Add an \"Update\" function to your script.");
		if (!m_LuaUpdate.isFunction())
			Raise("\"Update\" must be a function.");

		m_IsLoaded = true;
	}
	catch (const std::exception& except) {
		ErrorBoxA(except.what(), "Error");
		m_IsStarted = false;
	}
}