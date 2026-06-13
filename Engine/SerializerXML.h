#pragma once

#include <TinyXML.h>
#include <Nt/Core/Math/Vector.h>
#include <Script/Script.h>

class Plane;
class Pyramid;
class Cube;
class Quad;

class GameCamera;
class GameLight;
class GameModel;
class GameSound;

namespace Nt {
	struct Vertex;
}

class Primitive;

namespace Nt {
	class Mesh;
	class Texture;
	class Model;
	class RigidBody;
	class IObject;
}

struct SerializerXML final {
	static TiXmlElement* ToXML(const Nt::IObject* pIObject);
	static TiXmlElement* ToXML(const Nt::RigidBody* pBody);

	static TiXmlElement* ToXML(const Nt::Texture* pTexture);
	static TiXmlElement* ToXML(const Nt::Mesh* pMesh);
	static TiXmlElement* ToXML(const Nt::Model* pModel);

	static TiXmlElement* ToXML(const Script* pScript, const std::vector<Script::Data>& allData);
	static TiXmlElement* ToXML(const Object* pObject);
	static TiXmlElement* ToXML(const Primitive* pPrimitive);
	static TiXmlElement* ToXML(const Entity* pEntity);

	static TiXmlElement* ToXML(const GameSound* pSound);
	static TiXmlElement* ToXML(const GameModel* pModel);
	static TiXmlElement* ToXML(const GameLight* pLight);
	static TiXmlElement* ToXML(const GameCamera* pCamera);

	static TiXmlElement* ToXML(const Cube* pCube);
	static TiXmlElement* ToXML(const Quad* pQuad);
	static TiXmlElement* ToXML(const Pyramid* pPyramid);
	static TiXmlElement* ToXML(const Plane* pPlane);

	static TiXmlElement* ToXML(const Scene* pScene);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Scene*> pScene, NotNull<Lua*> pLua);

	template <typename _Ty, uInt size>
	_NODISCARD static TiXmlElement* ToXML(const Nt::String& name, const Nt::Vector<_Ty, size>& vector) noexcept {
		TiXmlElement* element = new TiXmlElement("Vector");
		element->SetAttribute("Dimension", size);
		element->SetAttribute("Name", name.c_str());

		for (uInt i = 0; i < size; ++i) {
			Char literal[2] = { '\0' };
			if (i < 3)
				literal[0] = static_cast<Char>(static_cast<uInt>('x') + i);
			else
				literal[0] = static_cast<Char>(static_cast<uInt>('z') - i);

			if constexpr (std::is_floating_point_v<_Ty>)
				element->SetDoubleAttribute(literal, vector.Array[i]);
			else
				element->SetAttribute(literal, vector.Array[i]);
		}

		return element;
	}

	template <typename _Ty, uInt size>
	_NODISCARD static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Vector<_Ty, size>*> pVector, const std::string& requiredName) noexcept {
		Assert(pElement->ValueStr() == "Vector", "Element not Vector");

		uInt dimension;
		pElement->QueryUnsignedAttribute("Dimension", &dimension);
		Assert(dimension == size, "Vector has other dimension");

		std::string name;
		pElement->QueryStringAttribute("Name", &name);
		Assert(name == requiredName, "Name inconsistency");		

		for (uInt i = 0; i < size; ++i) {
			Char literal[2] = { '\0' };
			if (i < 3)
				literal[0] = static_cast<Char>(static_cast<uInt>('x') + i);
			else
				literal[0] = static_cast<Char>(static_cast<uInt>('z') - i);

			pElement->QueryValueAttribute<_Ty>(literal, &pVector->Array[i]);
			//if constexpr (std::is_floating_point_v<_Ty>) {
			//	Double value;
			//	pElement->QueryDoubleAttribute(literal, &value);
			//	pVector->Array[i] = static_cast<_Ty>(value);
			//}
			//else {
			//	Int value;
			//	pElement->QueryIntAttribute(literal, &value);
			//	pVector->Array[i] = static_cast<_Ty>(value);
			//}
		}
	}

	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::IObject*> pObject);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::RigidBody*> pBody);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Texture*> pTexture);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Mesh*> pMesh);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Model*> pModel);

	static void FromXML(NotNull<TiXmlElement*> pElement, std::string& filePath, std::vector<Script::Data>& allData);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Object*> pObject, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Primitive*> pPrimitive, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Entity*> pEntity, NotNull<Lua*> pLua);

	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameSound*> pSound, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameModel*> pModel, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameLight*> pLight, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameCamera*> pCamera, NotNull<Lua*> pLua);

	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Cube*> pCube, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Quad*> pQuad, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Pyramid*> pPyramid, NotNull<Lua*> pLua);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Plane*> pPlane, NotNull<Lua*> pLua);
};