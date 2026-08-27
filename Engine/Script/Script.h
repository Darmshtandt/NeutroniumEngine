#pragma once

#include <Script/Lua.h>
#include <Nt/Core/MessageWindow.h>

class Object;
class Entity;

class Script {
public:
	struct Data {
		enum Types {
			STRING,
			NUMBER,
			BOOL
		};

		Data(const Types& type, const std::string& fieldName, const std::string& name, const std::string& value) :
			Type(type),
			FieldName(fieldName),
			Name(name),
			Value(value)
		{
		}

		Types Type;
		std::string FieldName;
		std::string Name;
		std::string Value;
	};

public:
	Script(NotNull<Lua*> pLua, const std::string& filePath, NotNull<Object*> pObject);
	Script(const Script& script) = delete;
	Script(Script&& script) noexcept;
	~Script() = default;

	void Load();

	void Start();
	void Update(const Float& time);
	void Stop() noexcept;

	void AddData(const std::string& fieldName, const std::string& name, const Data::Types& type);
	void ClearData();

	Script& operator = (const Script& script) = delete;
	Script& operator = (Script&& script) noexcept;

	std::vector<Data>::iterator FindDataByName(const std::string& name);

	std::vector<Data> GetScriptData() const;
	std::string GetDataValue(const std::string& name);
	luabridge::LuaRef GetGlobal(const std::string& name) const;
	std::string GetFilePath() const;
	Lua* GetLua() noexcept;
	Bool IsLoaded() const noexcept;

	template <typename _Ty>
	Bool SetGlobal(const std::string& name, _Ty* pData) const {
		if (m_IsLoaded) {
			Nt::MessageWindow("The global variable cannot be assigned because the script is already loaded.", "Warning").ShowWarning();
			return false;
		}

		return m_pLua->SetGlobal(name, pData);
	}
	void SetScriptData(const std::vector<Data>& data);
	void SetDataValue(const std::string& name, const Nt::String & value);

private:
	std::vector<Data> m_AllData;
	luabridge::LuaRef m_LuaUpdate;
	luabridge::LuaRef m_LuaStart;
	std::string m_FilePath;

	Lua* m_pLua = nullptr;
	Object* m_pObject = nullptr;

	Bool m_IsStarted = false;
	Bool m_IsLoaded = false;

private:
	template <class... Args>
	void _Call(luabridge::LuaRef& function, Args&& ... args) const {
		const luabridge::LuaResult result = function(std::forward<Args>(args)...);

		Assert(!result.hasFailed(), result.errorMessage());
	}

	void _SetGlobalThis() const;
	Bool _SetGlobalObject(const std::string& name, Object* pObject) const;
	Bool _SetGlobalEntity(const std::string& name, Entity* pEntity) const;
};