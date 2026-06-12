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

	TiXmlElement* el = new TiXmlElement("Nt::IObject");
	el->SetAttribute("IsVisible", pIObject->IsVisible());
	el->LinkEndChild(ToXML("Position", pIObject->GetPosition()));
	el->LinkEndChild(ToXML("Origin", pIObject->GetOrigin()));
	el->LinkEndChild(ToXML("Angle", pIObject->GetAngle()));
	el->LinkEndChild(ToXML("AngleOrigin", pIObject->GetAngleOrigin()));
	el->LinkEndChild(ToXML("Size", pIObject->GetSize()));
	el->LinkEndChild(ToXML("Color", pIObject->GetColor()));
	return el;
}

TiXmlElement* SerializerXML::ToXML(const Nt::RigidBody* pBody) {
	if (pBody == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Nt::RigidBody");
	el->LinkEndChild(ToXML(static_cast<const Nt::IObject*>(pBody)));
	el->SetDoubleAttribute("Mass", pBody->GetMass());
	el->SetDoubleAttribute("Friction", pBody->GetFriction());
	el->SetDoubleAttribute("FrictionStatic", pBody->GetFrictionStatic());
	el->SetAttribute("Enabled", pBody->IsPhysicsEnabled());
	el->SetAttribute("EnabledCollision", pBody->IsEnabledCollision());
	el->SetAttribute("EnabledGravitation", pBody->IsEnabledGravitation());
	el->SetAttribute("IsActive", pBody->IsActive());
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
		xmlVertex->LinkEndChild(ToXML("Position", vert.Position));
		xmlVertex->LinkEndChild(ToXML("Color", vert.Color));
		xmlVertex->LinkEndChild(ToXML("TexCoord", vert.TexCoord));
		xmlVertex->LinkEndChild(ToXML("Normal", vert.Normal));
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

TiXmlElement* SerializerXML::ToXML(const Nt::Model* pModel) {
	if (pModel == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Nt::Model");
	el->LinkEndChild(ToXML(static_cast<const Nt::IObject*>(pModel)));

	const auto& meshHandle = pModel->GetMesh();
	if (meshHandle.IsValid() && (!meshHandle.Get()->GetFilePath().empty()))
		el->SetAttribute("MeshFilePath", meshHandle.Get()->GetFilePath().c_str());

	const auto& texHandle = pModel->GetTexture();
	if (texHandle.IsValid() && (!texHandle.Get()->GetFilePath().empty()))
		el->SetAttribute("TextureFilePath", texHandle.Get()->GetFilePath().c_str());
	return el;
}

TiXmlElement* SerializerXML::ToXML(const Object* pObject) {
	if (pObject == nullptr)
		return nullptr;

	TiXmlElement* el = new TiXmlElement("Object");
	el->LinkEndChild(ToXML(static_cast<const Nt::RigidBody*>(pObject)));
	el->LinkEndChild(ToXML(&pObject->GetModel()));
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
	el->LinkEndChild(ToXML(pPrimitive->GetMesh()));
	el->LinkEndChild(ToXML("TextureOffset", pPrimitive->GetTextureOffset()));
	el->LinkEndChild(ToXML("TextureScale", pPrimitive->GetTextureScale()));
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


TiXmlElement* SerializerXML::ToXML(const Scene* pScene) {
	if (pScene == nullptr)
		return nullptr;

	TiXmlElement* xmlScene = new TiXmlElement("Scene");

	const auto& objects = pScene->GetObjects();
	for (const Object* pObject : objects) {
		TiXmlElement* xmlObject;
		//if (Class<GameSound>::Is(*pObject))
		//	xmlObject = ToXML(reinterpret_cast<const GameSound*>(pObject));
		//else if (Class<GameModel>::Is(*pObject))
		//	xmlObject = ToXML(reinterpret_cast<const GameModel*>(pObject));
		//else if (Class<GameLight>::Is(*pObject))
		//	xmlObject = ToXML(reinterpret_cast<const GameLight*>(pObject));
		//else if (Class<GameCamera>::Is(*pObject))
		//	xmlObject = ToXML(reinterpret_cast<const GameCamera*>(pObject));

		if (pObject->GetToken() == Cube::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Cube*>(pObject));
		else if (pObject->GetToken() == Quad::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Quad*>(pObject));
		else if (pObject->GetToken() == Pyramid::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Pyramid*>(pObject));
		else if (pObject->GetToken() == Plane::GetClassToken())
			xmlObject = ToXML(reinterpret_cast<const Plane*>(pObject));
		else
			Raise("Unknown object");

		xmlScene->LinkEndChild(xmlObject);
	}

	return xmlScene;
}


void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::IObject*> pObject) {
	Assert(pElement->ValueStr() == "Nt::IObject", "Element not Nt::IObject");

	Bool isVisible;
	pElement->QueryBoolAttribute("IsVisible", &isVisible);

	if (isVisible)
		pObject->Show();
	else
		pObject->Hide();

	Nt::Float3D value3D;
	NotNull<TiXmlElement*> pSibling = pElement->FirstChildElement();
	FromXML<Float, 3>(pSibling, &value3D, "Position");
	pObject->SetPosition(value3D);

	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 3>(pSibling, &value3D, "Origin");
	pObject->SetOrigin(value3D);

	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 3>(pSibling, &value3D, "Angle");
	pObject->SetAngle(value3D);

	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 3>(pSibling, &value3D, "AngleOrigin");
	pObject->SetAngleOrigin(value3D);

	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 3>(pSibling, &value3D, "Size");
	pObject->SetSize(value3D);

	Nt::Float4D color;
	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 4>(pSibling, &color, "Color");
	pObject->SetColor(color);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::RigidBody*> pBody) {
	Assert(pElement->ValueStr() == "Nt::RigidBody", "Element not Nt::RigidBody");

	TiXmlElement* xmlObject = pElement->FirstChildElement();
	FromXML(xmlObject, static_cast<Nt::IObject*>(pBody));

	Float floatValue;
	pElement->QueryFloatAttribute("Mass", &floatValue);
	pBody->SetMass(floatValue);

	pElement->QueryFloatAttribute("Friction", &floatValue);
	pBody->SetFriction(floatValue);

	pElement->QueryFloatAttribute("FrictionStatic", &floatValue);
	pBody->SetFrictionStatic(floatValue);

	Bool boolValue;
	pElement->QueryBoolAttribute("Enabled", &boolValue);
	if (boolValue) 
		pBody->EnablePhysics();
	else 
		pBody->DisablePhysics();

	pElement->QueryBoolAttribute("EnabledCollision", &boolValue);
	if (boolValue)
		pBody->EnableCollider();
	else
		pBody->DisableCollider();

	pElement->QueryBoolAttribute("EnabledGravitation", &boolValue);
	if (boolValue)
		pBody->EnableGravitation();
	else
		pBody->DisableGravitation();

	pElement->QueryBoolAttribute("IsActive", &boolValue);
	if (boolValue)
		pBody->Activate();
	else
		pBody->Deactivate();
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
			Nt::Vertex vert;
			NotNull<TiXmlElement*> pParam = pSibling->FirstChildElement();
			FromXML<Float, 4>(pParam, &vert.Position, "Position");

			pParam = pParam->NextSiblingElement();
			FromXML<Float, 4>(pParam, &vert.Color, "Color");

			pParam = pParam->NextSiblingElement();
			FromXML<Float, 3>(pParam, &vert.TexCoord, "TexCoord");

			pParam = pParam->NextSiblingElement();
			FromXML<Float, 4>(pParam, &vert.Normal, "Normal");
			shape.Vertices.emplace_back(vert);
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

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Nt::Model*> pModel) {
	Assert(pElement->ValueStr() == "Nt::Model", "Element not Nt::Model");

	TiXmlElement* xmlObject = pElement->FirstChildElement();
	FromXML(xmlObject, static_cast<Nt::IObject*>(pModel));

	std::string filePath;
	if (pElement->QueryStringAttribute("MeshFilePath", &filePath) == TIXML_SUCCESS) {
		assert(0);
	}

	if (pElement->QueryStringAttribute("TextureFilePath", &filePath) == TIXML_SUCCESS) {
		const uInt index = Nt::ResourceManager::Instance().Add<Nt::Texture>(filePath);
		Assert(std::cmp_not_equal(index ,-1), "Failed to load texture: " + filePath);

		pModel->SetTexture(index);
	}
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

	TiXmlElement* pSibling = RequireNotNull(pElement->FirstChildElement());
	FromXML(pSibling, static_cast<NotNull<Nt::RigidBody*>>(pObject));

	Nt::Model model;

	pSibling = pSibling->NextSiblingElement();
	FromXML(pSibling, NotNull<Nt::Model*>(&model));

	pObject->SetModel(model);

	std::string name;
	pElement->QueryStringAttribute("LayerName", &name);
	pObject->SetLayerName(name);
	pElement->QueryStringAttribute("Name", &name);
	pObject->SetName(name);

	Bool isInvisible;
	pElement->QueryBoolAttribute("IsInvisible", &isInvisible);

	if (isInvisible)
		pObject->EnableInvisible();
	else
		pObject->DisableInvisible();

	pSibling = pSibling->NextSiblingElement();
	if (pSibling == nullptr)
		return;

	std::string scriptFilePath;
	std::vector<Script::Data> scriptData;
	FromXML(pSibling, scriptFilePath, scriptData);

	pObject->AttachScript(pLua, scriptFilePath, scriptData);
}

void SerializerXML::FromXML(NotNull<TiXmlElement*> pElement, NotNull<Primitive*> pPrimitive, NotNull<Lua*> pLua) {
	Assert(pElement->ValueStr() == "Primitive", "Element not Primitive");

	NotNull<TiXmlElement*> pSibling = pElement->FirstChildElement();
	FromXML(pSibling, NotNull<Object*>(pPrimitive), pLua);

	Nt::Mesh mesh;
	pSibling = pSibling->NextSiblingElement();
	FromXML(pSibling, &mesh);
	pPrimitive->SetPrimitiveMesh(mesh);

	Nt::Float2D value2D;
	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 2>(pSibling, &value2D, "TextureOffset");
	pPrimitive->SetTextureOffset(value2D);

	pSibling = pSibling->NextSiblingElement();
	FromXML<Float, 2>(pSibling, &value2D, "TextureScale");
	pPrimitive->SetTextureScale(value2D);
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