#include <SerializerXML.h>

#include <ResourceManager.h>
#include <Scene.h>
#include <Objects/Object.h>
#include <Objects/Primitives/Primitive.h>

#include <Objects/Primitives/Cube.h>
#include <Objects/Primitives/Plane.h>
#include <Objects/Primitives/Pyramid.h>
#include <Objects/Primitives/Quad.h>

#include <Objects/Entities/GameCamera.h>
#include <Objects/Entities/GameSound.h>
#include <Objects/Entities/GameModel.h>
#include <Objects/Entities/GameLight.h>

TiXmlElement* SerializerXML::ToXML(const Nt::IObject* pIObject) {
	if (pIObject == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Transform");
	WriteVecAttribute(el, "Position", pIObject->GetPosition());
	WriteVecAttribute(el, "Angle", pIObject->GetAngle());
	WriteVecAttribute(el, "Size", pIObject->GetSize());
	return el;
}

TiXmlElement* SerializerXML::ToXML(const NtEx::RigidBody* pBody) {
	if (pBody == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("RigidBody");
	el->SetDoubleAttribute("Mass", pBody->Body.GetMass());
	el->SetDoubleAttribute("Restitution", pBody->Body.GetRestitution());
	el->SetDoubleAttribute("LinearDamping", pBody->Body.GetLinearDamping());
	WriteVecAttribute(el, "LinearVelocity", pBody->Body.GetLinearVelocity());
	WriteVecAttribute(el, "Force", pBody->Body.GetForce());
	return el;
}

TiXmlElement* SerializerXML::ToXML(const Nt::Texture* pTexture) {
	if (pTexture == nullptr)
		return nullptr;

	TiXmlElement* xmlTexture = new TiXmlElement("Nt::Texture");
	if (!pTexture->GetFilePath().empty()) {
		xmlTexture->SetAttribute("FilePath", pTexture->GetFilePath().c_str());
		return xmlTexture;
	}

	assert(0);

	return xmlTexture;
}

TiXmlElement* SerializerXML::ToXML(const Script* pScript, const std::vector<Script::Data>& allData) {
	if (pScript == nullptr)
		return nullptr;

	TiXmlElement* elScript = new TiXmlElement("Script");
	elScript->SetAttribute("FilePath", pScript->GetFilePath().c_str());

	for (const Script::Data& data : allData) {
		TiXmlElement* elData = new TiXmlElement("Data");
		elData->SetAttribute("Type", data.Type);
		elData->SetAttribute("FieldName", data.FieldName.c_str());
		elData->SetAttribute("Name", data.Name.c_str());
		elData->SetAttribute("Value", data.Value.c_str());
		elScript->LinkEndChild(elData);
	}

	return elScript;
}

TiXmlElement* SerializerXML::ToXML(const Nt::Mesh* pMesh) {
	if (pMesh == nullptr)
		return nullptr;

	TiXmlElement* xmlMesh = new TiXmlElement("Nt::Mesh");
	if (!pMesh->GetFilePath().empty()) {
		xmlMesh->SetAttribute("FilePath", pMesh->GetFilePath().c_str());
		return xmlMesh;
	}

	for (const Nt::Vertex& vert : pMesh->GetVertices()) {
		TiXmlElement* xmlVertex = new TiXmlElement("Nt::Vertex");
		WriteVecAttribute(xmlVertex, "Position", vert.Position.xyz);
		WriteVecAttribute(xmlVertex, "Color", vert.Color);
		WriteVecAttribute(xmlVertex, "TexCoord", vert.TexCoord);
		WriteVecAttribute(xmlVertex, "Normal", vert.Normal.xyz);
		xmlMesh->LinkEndChild(xmlVertex);
	}

	const auto& indices = pMesh->GetIndices();
	for (uInt i = 2; i < indices.size(); i += 3) {
		TiXmlElement* xmlFace = new TiXmlElement("Triangle");
		xmlFace->SetAttribute("a", indices[i - 2]);
		xmlFace->SetAttribute("b", indices[i - 1]);
		xmlFace->SetAttribute("c", indices[i]);
		xmlMesh->LinkEndChild(xmlFace);
	}
	return xmlMesh;
}

TiXmlElement* SerializerXML::ToXML(const Object* pObject) {
	if (pObject == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Object");
	el->SetAttribute("IsVisible", pObject->IsVisible());
	el->SetAttribute("EnabledCollision", pObject->EnabledCollider());
	el->SetAttribute("EnabledGravitation", pObject->EnabledGravitation());
	WriteVecAttribute(el, "Color", pObject->GetColor());
	el->LinkEndChild(ToXML(static_cast<const Nt::IObject*>(pObject)));
	el->LinkEndChild(ToXML(pObject->GetRigidBody()));

	auto meshHandler = pObject->GetMesh();
	if (meshHandler.IsValid()) {
		Nt::Mesh* mesh = meshHandler.Get();
		if (!mesh->GetFilePath().empty())
			el->LinkEndChild(ToXML(mesh));
	}

	if (pObject->GetTexture().IsValid())
		el->LinkEndChild(ToXML(pObject->GetTexture().Get()));

	el->SetAttribute("LayerName", pObject->GetLayerName().c_str());
	el->SetAttribute("Name", pObject->GetName().c_str());
	el->SetAttribute("IsInvisible", pObject->IsInvisible());

	const Script* pScript = pObject->GetScript();
	if (pScript != nullptr)
		el->LinkEndChild(ToXML(pScript, pObject->GetScriptData()));
	return el;
}

TiXmlElement* SerializerXML::ToXML(const Primitive* pPrimitive) {
	if (pPrimitive == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Primitive");
	el->LinkEndChild(ToXML(static_cast<const Object*>(pPrimitive)));
	el->LinkEndChild(ToXML(pPrimitive->GetMesh().Get()));
	WriteVecAttribute(el, "TextureOffset", pPrimitive->GetTextureOffset());
	WriteVecAttribute(el, "TextureScale", pPrimitive->GetTextureScale());
	return el;
}

TiXmlElement* SerializerXML::ToXML(const Entity* pEntity) {
	if (pEntity == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Entity");
	el->LinkEndChild(ToXML(static_cast<const Object*>(pEntity)));
	return el;
}

template <class T>
TiXmlElement* PrimitiveToXML(const T* pObj, const std::string& elementName) {
	if (pObj == nullptr)
		return nullptr;

	const Primitive* pPrimitive = static_cast<const Primitive*>(pObj);

	TiXmlElement* el = new TiXmlElement(elementName);
	el->LinkEndChild(SerializerXML::ToXML(pPrimitive));
	return el;
}


TiXmlElement* SerializerXML::ToXML(const Cube* pCube) {
	return PrimitiveToXML(pCube, "Cube");
}
TiXmlElement* SerializerXML::ToXML(const Quad* pQuad) {
	return PrimitiveToXML(pQuad, "Quad");
}
TiXmlElement* SerializerXML::ToXML(const Pyramid* pPyramid) {
	return PrimitiveToXML(pPyramid, "Pyramid");
}
TiXmlElement* SerializerXML::ToXML(const Plane* pPlane) {
	return PrimitiveToXML(pPlane, "Plane");
}

template <class T>
TiXmlElement* EntityToXML(const T* pObj, const std::string& elementName) {
	if (pObj == nullptr)
		return nullptr;

	const Entity* pEntity = static_cast<const Entity*>(pObj);

	TiXmlElement* el = new TiXmlElement(elementName);
	el->LinkEndChild(SerializerXML::ToXML(pEntity));
	return el;
}

TiXmlElement* SerializerXML::ToXML(const GameSound* pSound) {
	assert(0);
	return EntityToXML(pSound, "GameSound");
}
TiXmlElement* SerializerXML::ToXML(const GameModel* pModel) {
	TiXmlElement* el = EntityToXML(pModel, "GameModel");
	el->LinkEndChild(SerializerXML::ToXML(pModel->GetMesh().Get()));
	return el;
}
TiXmlElement* SerializerXML::ToXML(const GameLight* pLight) {
	assert(0);
	return EntityToXML(pLight, "GameLight");
}
TiXmlElement* SerializerXML::ToXML(const GameCamera* pCamera) {
	return EntityToXML(pCamera, "GameCamera");
}


TiXmlElement* SerializerXML::ToXML(const Scene* pScene) {
	if (pScene == nullptr)
		return nullptr;

	TiXmlElement* xmlScene = new TiXmlElement("Scene");

	const auto& objects = pScene->GetObjects();
	for (const ObjectPtr& object : objects) {
		const Object* pObject = object.get();
		const std::string token = pObject->GetToken();

		TiXmlElement* xmlObject;
		if (token == GameSound::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const GameSound*>(pObject));
		else if (token == GameModel::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const GameModel*>(pObject));
		else if (token == GameLight::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const GameLight*>(pObject));
		else if (token == GameCamera::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const GameCamera*>(pObject));

		else if (token == Cube::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Cube*>(pObject));
		else if (token == Quad::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Quad*>(pObject));
		else if (token == Pyramid::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Pyramid*>(pObject));
		else if (token == Plane::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Plane*>(pObject));
		else
			Raise("Unknown object");

		xmlScene->LinkEndChild(xmlObject);
	}

	return xmlScene;
}

template <class T>
void SceneObjectFromXML(NotNull<TiXmlElement*> pElement, NotNull<Scene*> pScene, NotNull<Lua*> pLua) {
	T* pObject = new T("");
	SerializerXML::FromXML(pElement, NotNull<T*>(pObject), pLua);
	pScene->AddObject(pObject);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Scene*> pScene, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Scene", "Element not Scene");

	pScene->Clear();
	for (TiXmlElement* xmlObject = pElement->FirstChildElement();
		xmlObject;
		xmlObject = xmlObject->NextSiblingElement()) 
	{
		const std::string name = xmlObject->ValueStr();
		if (name == "GameSound")
			SceneObjectFromXML<GameSound>(xmlObject, pScene, pLua);
		else if (name == "GameModel")
			SceneObjectFromXML<GameModel>(xmlObject, pScene, pLua);
		else if (name == "GameLight")
			SceneObjectFromXML<GameLight>(xmlObject, pScene, pLua);
		else if (name == "GameCamera")
			SceneObjectFromXML<GameCamera>(xmlObject, pScene, pLua);

		else if (name == "Cube")
			SceneObjectFromXML<Cube>(xmlObject, pScene, pLua);
		else if (name == "Quad")
			SceneObjectFromXML<Quad>(xmlObject, pScene, pLua);
		else if (name == "Pyramid")
			SceneObjectFromXML<Pyramid>(xmlObject, pScene, pLua);
		else if (name == "Plane")
			SceneObjectFromXML<Plane>(xmlObject, pScene, pLua);
		else
			Raise("Unknown element");
	}
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::IObject*> pObject) {
	Assert(pElement->ValueStr() == "Transform", "Element not Transform");

	pObject->SetPosition(ReadVecAttribute<Float, 3>(pElement, "Position"));
	pObject->SetAngle(ReadVecAttribute<Float, 3>(pElement, "Angle"));
	pObject->SetSize(ReadVecAttribute<Float, 3>(pElement, "Size"));
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<NtEx::RigidBody*> pBody, NotNull<Object*> pObject) {
	Assert(pElement->ValueStr() == "RigidBody", "Element not RigidBody");

	Float floatValue;
	pElement->QueryFloatAttribute("Mass", &floatValue);
	pBody->Body.SetMass(floatValue);

	Bool boolValue;
	pElement->QueryBoolAttribute("EnabledCollision", &boolValue);
	pObject->ToggleCollider(boolValue);

	pElement->QueryBoolAttribute("EnabledGravitation", &boolValue);
	pObject->ToggleGravitation(boolValue);

	pBody->Body.SetLinearVelocity(ReadVecAttribute<Float, 3>(pElement, "LinearVelocity"));
	pBody->Body.SetForce(ReadVecAttribute<Float, 3>(pElement, "Force"));
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Texture*> pTexture) {
	Assert(pElement->ValueStr() == "Nt::Texture", "Element not Nt::Texture");

	std::string filePath;
	if (pElement->QueryStringAttribute("FilePath", &filePath) == TIXML_SUCCESS) {
		pTexture->LoadFromFile(filePath);
		return;
	}

	assert(0);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Mesh*> pMesh) {
	Assert(pElement->ValueStr() == "Nt::Mesh", "Element not Nt::Mesh");

	std::string filePath;
	if (pElement->QueryStringAttribute("FilePath", &filePath) == TIXML_SUCCESS) {
		pMesh->LoadFromFile(filePath);
		return;
	}

	Nt::Shape shape;
	for (TiXmlElement* pSibling = pElement->FirstChildElement();
		pSibling;
		pSibling = pSibling->NextSiblingElement()) 
	{
		std::string tagName = pSibling->ValueStr();
		if (tagName == "Nt::Vertex") {
			shape.Vertices.emplace_back(
				ReadVecAttribute<Float, 3>(pSibling, "Position"),
				ReadVecAttribute<Float, 3>(pSibling, "Normal"),
				ReadVecAttribute<Float, 3>(pSibling, "TexCoord"),
				ReadVecAttribute<Float, 4>(pSibling, "Color"));
		}
		else if (tagName == "Triangle") {
			Int face[3];
			pSibling->QueryIntAttribute("a", &face[0]);
			pSibling->QueryIntAttribute("b", &face[1]);
			pSibling->QueryIntAttribute("c", &face[2]);

			shape.Indices.insert(shape.Indices.end(), std::begin(face), std::end(face));
		}
		else {
			Raise("Unknown element");
		}
	}

	pMesh->SetShape(shape);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, std::string& filePath, std::vector<Script::Data>& allData) {
	Assert(pElement->ValueStr() == "Script", "Element not Script");

	Assert(pElement->QueryStringAttribute("FilePath", &filePath) == TIXML_SUCCESS,
		"Failed to get Script file path");

	for (TiXmlElement* pSibling = pElement->FirstChildElement();
		pSibling;
		pSibling = pSibling->NextSiblingElement()) 
	{
		Assert(pSibling->ValueStr() == "Data", "Element not Data");

		Int type;
		pSibling->QueryIntAttribute("Type", &type);

		std::string fieldName;
		pSibling->QueryStringAttribute("FieldName", &fieldName);

		std::string name;
		pSibling->QueryStringAttribute("Name", &name);

		std::string value;
		pSibling->QueryStringAttribute("Value", &value);

		allData.emplace_back((Script::Data::Types)type, fieldName, name, value);
	}
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Object*> pObject, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Object", "Element not Object");

	Bool enabled;
	pElement->QueryBoolAttribute("IsVisible", &enabled);
	pObject->ToggleVisible(enabled);

	pElement->QueryBoolAttribute("IsInvisible", &enabled);
	pObject->ToggleInvisible(enabled);

	std::string name;
	pElement->QueryStringAttribute("LayerName", &name);
	pObject->SetLayerName(name);
	pElement->QueryStringAttribute("Name", &name);
	pObject->SetName(name);
	pObject->SetColor(ReadVecAttribute<Float, 4>(pElement, "Color"));

	for (TiXmlElement* pSibling = pElement->FirstChildElement();
		pSibling;
		pSibling = pSibling->NextSiblingElement()) 
	{
		const std::string name = pSibling->ValueStr();
		if (name == "Nt::Mesh") {
			Nt::Log::Instance().Warning("THE MESH DOES NOT HAVE A LOADER: SerializerXML::FromXML(TiXmlElement, Object, Lua)");
		}
		else if (name == "Nt::Texture") {
			Nt::Texture* pTexture = new Nt::Texture;
			FromXML(pSibling, pTexture);

			const std::string filePath = pTexture->GetFilePath();
			if (filePath.empty()) {
				delete(pTexture);
				continue;
			}

			const uInt index =
				ResourceManager::Instance().Add(filePath, std::unique_ptr<Nt::Texture>(pTexture));
			pObject->SetTexture(index);
		}
		else if (name == "Script") {
			std::string filePath;
			std::vector<Script::Data> datas;
			FromXML(pSibling, filePath, datas);
			if (filePath.empty())
				continue;

			pObject->AttachScript(pLua, filePath, datas);
		}
		else if (name == "Transform") {
			FromXML(pSibling, static_cast<Nt::IObject*>(pObject));
		}
		else if (name == "RigidBody") {
			FromXML(pSibling, pObject->GetRigidBody(), pObject);
		}
	}
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Primitive*> pPrimitive, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Primitive", "Element not Primitive");

	NotNull<TiXmlElement*> pSibling = pElement->FirstChildElement();
	FromXML(pSibling, NotNull<Object*>(pPrimitive), pLua);

	Nt::Mesh mesh;
	pSibling = pSibling->NextSiblingElement();
	FromXML(pSibling, &mesh);
	pPrimitive->SetPrimitiveMesh(mesh);

	pPrimitive->SetTextureOffset(ReadVecAttribute<Float, 2>(pElement, "TextureOffset"));
	pPrimitive->SetTextureScale(ReadVecAttribute<Float, 2>(pElement, "TextureScale"));
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Entity*> pEntity, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Entity", "Element not Entity");

	NotNull<TiXmlElement*> pSibling = pElement->FirstChildElement();
	FromXML(pSibling, NotNull<Object*>(pEntity), pLua);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameSound*> pSound, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "GameSound", "Element not GameSound");
	FromXML(pElement->FirstChildElement(), NotNull<Entity*>(pSound), pLua);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameModel*> pModel, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "GameModel", "Element not GameModel");

	TiXmlElement* pSibling = pElement->FirstChildElement();
	FromXML(pSibling, NotNull<Entity*>(pModel), pLua);

	auto mesh = new Nt::Mesh;
	FromXML(pSibling->NextSiblingElement(), mesh);
	pModel->SetMesh(mesh);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameLight*> pLight, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "GameLight", "Element not GameLight");
	FromXML(pElement->FirstChildElement(), NotNull<Entity*>(pLight), pLua);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<GameCamera*> pCamera, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "GameCamera", "Element not GameCamera");
	FromXML(pElement->FirstChildElement(), NotNull<Entity*>(pCamera), pLua);

	pCamera->SetPosition(pCamera->GetPosition());
	pCamera->SetOrigin(pCamera->GetOrigin());
	pCamera->SetAngle(pCamera->GetAngle());
	pCamera->SetAngleOrigin(pCamera->GetAngleOrigin());
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Cube*> pCube, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Cube", "Element not Cube");
	FromXML(pElement->FirstChildElement(), NotNull<Primitive*>(pCube), pLua);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Quad*> pQuad, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Quad", "Element not Quad");
	FromXML(pElement->FirstChildElement(), NotNull<Primitive*>(pQuad), pLua);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Pyramid*> pPyramid, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Pyramid", "Element not Pyramid");
	FromXML(pElement->FirstChildElement(), NotNull<Primitive*>(pPyramid), pLua);
}
void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Plane*> pPlane, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Plane", "Element not Plane");
	FromXML(pElement->FirstChildElement(), NotNull<Primitive*>(pPlane), pLua);
}