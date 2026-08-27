// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Main.h>

#include <Script/Script.h>
#include <Script/EnumWrapper.h>

#include <Objects/Object.h>
#include <Objects/Primitives/Primitive.h>
#include <Objects/Entities/Entity.h>
#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameSound.h>
#include <Objects/Entities/GameModel.h>
#include <Scene.h>
#include <Nt/Core/EventBus.h>


template <class _Vector, typename = typename std::enable_if_t<Nt::is_Vector_v<_Vector>>>
void AddVectorToLua(NotNull<Lua*> pLua, const std::string& className) {
	auto vectorClass = pLua->GetGlobalNamespace()
		.beginClass<_Vector>(className.c_str());

	constexpr uInt vectorSize = _Vector::Size;
	using _Ty = _Vector::ValueType;
	using Vector = Nt::Vector<_Ty, vectorSize>;


	constexpr Bool is2D = std::is_same_v<_Vector, Nt::Vector2D<_Ty>>;
	constexpr Bool is3D = std::is_same_v<_Vector, Nt::Vector3D<_Ty>>;
	constexpr Bool is4D = std::is_same_v<_Vector, Nt::Vector4D<_Ty>>;

	if constexpr (is2D)
		vectorClass.addConstructor<void(*)(const _Ty&, const _Ty&)>();
	else if constexpr (is3D)
		vectorClass.addConstructor<void(*)(const _Ty&, const _Ty&, const _Ty&)>();
	else if constexpr (is4D)
		vectorClass.addConstructor<void(*)(const _Ty&, const _Ty&, const _Ty&, const _Ty&)>();

	vectorClass.addProperty("x", &_Vector::x);
	vectorClass.addProperty("y", &_Vector::y);

	if constexpr (is3D || is4D) {
		vectorClass.addProperty("z", &_Vector::z);
		if constexpr (is4D)
			vectorClass.addProperty("w", &_Vector::w);
	}

	vectorClass
		.addFunction("Length", &_Vector::Length)
		.addFunction("LengthSquare", &_Vector::LengthSquare)
		.addFunction("GetNormalize", &_Vector::GetNormalize)
		.addFunction("Dot", &_Vector::Dot)

		.addFunction("Add", static_cast<
			Vector(Vector::*)(const Vector&) const>(
				&Vector::operator+))
		.addFunction("Sub", static_cast<
			Vector(Vector::*)(const Vector&) const>(
				&Vector::operator-))
		.addFunction("Mult", static_cast<
			Vector(Vector::*)(const Vector&) const>(
				&Vector::operator*))
		.addFunction("Div", static_cast<
			Vector(Vector::*)(const Vector&) const>(
				&Vector::operator/))

		.addFunction("AddNum", static_cast<
			Vector(Vector::*)(const _Ty&) const>(
				&Vector::operator+))
		.addFunction("SubNum", static_cast<
			Vector(Vector::*)(const _Ty&) const>(
				&Vector::operator-))
		.addFunction("MultNum", static_cast<
			Vector(Vector::*)(const _Ty&) const>(
				&Vector::operator*))
		.addFunction("DivNum", static_cast<
			Vector(Vector::*)(const _Ty&) const>(
				&Vector::operator/))

		.addFunction("IsLess", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator <))
		.addFunction("IsLessOrEqual", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator <=))
		.addFunction("IsEqual", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator ==))
		.addFunction("IsNotEqual", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator !=))
		.addFunction("IsGreater", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator >))
		.addFunction("IsGreaterOrEqual", static_cast<
			Bool(Vector::*)(const Vector&) const>(
				&Vector::operator >=))

		.addFunction("IsLessNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator <))
		.addFunction("IsLessOrEqualNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator <=))
		.addFunction("IsEqualNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator ==))
		.addFunction("IsNotEqualNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator !=))
		.addFunction("IsGreaterNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator >))
		.addFunction("IsGreaterOrEqualNum", static_cast<
			Bool(Vector::*)(const _Ty&) const>(
				&Vector::operator >=))
		.endClass();
}

Lua::Lua(NotNull<Scene*> pScene) : m_ScenePtr(pScene) {
	m_pState = RequireNotNull(luaL_newstate());
	luaL_openlibs(m_pState);
	_AddClasses();
}

Lua::Lua(Lua&& lua) noexcept :
	m_pState(lua.m_pState),
	m_ScenePtr(lua.m_ScenePtr)
{
	lua.m_pState = nullptr;
	lua.m_ScenePtr = nullptr;
}

Lua::~Lua() {
	if (m_pState != nullptr)
		lua_close(m_pState);
}

Int Lua::LoadFile(const std::string& filePath) {
	return luaL_loadfile(m_pState, filePath.c_str());
}

Int Lua::PCall(const Int& numArgs, const Int& numResults, const Int& errorFunc) {
	return lua_pcall(m_pState, numArgs, numResults, errorFunc);
}

Lua& Lua::operator=(Lua&& lua) noexcept {
	if (this == &lua)
		return *this;

	m_pState = lua.m_pState;
	m_ScenePtr = lua.m_ScenePtr;

	lua.m_pState = nullptr;
	lua.m_ScenePtr = nullptr;
	return *this;
}

void Lua::SetPath(const Nt::String& path) {
	lua_getglobal(m_pState, "package");
	lua_getfield(m_pState, -1, "path");

	std::string cur_path = lua_tostring(m_pState, -1);
	cur_path += ";" + path + "?.lua";

	lua_pop(m_pState, 1);
	lua_pushstring(m_pState, cur_path.c_str());
	lua_setfield(m_pState, -2, "path");
	lua_pop(m_pState, 1);
}

luabridge::Namespace Lua::GetGlobalNamespace() const {
	return luabridge::getGlobalNamespace(m_pState);
}

luabridge::LuaRef Lua::GetGlobal(const Nt::String& name) const {
	return luabridge::getGlobal(m_pState, name);
}

lua_State* Lua::GetState() const {
	return m_pState;
}

Scene* Lua::GetScenePtr() const {
	return m_ScenePtr;
}

void Lua::_AddClasses() {
	GetGlobalNamespace()
		.beginClass<Nt::String>("String")
		.addConstructor<void(*)(const std::string&)>()
		.addFunction("ToBool", &Nt::String::operator Bool)
		.addFunction("ToInt", &Nt::String::operator Int)
		.addFunction("TouInt", &Nt::String::operator uInt)
		.addFunction("ToFloat", &Nt::String::operator Float)
		.addFunction("ToDouble", &Nt::String::operator Double)
		.addFunction("c_str", &Nt::String::c_str)
		.endClass();

	GetGlobalNamespace()
		.beginClass<NotNull<Nt::EventBus*>>("EventBusPtr")
		.addConstructor<void(*)(Nt::EventBus*)>()
		.endClass();

	AddVectorToLua<Nt::Float2D, Float>(this, "Float2D");
	AddVectorToLua<Nt::Double2D, Double>(this, "Double2D");
	AddVectorToLua<Nt::uInt2D, uInt>(this, "uInt2D");
	AddVectorToLua<Nt::Int2D, Int>(this, "Int2D");

	AddVectorToLua<Nt::Float3D, Float>(this, "Float3D");
	AddVectorToLua<Nt::Double3D, Double>(this, "Double3D");
	AddVectorToLua<Nt::uInt3D, uInt>(this, "uInt3D");
	AddVectorToLua<Nt::Int3D, Int>(this, "Int3D");

	AddVectorToLua<Nt::Float4D, Float>(this, "Float4D");
	AddVectorToLua<Nt::Double4D, Double>(this, "Double4D");
	AddVectorToLua<Nt::uInt4D, uInt>(this, "uInt4D");
	AddVectorToLua<Nt::Int4D, Int>(this, "Int4D");

	_AddInput();

	GetGlobalNamespace()
		.beginClass<Object>("Object")
		.addFunction("Translate", &Object::Translate)
		.addFunction("Rotate", &Object::Rotate)
		.addFunction("Scale", &Object::Scale)

		.addFunction("CheckCollision", &Object::CheckCollision)
		.addFunction("AddForce", &Object::AddForce)
		.addFunction("EnabledGravitation", &Object::EnabledGravitation)
		.addFunction("ToggleGravitation", &Object::ToggleGravitation)
		.addFunction("EnabledCollider", &Object::EnabledCollider)
		.addFunction("ToggleCollider", &Object::ToggleCollider)
		//.addFunction("GetGravityDirection", &Object::GetGravityDirection)
		//.addFunction("GetLinearVelocity", &Object::GetLinearVelocity)
		//.addFunction("GetLinearAcceleration", &Object::GetLinearAcceleration)
		//.addFunction("GetWeight", &Object::GetWeight)
		//.addFunction("IsObjectCollided", &Object::IsObjectCollided)
		//.addFunction("IsEnabledGravitation", &Object::IsEnabledGravitation)
		//.addFunction("SetGravityDirection", &Object::SetGravityDirection)
		//.addFunction("SetLinearAcceleration", &Object::SetLinearAcceleration)
		//.addFunction("SetLinearVelocity", &Object::SetLinearVelocity)
		//.addFunction("SetFriction", &Object::SetFriction)
		//.addFunction("SetFrictionStatic", &Object::SetFrictionStatic)
		//.addFunction("SetMass", &Object::SetMass)

		.addFunction("GetLayerName", &Object::GetLayerName)
		.addFunction("GetName", &Object::GetName)
		.addFunction("GetPosition", &Object::GetPosition)
		.addFunction("GetSize", &Object::GetSize)
		.addFunction("GetAngle", &Object::GetAngle)
		.addFunction("GetColor", &Object::GetColor)

		.addFunction("SetLayerName", &Object::SetLayerName)
		.addFunction("SetName", &Object::SetName)
		.addFunction("SetPosition", &Object::SetPosition)
		.addFunction("SetSize", &Object::SetSize)
		.addFunction("SetAngle", &Object::SetAngle)
		.addFunction("SetColor", &Object::SetColor)
		.endClass()

		.deriveClass<Primitive, Object>("Primitive")
		.addFunction("SetTextureOffset", &Primitive::SetTextureOffset)
		.addFunction("SetTextureScale", &Primitive::SetTextureScale)
		.addFunction("GetTextureOffset", &Primitive::GetTextureOffset)
		.addFunction("GetTextureScale", &Primitive::GetTextureScale)
		.addFunction("SetSize", &Primitive::SetSize)
		.endClass()

		.deriveClass<Entity, Object>("Entity")
		.endClass()

		.deriveClass<GameCamera, Entity>("Camera")
		.addConstructor<void(*)(const Nt::String&)>()
		.addFunction("SetOrigin", &GameCamera::SetPosition)
		.addFunction("SetPosition", &GameCamera::SetPosition)
		.addFunction("SetAngle", &GameCamera::SetAngle)
		.endClass()

		.deriveClass<GameSound, Entity>("Sound")
		.addConstructor<void(*)(const Nt::String&)>()
		.addFunction("Play", &GameSound::Play)
		.addFunction("Stop", &GameSound::Stop)
		.addFunction<void (GameSound::*)(const Bool&)>("ToggleLooping", &GameSound::ToggleLooping)
		.addFunction("SetPosition", &GameSound::SetPosition)
		.endClass()

		.deriveClass<GameModel, Entity>("Model")
		.addConstructor<void(*)(const Nt::String&)>()
		.endClass();

	GetGlobalNamespace()
		.beginClass<ObjectContainer>("ObjectContainer")
		.addConstructor<void(*)()>()
		.addFunction("Size", &ObjectContainer::size)
		.endClass();

	GetGlobalNamespace()
		.beginClass<ObjectContainer>("ObjectContainer")
		.addConstructor<void(*)()>()
		.addFunction("Size", &ObjectContainer::size)
		.endClass();

	GetGlobalNamespace()
		.beginClass<Scene>("Scene")
		//.addConstructor<void(*)(NotNull<Nt::EventBus*>)>()
		.addFunction(
			"AddObject", static_cast<void (Scene::*)(NotNull<Object*>)>(&Scene::AddObject))
		.addFunction("RemoveObject", &Scene::RemoveObject)
		.addFunction("Clear", &Scene::Clear)
		.addFunction("AllowIntersectionOfLayers", &Scene::AllowLayerOverlap)
		.addFunction("GetAllObjects", &Scene::GetObjects)
		.addFunction("GetObjectByName", &Scene::GetObjectPtrByName)
		.addFunction("GetObject", &Scene::GetObjectPtr)
		.addFunction("GetSoundByName", &Scene::GetObjectPtrByTypeAndName<GameSound>)
		.addFunction("GetModelByName", &Scene::GetObjectPtrByTypeAndName<GameModel>)
		.addFunction("GetCameraByName", &Scene::GetObjectPtrByTypeAndName<GameCamera>)
		.endClass();

	GetGlobalNamespace()
		.beginClass<Nt::Timer>("Timer")
		.addConstructor<void(*)()>()
		.addFunction("GetElapsedTimeMs", &Nt::Timer::GetElapsedTimeMs)
		.addFunction("Restart", &Nt::Timer::Restart)
		.endClass();

	GetGlobalNamespace()
		.addFunction("Sleep", &Sleep);

	SetGlobal("g_Scence", m_ScenePtr);
}

void Lua::_AddInput() {
	const cString keysSymbols = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM";
	const uInt symbolCount = strlen(keysSymbols);
	Char keyName[3] = "_0";
	luabridge::Namespace luaNamespace = GetGlobalNamespace().beginNamespace("Key");
	for (uInt i = 0; i < symbolCount; ++i) {
		keyName[1] = keysSymbols[i];
		luaNamespace.addVariable<Int>(keyName, Int(keysSymbols[i]));
	}

	luaNamespace.addVariable<Int>("_NUMPAD0", Nt::KEY_NUMPAD0);
	luaNamespace.addVariable<Int>("_NUMPAD1", Nt::KEY_NUMPAD1);
	luaNamespace.addVariable<Int>("_NUMPAD2", Nt::KEY_NUMPAD2);
	luaNamespace.addVariable<Int>("_NUMPAD3", Nt::KEY_NUMPAD3);
	luaNamespace.addVariable<Int>("_NUMPAD4", Nt::KEY_NUMPAD4);
	luaNamespace.addVariable<Int>("_NUMPAD5", Nt::KEY_NUMPAD5);
	luaNamespace.addVariable<Int>("_NUMPAD6", Nt::KEY_NUMPAD6);
	luaNamespace.addVariable<Int>("_NUMPAD7", Nt::KEY_NUMPAD7);
	luaNamespace.addVariable<Int>("_NUMPAD8", Nt::KEY_NUMPAD8);
	luaNamespace.addVariable<Int>("_NUMPAD9", Nt::KEY_NUMPAD9);
	luaNamespace.addVariable<Int>("_SHIFT", Nt::KEY_SHIFT);
	luaNamespace.addVariable<Int>("_LSHIFT", Nt::KEY_LSHIFT);
	luaNamespace.addVariable<Int>("_RSHIFT", Nt::KEY_RSHIFT);
	luaNamespace.addVariable<Int>("_RETURN", Nt::KEY_RETURN);
	luaNamespace.addVariable<Int>("_CONTROL", Nt::KEY_CONTROL);
	luaNamespace.addVariable<Int>("_ESCAPE", Nt::KEY_ESCAPE);
	luaNamespace.addVariable<Int>("_MENU", Nt::KEY_MENU);
	luaNamespace.addVariable<Int>("_RMENU", Nt::KEY_RMENU);
	luaNamespace.addVariable<Int>("_LMENU", Nt::KEY_LMENU);
	luaNamespace.addVariable<Int>("_F1", Nt::KEY_F1);
	luaNamespace.addVariable<Int>("_F2", Nt::KEY_F2);
	luaNamespace.addVariable<Int>("_F3", Nt::KEY_F3);
	luaNamespace.addVariable<Int>("_F4", Nt::KEY_F4);
	luaNamespace.addVariable<Int>("_F5", Nt::KEY_F5);
	luaNamespace.addVariable<Int>("_F6", Nt::KEY_F6);
	luaNamespace.addVariable<Int>("_F7", Nt::KEY_F7);
	luaNamespace.addVariable<Int>("_F8", Nt::KEY_F8);
	luaNamespace.addVariable<Int>("_F9", Nt::KEY_F9);
	luaNamespace.addVariable<Int>("_F10", Nt::KEY_F10);
	luaNamespace.addVariable<Int>("_F11", Nt::KEY_F11);
	luaNamespace.addVariable<Int>("_F12", Nt::KEY_F12);
	luaNamespace.addVariable<Int>("_SPACE", Nt::KEY_SPACE);
	luaNamespace.endNamespace();

	GetGlobalNamespace()
		.beginNamespace("Button")
		.addVariable<Nt::Key>("_LEFT", Nt::BUTTON_LEFT)
		.addVariable<Nt::Key>("_RIGHT", Nt::BUTTON_RIGHT)
		.addVariable<Nt::Key>("_MIDDLE", Nt::BUTTON_MIDDLE)
		.addVariable<Nt::Key>("_X1", Nt::BUTTON_X1)
		.addVariable<Nt::Key>("_X2", Nt::BUTTON_X2)
		.endNamespace();

	GetGlobalNamespace()
		.beginClass<Nt::Keyboard>("Keyboard")
		.addConstructor<void(*)()>()
		.addFunction("Update", &Nt::Keyboard::Update)
		.addFunction("IsKeyPressed", &Nt::Keyboard::IsKeyPressed)
		.endClass();

	GetGlobalNamespace()
		.beginClass<Nt::Mouse>("Mouse")
		.addConstructor<void(*)()>()
		.addFunction("Update", &Nt::Mouse::Update)
		.addFunction("IsButtonPressed", &Nt::Mouse::IsButtonPressed)
		.addStaticFunction("GetCursorPosition", &Nt::Mouse::GetCursorPosition)
		.addStaticFunction("SetCursorPosition", &Nt::Mouse::SetCursorPosition)
		.endClass();
}