#pragma once

#include <TinyXML.h>
#include <Nt/Core/Math/Vector.h>
#include <Script/Script.h>

namespace NtEx {
	class RigidBody;
}

class Plane;
class Pyramid;
class Cube;
class Quad;

class GameCamera;
class GameLight;
class GameModel;
class GameSound;

class Primitive;

namespace Nt {
	struct Vertex;

	class Mesh;
	class Texture;
	class Model;
	class IObject;
}

struct SerializerXML {
	static [[nodiscard]] TiXmlElement* ToXML(const Nt::IObject* pIObject);
	static [[nodiscard]] TiXmlElement* ToXML(const NtEx::RigidBody* pBody);

	static [[nodiscard]] TiXmlElement* ToXML(const Nt::Texture* pTexture);
	static [[nodiscard]] TiXmlElement* ToXML(const Nt::Mesh* pMesh);
	static [[nodiscard]] TiXmlElement* ToXML(const Nt::Model* pModel);

	static [[nodiscard]] TiXmlElement* ToXML(const Script* pScript, const std::vector<Script::Data>& allData);
	static [[nodiscard]] TiXmlElement* ToXML(const Object* pObject);
	static [[nodiscard]] TiXmlElement* ToXML(const Primitive* pPrimitive);
	static [[nodiscard]] TiXmlElement* ToXML(const Entity* pEntity);

	static [[nodiscard]] TiXmlElement* ToXML(const GameSound* pSound);
	static [[nodiscard]] TiXmlElement* ToXML(const GameModel* pModel);
	static [[nodiscard]] TiXmlElement* ToXML(const GameLight* pLight);
	static [[nodiscard]] TiXmlElement* ToXML(const GameCamera* pCamera);

	static [[nodiscard]] TiXmlElement* ToXML(const Cube* pCube);
	static [[nodiscard]] TiXmlElement* ToXML(const Quad* pQuad);
	static [[nodiscard]] TiXmlElement* ToXML(const Pyramid* pPyramid);
	static [[nodiscard]] TiXmlElement* ToXML(const Plane* pPlane);

	static [[nodiscard]] TiXmlElement* ToXML(const Scene* pScene);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Scene*> pScene, NotNull<Lua*> pLua);

	template <typename _Ty, uInt size>
	static [[nodiscard]] TiXmlElement* ToXML(const Nt::String& name, const Nt::Vector<_Ty, size>& vector) noexcept {
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
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Vector<_Ty, size>*> pVector, const std::string& requiredName) noexcept {
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
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<NtEx::RigidBody*> pBody, NotNull<Object*> pObject);
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