#pragma once

#include <ranges>
#include <TinyXML.h>
#include <Nt/Core/Math/Vector.h>
#include <Nt/Graphics/Ex/Components/Transform.h>
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
	static [[nodiscard]] TiXmlElement* ToXML(const NtEx::TransformFloat3D* pTransform);
	static [[nodiscard]] TiXmlElement* ToXML(const NtEx::RigidBody* pBody);

	static [[nodiscard]] TiXmlElement* ToXML(const Nt::Texture* pTexture);
	static [[nodiscard]] TiXmlElement* ToXML(const Nt::Mesh* pMesh);

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
	static void WriteVecAttribute(NotNull<TiXmlElement*> pElement, const Nt::String& name, const Nt::Vector<_Ty, size>& vector) noexcept {
		pElement->SetAttribute(name, VecToString(vector));
	}

	template <typename _Ty, uInt size>
	static [[nodiscard]] Nt::Vector<_Ty, size> ReadVecAttribute(NotNull<TiXmlElement*> pElement, const Nt::String& name) noexcept {
		std::string str;
		pElement->QueryStringAttribute(name, &str);
		return StringToVec<_Ty, size>(str);
	}

	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<NtEx::TransformFloat3D*> pTransform);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<NtEx::RigidBody*> pBody, NotNull<Object*> pObject);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Texture*> pTexture);
	static void FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Mesh*> pMesh);

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

	template <typename _Ty, uInt size>
	static std::string VecToString(const Nt::Vector<_Ty, size>& vector) {
		std::string str;
		for (_Ty scalar : vector.Array)
			str += std::format("{},", scalar);
		str.pop_back();
		return str;
	}
	template <typename _Ty, uInt size>
	static Nt::Vector<_Ty, size> StringToVec(const std::string& str) {
		Nt::Vector<_Ty, size> vec;
		uInt i = 0;

		for (auto&& part : str | std::views::split(',')) {
			if (i >= size)
				break;

			const std::string scalar(part.begin(), part.end());
			if constexpr (std::is_integral_v<_Ty>)
				vec[i] = std::stoi(scalar);
			else
				vec[i] = std::stof(scalar);

			++i;
		}

		return vec;
	}
};