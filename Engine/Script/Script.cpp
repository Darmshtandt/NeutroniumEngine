// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <filesystem>

#include <Script/Script.h>
#include <Script/EnumWrapper.h>

#include <Objects/Object.h>
#include <Objects/Primitives/Primitive.h>
#include <Objects/Entities/Entity.h>
#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameSound.h>
#include <Objects/Entities/GameModel.h>


Script::Script(NotNull<Lua*> pLua, const std::string& filePath, NotNull<Object*> pObject) :
	m_pLua(pLua),
	m_FilePath(filePath),
	m_pObject(pObject),
	m_LuaUpdate(nullptr),
	m_LuaStart(nullptr) 
{
	m_pLua->GetGlobalNamespace()
		.beginNamespace("DataType")
		.addVariable<Int>("STRING", Script::Data::STRING)
		.addVariable<Int>("NUMBER", Script::Data::NUMBER)
		.addVariable<Int>("BOOL", Script::Data::BOOL)
		.endNamespace();
	m_pLua->GetGlobalNamespace()
		.beginClass<Lua>("Lua")
		.addConstructor<void(*)(Scene*)>()
		.endClass();
	m_pLua->GetGlobalNamespace()
		.beginClass<Script>("Script")
		//.addConstructor<void(*)(Lua*)>()
		.addFunction("AddData", &Script::AddData)
		.addFunction("GetDataValue", &Script::GetDataValue)
		.endClass();
	m_pLua->SetGlobal("g_ThisScript", this);
}

Script::Script(Script&& script) noexcept :
	m_AllData(std::move(script.m_AllData)),
	m_LuaUpdate(std::move(script.m_LuaUpdate)),
	m_LuaStart(std::move(script.m_LuaStart)),
	m_FilePath(std::move(script.m_FilePath)),

	m_pLua(script.m_pLua),
	m_pObject(script.m_pObject),

	m_IsStarted(script.m_IsStarted),
	m_IsLoaded(script.m_IsLoaded)
{
	script.m_pLua = nullptr;
	script.m_pObject = nullptr;
	script.m_LuaUpdate = nullptr;
	script.m_LuaStart = nullptr;
}


void Script::Load() {
	if (m_IsLoaded)
		m_IsLoaded = false;

	_SetGlobalObject("g_ThisObject", m_pObject);

	try {
		if (m_pLua->LoadFile(m_FilePath) != LUA_OK)
			Raise(m_pLua->GetLastError(), "Script error");

		std::string newPath = std::filesystem::current_path().string() + '\\' + m_FilePath;
		while (newPath.back() != '\\' && newPath.back() != '/')
			newPath.erase(newPath.end() - 1);

		m_pLua->SetPath(newPath);
		if (m_pLua->PCall(0, LUA_MULTRET, 0) != LUA_OK)
			Raise(m_pLua->GetLastError(), "Script error");

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
		Nt::MessageWindow(except.what(), "Error").ShowError();
		m_IsStarted = false;
	}
}

void Script::Start() {
	try {
		Load();

		_SetGlobalThis();
		_Call(m_LuaStart);

		m_IsStarted = true;
	}
	catch (const std::exception& except) {
		Raise(except.what());
	}
}

void Script::Update(const Float& time) {
	if (!m_IsStarted)
		return;

	try {
		_SetGlobalThis();
		_Call(m_LuaUpdate, time);
	}
	catch (const std::exception& except) {
		m_IsStarted = false;
		Raise(except.what());
	}
}

void Script::Stop() noexcept {
	m_IsStarted = false;
}

void Script::AddData(const std::string& fieldName, const std::string& name, const Data::Types& type) {
	if (FindDataByName(name) == m_AllData.end())
		m_AllData.emplace_back(type, fieldName, name, "");
}

void Script::ClearData() {
	m_AllData.clear();
}

std::vector<Script::Data> Script::GetScriptData() const {
	return m_AllData;
}

std::string Script::GetDataValue(const std::string& name) {
	auto data = FindDataByName(name);
	if (data == m_AllData.end()) {
		Nt::MessageWindow("Failed to find script data.", "Error").ShowError();
		return "";
	}

	return data->Value;
}

luabridge::LuaRef Script::GetGlobal(const std::string& name) const {
	return m_pLua->GetGlobal(name);
}

std::string Script::GetFilePath() const {
	return m_FilePath;
}

Lua* Script::GetLua() noexcept {
	return m_pLua;
}

Bool Script::IsLoaded() const noexcept {
	return m_IsLoaded;
}

void Script::SetScriptData(const std::vector<Data>& data) {
	m_AllData = data;
}

void Script::SetDataValue(const std::string& name, const Nt::String& value) {
	auto data = FindDataByName(name);
	if (data == m_AllData.end()) {
		Nt::MessageWindow("Failed to find script data.", L"Error").ShowError();
		return;
	}

	data->Value = value;
}

Script& Script::operator=(Script&& script) noexcept {
	if (this == &script)
		return *this;

	m_AllData = std::move(script.m_AllData);
	m_LuaUpdate = std::move(script.m_LuaUpdate);
	m_LuaStart = std::move(script.m_LuaStart);
	m_FilePath = std::move(script.m_FilePath);

	m_pLua = script.m_pLua;
	m_pObject = script.m_pObject;

	m_IsStarted = script.m_IsStarted;
	m_IsLoaded = script.m_IsLoaded;

	script.m_pLua = nullptr;
	script.m_pObject = nullptr;
	script.m_LuaUpdate = nullptr;
	script.m_LuaStart = nullptr;

	return *this;
}

std::vector<Script::Data>::iterator Script::FindDataByName(const std::string& name) {
	return std::find_if(m_AllData.begin(), m_AllData.end(),
		[&] (const Data& data) { return (data.Name == name); });
}

void Script::_SetGlobalThis() const {
	Assert(m_pLua->SetGlobal("g_ThisScript", this), "Failed to set global Script");
}
Bool Script::_SetGlobalObject(const std::string& name, Object* pObject) const {
	if (pObject->GetTypeToken() == Primitive::GetClassTypeToken())
		return m_pLua->SetGlobal(name, static_cast<Primitive*>(pObject));

	if (pObject->GetTypeToken() == Entity::GetClassTypeToken())
		return m_pLua->SetGlobal(name, static_cast<Entity*>(pObject));

	Raise("Invalid object type");
	return false;
}
Bool Script::_SetGlobalEntity(const std::string& name, Entity* pEntity) const {
	if (pEntity->GetToken() == GameCamera::GetClassToken())
		return m_pLua->SetGlobal(name, static_cast<GameCamera*>(pEntity));

	if (pEntity->GetToken() == GameModel::GetClassToken())
		return m_pLua->SetGlobal(name, static_cast<GameModel*>(pEntity));

	Raise("Invalid entity type");
	return false;
}