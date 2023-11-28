#pragma once

template <class _Ty>
struct EnumWrapper {
	static auto push(lua_State* pState, const _Ty& value, std::error_code& ec)
		-> std::enable_if_t<std::is_enum_v<_Ty>, Bool>
	{
		lua_pushnumber(pState, (uInt)value);
		return true;
	}

	static auto get(lua_State* pState, const Int& index)
		-> std::enable_if_t<std::is_enum_v<_Ty>, _Ty>
	{
		return (_Ty)lua_tointeger(pState, index);
	}
};