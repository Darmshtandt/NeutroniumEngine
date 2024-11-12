#include <Main.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Primitive.h>
#include <Entity.h>

#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>


Nt::ISerialization* Entity::New(const uInt& classType) {
	switch (EntityTypes(classType)) {
	case EntityTypes::CAMERA:
		return new GameCamera("");
	case EntityTypes::MODEL:
		return new GameModel("");
	case EntityTypes::SOUND:
		return new GameSound("");
	}

	Raise("Non-existent entity type specified");
	return nullptr;
}