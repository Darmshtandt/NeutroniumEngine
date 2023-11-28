#include <Main.h>
#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Primitive.h>
#include <Entity.h>

Nt::ISerialization* Object::New(const uInt& classType) {
	switch (ObjectTypes(classType)) {
	case ObjectTypes::PRIMITIVE:
		return new Primitive("");
	case ObjectTypes::ENTITY:
		return new Entity("", EntityTypes::NONE);
	}
	Raise("Non-existent object type specified");
	return nullptr;
}