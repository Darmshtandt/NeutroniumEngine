#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Primitive.h>
#include <Entity.h>
#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>
#include <Selector.h>
#include <ObjectsTree.h>
#include <Scence.h>
#include <EnumWrapper.h>

namespace luabridge {
	template<> 
	struct Stack<Nt::Keyboard::Key> : EnumWrapper<Nt::Keyboard::Key> 
	{
	};
	template<> 
	struct Stack<Nt::Mouse::Button> : EnumWrapper<Nt::Mouse::Button> 
	{
	};
}

template <class _Vector, typename _Ty>
void AddVectorToLua(Lua* pLua, const std::string& className) {
	auto vectorClass = pLua->GetGlobalNamespace()
		.beginClass<_Vector>(className.c_str());

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
			_Vector(_Vector::*)(const _Vector&) const>(
				&_Vector::operator+))
		.addFunction("Sub", static_cast<
			_Vector(_Vector::*)(const _Vector&) const>(
				&_Vector::operator-))
		.addFunction("Mult", static_cast<
			_Vector(_Vector::*)(const _Vector&) const>(
				&_Vector::operator*))
		.addFunction("Div", static_cast<
			_Vector(_Vector::*)(const _Vector&) const>(
				&_Vector::operator/))

		.addFunction("AddNum", static_cast<
			_Vector(_Vector::*)(const _Ty&) const>(
				&_Vector::operator+))
		.addFunction("SubNum", static_cast<
			_Vector(_Vector::*)(const _Ty&) const>(
				&_Vector::operator-))
		.addFunction("MultNum", static_cast<
			_Vector(_Vector::*)(const _Ty&) const>(
				&_Vector::operator*))
		.addFunction("DivNum", static_cast<
			_Vector(_Vector::*)(const _Ty&) const>(
				&_Vector::operator/))

		.addFunction("IsLess", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator <))
		.addFunction("IsLessOrEqual", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator <=))
		.addFunction("IsEqual", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator ==))
		.addFunction("IsNotEqual", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator !=))
		.addFunction("IsGreater", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator >))
		.addFunction("IsGreaterOrEqual", static_cast<
			Bool(_Vector::*)(const _Vector&) const>(
				&_Vector::operator >=))

		.addFunction("IsLessNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator <))
		.addFunction("IsLessOrEqualNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator <=))
		.addFunction("IsEqualNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator ==))
		.addFunction("IsNotEqualNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator !=))
		.addFunction("IsGreaterNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator >))
		.addFunction("IsGreaterOrEqualNum", static_cast<
			Bool(_Vector::*)(const _Ty&) const>(
				&_Vector::operator >=))
		.endClass();
}

void Lua::Initialize(Scence* pScence) {
	if (pScence == nullptr)
		Raise("Scence pointer is null");

	m_ScencePtr = pScence;
	m_pState = luaL_newstate();
	luaL_openlibs(m_pState);

	GetGlobalNamespace()
		.beginClass<Nt::String>("String")
		.addConstructor<void(*)(const std::string&)>()
		.addFunction("ToBool", static_cast<
			Bool(Nt::String::*)() const>(&Nt::String::operator Bool))
		.addFunction("ToInt", static_cast<
			Int(Nt::String::*)() const>(&Nt::String::operator Int))
		.addFunction("TouInt", static_cast<
			uInt(Nt::String::*)() const>(&Nt::String::operator uInt))
		.addFunction("ToFloat", static_cast<
			Float(Nt::String::*)() const>(&Nt::String::operator Float))
		.addFunction("ToDouble", static_cast<
			Double(Nt::String::*)() const>(&Nt::String::operator Double))
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
	
	const cString keysSymbols = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM";
	const uInt symbolCount = strlen(keysSymbols);
	Char keyName[3] = "_0";
	luabridge::Namespace luaNamespace = GetGlobalNamespace().beginNamespace("Key");
	for (uInt i = 0; i < symbolCount; ++i) {
		keyName[1] = keysSymbols[i];
		luaNamespace.addVariable<Int>(keyName, keysSymbols[i]);
	}

	luaNamespace.addVariable<Int>("_NUMPAD0", Nt::Keyboard::KEY_NUMPAD0);
	luaNamespace.addVariable<Int>("_NUMPAD1", Nt::Keyboard::KEY_NUMPAD1);
	luaNamespace.addVariable<Int>("_NUMPAD2", Nt::Keyboard::KEY_NUMPAD2);
	luaNamespace.addVariable<Int>("_NUMPAD3", Nt::Keyboard::KEY_NUMPAD3);
	luaNamespace.addVariable<Int>("_NUMPAD4", Nt::Keyboard::KEY_NUMPAD4);
	luaNamespace.addVariable<Int>("_NUMPAD5", Nt::Keyboard::KEY_NUMPAD5);
	luaNamespace.addVariable<Int>("_NUMPAD6", Nt::Keyboard::KEY_NUMPAD6);
	luaNamespace.addVariable<Int>("_NUMPAD7", Nt::Keyboard::KEY_NUMPAD7);
	luaNamespace.addVariable<Int>("_NUMPAD8", Nt::Keyboard::KEY_NUMPAD8);
	luaNamespace.addVariable<Int>("_NUMPAD9", Nt::Keyboard::KEY_NUMPAD9);
	luaNamespace.addVariable<Int>("_SHIFT", Nt::Keyboard::KEY_SHIFT);
	luaNamespace.addVariable<Int>("_LSHIFT", Nt::Keyboard::KEY_LSHIFT);
	luaNamespace.addVariable<Int>("_RSHIFT", Nt::Keyboard::KEY_RSHIFT);
	luaNamespace.addVariable<Int>("_RETURN", Nt::Keyboard::KEY_RETURN);
	luaNamespace.addVariable<Int>("_CONTROL", Nt::Keyboard::KEY_CONTROL);
	luaNamespace.addVariable<Int>("_ESCAPE", Nt::Keyboard::KEY_ESCAPE);
	luaNamespace.addVariable<Int>("_MENU", Nt::Keyboard::KEY_MENU);
	luaNamespace.addVariable<Int>("_RMENU", Nt::Keyboard::KEY_RMENU);
	luaNamespace.addVariable<Int>("_LMENU", Nt::Keyboard::KEY_LMENU);
	luaNamespace.addVariable<Int>("_F1", Nt::Keyboard::KEY_F1);
	luaNamespace.addVariable<Int>("_F2", Nt::Keyboard::KEY_F2);
	luaNamespace.addVariable<Int>("_F3", Nt::Keyboard::KEY_F3);
	luaNamespace.addVariable<Int>("_F4", Nt::Keyboard::KEY_F4);
	luaNamespace.addVariable<Int>("_F5", Nt::Keyboard::KEY_F5);
	luaNamespace.addVariable<Int>("_F6", Nt::Keyboard::KEY_F6);
	luaNamespace.addVariable<Int>("_F7", Nt::Keyboard::KEY_F7);
	luaNamespace.addVariable<Int>("_F8", Nt::Keyboard::KEY_F8);
	luaNamespace.addVariable<Int>("_F9", Nt::Keyboard::KEY_F9);
	luaNamespace.addVariable<Int>("_F10", Nt::Keyboard::KEY_F10);
	luaNamespace.addVariable<Int>("_F11", Nt::Keyboard::KEY_F11);
	luaNamespace.addVariable<Int>("_F12", Nt::Keyboard::KEY_F12);
	luaNamespace.addVariable<Int>("_F13", Nt::Keyboard::KEY_F13);
	luaNamespace.addVariable<Int>("_F14", Nt::Keyboard::KEY_F14);
	luaNamespace.addVariable<Int>("_F15", Nt::Keyboard::KEY_F15);
	luaNamespace.addVariable<Int>("_F16", Nt::Keyboard::KEY_F16);
	luaNamespace.addVariable<Int>("_F17", Nt::Keyboard::KEY_F17);
	luaNamespace.addVariable<Int>("_F18", Nt::Keyboard::KEY_F18);
	luaNamespace.addVariable<Int>("_F19", Nt::Keyboard::KEY_F19);
	luaNamespace.addVariable<Int>("_F20", Nt::Keyboard::KEY_F20);
	luaNamespace.addVariable<Int>("_F21", Nt::Keyboard::KEY_F21);
	luaNamespace.addVariable<Int>("_F22", Nt::Keyboard::KEY_F22);
	luaNamespace.addVariable<Int>("_F23", Nt::Keyboard::KEY_F23);
	luaNamespace.addVariable<Int>("_F24", Nt::Keyboard::KEY_F24);
	luaNamespace.addVariable<Int>("_SPACE", Nt::Keyboard::KEY_SPACE);
	luaNamespace.endNamespace();

	GetGlobalNamespace()
		.beginNamespace("Button")
		.addVariable<Int>("_LEFT", Nt::Mouse::BUTTON_LEFT)
		.addVariable<Int>("_RIGHT", Nt::Mouse::BUTTON_RIGHT)
		.addVariable<Int>("_MIDDLE", Nt::Mouse::BUTTON_MIDDLE)
		.addVariable<Int>("_X1", Nt::Mouse::BUTTON_X1)
		.addVariable<Int>("_X2", Nt::Mouse::BUTTON_X2)
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

	GetGlobalNamespace()
		.beginClass<Object>("Object")
		.addConstructor<void(*)(const ObjectTypes&, const std::string&)>()
		.addFunction("Translate", &Object::Translate)
		.addFunction("Rotate", &Object::Rotate)
		.addFunction("Resize", &Object::Resize)

		.addFunction("CheckCollision", &Object::CheckCollision)
		.addFunction("AddForce", &Object::AddForce)
		.addFunction("EnableGravitation", &Object::EnableGravitation)
		.addFunction("DisableGravitation", &Object::DisableGravitation)
		.addFunction("EnableCollider", &Object::EnableCollider)
		.addFunction("DisableCollider", &Object::DisableCollider)
		.addFunction("GetGravityDirection", &Object::GetGravityDirection)
		.addFunction("GetLinearVelocity", &Object::GetLinearVelocity)
		.addFunction("GetLinearAcceleration", &Object::GetLinearAcceleration)
		.addFunction("GetWeight", &Object::GetWeight)
		.addFunction("IsObjectCollided", &Object::IsObjectCollided)
		.addFunction("IsEnabledGravitation", &Object::IsEnabledGravitation)
		.addFunction("SetGravityDirection", &Object::SetGravityDirection)
		.addFunction("SetLinearAcceleration", &Object::SetLinearAcceleration)
		.addFunction("SetLinearVelocity", &Object::SetLinearVelocity)
		.addFunction("SetFriction", &Object::SetFriction)
		.addFunction("SetFrictionStatic", &Object::SetFrictionStatic)
		.addFunction("SetMass", &Object::SetMass)

		.addFunction("GetLayerName", &Object::GetLayerName)
		.addFunction("GetName", &Object::GetName)
		.addFunction("GetPosition", &Object::GetPosition)
		.addFunction("GetSize", &Object::GetSize)
		.addFunction("GetAngle", &Object::GetAngle)
		.addFunction("GetAngleOrigin", &Object::GetAngleOrigin)
		.addFunction("GetOrigin", &Object::GetOrigin)
		.addFunction("GetColor", &Object::GetColor)

		.addFunction("SetLayerName", &Object::SetLayerName)
		.addFunction("SetName", &Object::SetName)
		.addFunction("SetPosition", &Object::SetPosition)
		.addFunction("SetSize", &Object::SetSize)
		.addFunction("SetAngle", &Object::SetAngle)
		.addFunction("SetAngleOrigin", &Object::SetAngleOrigin)
		.addFunction("SetOrigin", &Object::SetOrigin)
		.addFunction("SetColor", &Object::SetColor)
		.endClass()
		.deriveClass<Primitive, Object>("Primitive")
		.addConstructor<void(*)(const std::string&)>()
		.addFunction("Create", &Primitive::Create)
		.addFunction("SetTextureOffset", &Primitive::SetTextureOffset)
		.addFunction("SetTextureScale", &Primitive::SetTextureScale)
		.addFunction("GetTextureOffset", &Primitive::GetTextureOffset)
		.addFunction("GetTextureScale", &Primitive::GetTextureScale)
		.addFunction("GetType", &Primitive::GetPrimitiveType)
		.addFunction("SetSize", &Primitive::SetSize)
		.endClass()
		.deriveClass<Entity, Object>("Entity")
		.addConstructor<void(*)(const std::string&, const EntityTypes&)>()
		.endClass()
		.deriveClass<GameCamera, Entity>("Camera")
		.addConstructor<void(*)(const Nt::String&)>()
		.addFunction("SetOrigin", &Entity::SetPosition)
		.addFunction("SetPosition", &Entity::SetPosition)
		.addFunction("SetAngle", &Entity::SetAngle)
		.endClass()
		.deriveClass<GameSound, Entity>("Sound")
		.addConstructor<void(*)(const Nt::String&)>()
		.addFunction("Play", &GameSound::Play)
		.addFunction("Stop", &GameSound::Stop)
		.addFunction<void (GameSound::*)(const Bool&)>("ToggleLooping", &GameSound::ToggleLooping)
		.addFunction("SetPosition", &GameSound::SetPosition)
		.endClass();

	GetGlobalNamespace()
		.beginClass<ObjectContainer>("ObjectContainer")
		.addConstructor<void(*)()>()
		.addFunction("Size", &ObjectContainer::size)
		.endClass();

	GetGlobalNamespace()
		.beginClass<Scence>("Scence")
		.addConstructor<void(*)()>()
		.addFunction("AddObject", &Scence::AddObject)
		.addFunction("RemoveObject", &Scence::RemoveObject)
		.addFunction("Clear", &Scence::Clear)
		.addFunction("AllowIntersectionOfLayers", &Scence::AllowIntersectionOfLayers)
		.addFunction("GetAllObjects", &Scence::GetObjects)
		.addFunction("GetObjectByName", &Scence::GetObjectPtrByName)
		.addFunction("GetObject", &Scence::GetObjectPtr)
		.addFunction("GetSoundByName", &Scence::GetSoundPtrByName)
		.addFunction("GetModelByName", &Scence::GetModelPtrByName)
		.addFunction("GetCameraByName", &Scence::GetCameraPtrByName)
		.endClass();

	GetGlobalNamespace()
		.beginClass<Nt::Timer>("Timer")
		.addConstructor<void(*)()>()
		.addFunction("GetElapsedTimeMs", &Nt::Timer::GetElapsedTimeMs)
		.addFunction("Restart", &Nt::Timer::Restart)
		.endClass();

	GetGlobalNamespace()
		.addFunction("Sleep", &Sleep);

	SetGlobal("g_Scence", m_ScencePtr);

	m_IsInitialized = true;
}