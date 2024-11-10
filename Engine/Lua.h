#pragma once

class Scence;

class Lua {
public:
	Lua() = default;
	Lua(const Lua& lua) {
		Initialize(lua.m_ScencePtr);
	}
	~Lua() {
		Uninitialize();
	}

	void Initialize(Scence* pScence);

	void Uninitialize() {
		if (!m_IsInitialized)
			return;
		if (m_pState == nullptr)
			Raise("Lua state is null.");

		lua_close(m_pState);
		m_ScencePtr = nullptr;
		m_IsInitialized = false;
	}

	template <typename _Ty>
	Bool SetGlobal(const Nt::String& name, _Ty* pData) const {
		if (pData == nullptr)
			Raise("Data pointer is null.");
		return luabridge::setGlobal(m_pState, pData, name);
	}

	void SetPath(const Nt::String& path) {
		lua_getglobal(m_pState, "package");
		lua_getfield(m_pState, -1, "path");

		std::string cur_path = lua_tostring(m_pState, -1);
		cur_path += ";" + path + "?.lua";

		lua_pop(m_pState, 1);
		lua_pushstring(m_pState, cur_path.c_str());
		lua_setfield(m_pState, -2, "path");
		lua_pop(m_pState, 1);
	}

	luabridge::Namespace GetGlobalNamespace() const {
		return luabridge::getGlobalNamespace(m_pState);
	}
	luabridge::LuaRef GetGlobal(const Nt::String& name) const {
		return luabridge::getGlobal(m_pState, name);
	}

	lua_State* GetState() const {
		return m_pState;
	}
	Scence* GetScencePtr() const {
		return m_ScencePtr;
	}

	Bool IsInitialized() const noexcept {
		return m_IsInitialized;
	}

private:
	lua_State* m_pState;
	Scence* m_ScencePtr = nullptr;
	Bool m_IsInitialized;
};