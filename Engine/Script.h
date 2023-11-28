#pragma once

class Object;

class Script {
public:
	struct Data : public Nt::ISerialization {
		enum Types {
			STRING,
			NUMBER,
			BOOL
		};

		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, Type, FieldName, Name, Value);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, Type, FieldName, Name, Value);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Types Type;
		Nt::String FieldName;
		Nt::String Name;
		Nt::String Value;
	};

public:
	Script() :
		m_LuaUpdate(nullptr),
		m_LuaStart(nullptr)
	{
	}

	void Initialize(Scence* pScence);
	void Uninitialize() {
		m_pLua->Uninitialize();
	}

	void Load(const Nt::String& filePath, Object* pObject);

	void Start() {
		if (!m_IsInitialized)
			Raise("Script not initialized");

		try {
			const luabridge::LuaResult result = m_LuaStart();
			if (result.hasFailed())
				throw std::exception(result.errorMessage().c_str());
			m_IsStarted = true;
		}
		catch (const std::exception& except) {
			ErrorBoxA(except.what(), "Error");
		}
	}
	void Update(const Float& time) {
		if (!m_IsInitialized)
			Raise("Script not initialized");

		if (m_IsStarted) {
			try {
				const luabridge::LuaResult result = m_LuaUpdate(time);
				if (result.hasFailed())
					throw std::exception(result.errorMessage().c_str());
			}
			catch (const std::exception& except) {
				ErrorBoxA(except.what(), "Error");
				m_IsStarted = false;
			}
		}
	}
	void Stop() noexcept {
		m_IsStarted = false;
	}

	void AddData(const Nt::String& fieldName, const Nt::String& name, const Data::Types& type) {
		if (_FindData(name) == m_Data.end()) {
			Data data;
			data.FieldName = fieldName;
			data.Name = name;
			data.Type = type;
			m_Data.push_back(data);
		}
	}
	void ClearData() {
		m_Data.clear();
	}

	std::vector<Data> GetScriptData() const {
		return m_Data;
	}
	Nt::String GetDataValue(const Nt::String& name) {
		auto data = _FindData(name);
		if (data == m_Data.end()) {
			ErrorBox(L"Failed to find script data.", L"Error");
			return "";
		}
		return data->Value;
	}
	luabridge::LuaRef GetGlobal(const Nt::String& name) const {
		return m_pLua->GetGlobal(name);
	}
	Nt::String GetFilePath() const {
		return m_FilePath;
	}
	Bool IsInitialized() const noexcept {
		return m_IsInitialized;
	}
	Bool IsLoaded() const noexcept {
		return m_IsLoaded;
	}

	template <typename _Ty>
	Bool SetGlobal(const Nt::String& name, _Ty* pData) const {
		if (m_IsLoaded) {
			WarningBox(L"The global variable cannot be assigned because the script is already loaded.", L"Warning");
			return false;
		}
		return m_pLua->SetGlobal(name, pData);
	}
	void SetScriptData(const std::vector<Data>& data) {
		m_Data = data;
	}
	void SetDataValue(const Nt::String& name, const Nt::String& value) {
		auto data = _FindData(name);
		if (data == m_Data.end()) {
			ErrorBox(L"Failed to find script data.", L"Error");
			return;
		}
		data->Value = value;
	}

private:
	std::vector<Data> m_Data;
	luabridge::LuaRef m_LuaUpdate;
	luabridge::LuaRef m_LuaStart;
	Lua* m_pLua = nullptr;
	Object* m_pObject = nullptr;
	Nt::String m_FilePath;
	Bool m_IsStarted = false;
	Bool m_IsInitialized = false;
	Bool m_IsLoaded = false;

private:
	std::vector<Data>::iterator _FindData(const Nt::String& name) {
		return std::find_if(m_Data.begin(), m_Data.end(),
			[&](const Data& data) { return (data.Name == name); });
	}

	std::string _GetLastError() const {
		std::string errorMsg = lua_tostring(m_pLua->GetState(), -1);
		lua_pop(m_pLua->GetState(), 1);
		return errorMsg;
	}
};