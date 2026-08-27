#pragma once

#include <Nt/Core/Utilities.h>

extern "C" {
#include <Lua/lua.hpp>
}

#include <LuaBridge/LuaBridge.h>

class Scene;

class Lua {
public:
	Lua(NotNull<Scene*> pScene);
	Lua(Lua&& lua) noexcept;
	Lua(const Lua& lua) = delete;
	~Lua();

	[[nodiscard]]
	Int LoadFile(const std::string& filePath);

	[[nodiscard]]
	Int PCall(const Int& numArgs, const Int& numResults, const Int& errorFunc);

	Lua& operator = (const Lua& lua) = delete;
	Lua& operator = (Lua&& lua) noexcept;

	template <typename _Ty>
	Bool SetGlobal(const Nt::String& name, _Ty* pData) const {
		return luabridge::setGlobal(m_pState, RequireNotNull(pData), name);
	}

	void SetPath(const Nt::String& path);

	luabridge::Namespace GetGlobalNamespace() const;
	luabridge::LuaRef GetGlobal(const Nt::String& name) const;

	lua_State* GetState() const;
	Scene* GetScenePtr() const;

	std::string GetLastError() const {
		std::string errorMsg = lua_tostring(m_pState, -1);
		lua_pop(m_pState, 1);
		return errorMsg;
	}

private:
	lua_State* m_pState;
	Scene* m_ScenePtr = nullptr;

private:
	void _AddClasses();
	void _AddInput();
};