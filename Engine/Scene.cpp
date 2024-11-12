#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Entity.h>
#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>
#include <Selector.h>
#include <ObjectsTree.h>
#include <Scene.h>


Scene::Scene() {
	_Initialize();
}
Scene::Scene(const Scene& scence) {
	_Initialize();

	for (const Object* pObject : scence.m_Objects) {
		Object* pCopiedObject = pObject->GetCopy();
		if (pCopiedObject == nullptr)
			Raise("Failed to crete object.");

		pCopiedObject->SetForce({ });

		const Script* pScript = pObject->GetScript();
		if (pScript != nullptr)
			pCopiedObject->AttachScript(m_pLua, this, pScript->GetFilePath(), pScript->GetScriptData());

		m_Objects.push_back(pCopiedObject);
	}
}
Scene::~Scene() {
	Clear();
	ClearBuffer();
}

void Scene::Start() {
	for (Object* pObject : m_Objects)
		pObject->Start();
}
void Scene::Stop() {
	for (Object* pObject : m_Objects)
		pObject->Stop();
}

Object* Scene::RayCastObject(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint) {
	Float shortestDistance = -FLT_MAX;
	Object* pNearestObject = nullptr;

	for (uInt i = 0; i < m_Objects.size(); ++i) {
		Object* pObject = m_Objects[i];
		if (pObject == nullptr)
			Raise("Object pointer is null.");

		const Float objectDistance = (ray.Start - pObject->GetPosition()).LengthSquare();
		const Int faceIndex = pObject->RayCastTest(ray, pResultIntersectionPoint);

		if (faceIndex != -1 && shortestDistance < objectDistance) {
			shortestDistance = objectDistance;
			pNearestObject = pObject;
		}
	}
	return pNearestObject;
}

void Scene::BindObjectsTree(ObjectsTree* objectsTreePtr) noexcept {
	m_ObjectsTreePtr = objectsTreePtr;

	if (m_ObjectsTreePtr != nullptr) {
		m_ObjectsTreePtr->Clear();

		for (uInt i = 0; i < m_Objects.size(); ++i)
			m_ObjectsTreePtr->Add(m_Objects[i]);
	}
}

void Scene::AddObject(Object* pObject) {
	if (pObject == nullptr) {
		Raise("Object pointer is null.");
		return;
	}

	if (m_ObjectsTreePtr != nullptr)
		m_ObjectsTreePtr->Add(pObject);

	m_Objects.push_back(pObject);
}
void Scene::RemoveObject(const Object* pObject) {
	const auto iterator = std::find(m_Objects.begin(), m_Objects.end(), pObject);

	if (iterator != m_Objects.end()) {
		delete(*iterator);
		m_Objects.erase(iterator);
	}
	else {
		ERROR_MSG(L"Scence::RemoveObject: This object is not finded.", L"Error");
	}
}
void Scene::RemoveSelected(Selector* pSelector) {
	for (Object* pObject : pSelector->GetObjectContaiter())
		RemoveObject(pObject);

	pSelector->AllDeselect();

	if (m_ObjectsTreePtr != nullptr) {
		m_ObjectsTreePtr->Clear();

		for (Object* pObject : m_Objects)
			m_ObjectsTreePtr->Add(pObject);
	}
}

void Scene::Clear() {
	for (Object* pObject : m_Objects)
		delete(pObject);

	m_Objects.clear();

	if (m_ObjectsTreePtr != nullptr)
		m_ObjectsTreePtr->Clear();
}

void Scene::Update(const Float& time) {
	const auto findLayerByName = [&](LayerContainer& container, const Nt::String& layerName) {
		return std::find_if(container.begin(), container.end(), [&](const Layer& layer) {
			return (layer.Name == layerName);
		});
	};

	const auto handleCollisionIfAllowed = [&](Object* pPrimaryObject, Object* pSecondaryObject) {
		const auto layerIterator = findLayerByName(m_DisjointLayers, pPrimaryObject->GetLayerName());
		if (layerIterator != m_DisjointLayers.end()) {
			if (findLayerByName(layerIterator->Layers, pSecondaryObject->GetLayerName()) != layerIterator->Layers.end())
				return;
		}

		pPrimaryObject->Collision(pSecondaryObject);
	};

	for (Object* pObject : m_Objects) {
		pObject->Update(time);
		if ((!pObject->IsActivePhycisc()) || (!pObject->IsEnabledCollision()))
			continue;

		for (Object* pOtherObject : m_Objects) {
			if (pOtherObject == pObject)
				continue;

			if ((!pOtherObject->IsActivePhycisc()) || (!pOtherObject->IsEnabledCollision()))
				continue;

			if (pObject->IsChanged())
				handleCollisionIfAllowed(pObject, pOtherObject);
			else if (pOtherObject->IsChanged())
				handleCollisionIfAllowed(pOtherObject, pObject);
		}

		pObject->UnmarkChanged();
	}
}
void Scene::Render(Nt::Renderer* pRenderer) const {
	for (Object* pObject : m_Objects)
		pObject->Render(pRenderer);
}

void Scene::AllowLayerOverlap(const Nt::String& firstLayerName, const Nt::String& secondLayerName, const Bool& isAllow) {
	if (firstLayerName == secondLayerName) {
		Nt::Log::Warning("Layer names are the same");
		return;
	}

	auto disjointLayerIterator = std::find_if(m_DisjointLayers.begin(), m_DisjointLayers.end(), [&](const Layer& layer) {
		return (layer.Name == firstLayerName || layer.Name == secondLayerName);
	});

	const Bool isFoundedLayer = (disjointLayerIterator != m_DisjointLayers.end());
	if (isAllow) {
		if (isFoundedLayer)
			m_DisjointLayers.erase(disjointLayerIterator);

		return;
	}

	if (isFoundedLayer) {
		const std::string overlappingLayerName =
			(disjointLayerIterator->Name == firstLayerName) ? secondLayerName : firstLayerName;

		auto overlappingLayerIterator = std::find_if(m_DisjointLayers.begin(), m_DisjointLayers.end(), [&] (const Layer& layer) {
			return (layer.Name == overlappingLayerName);
		});

		if (overlappingLayerIterator == m_DisjointLayers.end())
			disjointLayerIterator->Layers.push_back({ LayerContainer(), overlappingLayerName});
	}
	else {
		Layer layer;
		layer.Name = firstLayerName;
		layer.Layers.push_back({ LayerContainer(), secondLayerName });

		m_DisjointLayers.push_back(layer);
	}
}

Bool Scene::Load(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open()) {
		ErrorBox(L"Failed to open file", L"Error");
		return false;
	}

	try {
		Clear();
		file.read((Char*)&m_Version, sizeof(m_Version));

		uInt objectCount;
		file.read((Char*)&objectCount, sizeof(uInt));

		for (uInt i = 0; i < objectCount; ++i) {
			ObjectTypes objectType;
			file.read((Char*)&objectType, sizeof(ObjectTypes));

			Object* pObject = nullptr;
			if (objectType == ObjectTypes::ENTITY) {
				EntityTypes entityType;
				file.read((Char*)&entityType, sizeof(entityType));

				pObject = dynamic_cast<Object*>(Entity::New(uInt(entityType)));
			}
			else {
				pObject = dynamic_cast<Object*>(Object::New(uInt(objectType)));
			}

			if (pObject == nullptr) {
				Raise("Failed to load scence");
				return false;
			}

			pObject->Read(file);

			uInt pathLength;
			file.read((Char*)&pathLength, sizeof(uInt));

			if (pathLength > 0) {
				std::string scriptFilePath(pathLength, '\0');
				file.read(scriptFilePath.data(), pathLength);

				if (scriptFilePath != "") {
					std::vector<Script::Data> data;
					Nt::Serialization::ReadAll(file, data);

					pObject->AttachScript(m_pLua, this, scriptFilePath, data);
				}
			}

			AddObject(pObject);
		}

		return true;
	}
	catch (const Nt::Error& error) {
		ErrorBoxA(Nt::String(error.what()), error.Caption);
		Clear();
	}

	file.close();
	return false;
}
void Scene::Save(const std::string& fileName) {
	std::ofstream file(fileName, std::ios::binary);
	if (!file.is_open()) {
		ErrorBox(L"Failed to save file", L"Error");
		return;
	}

	file.write((Char*)&m_Version, sizeof(m_Version));

	const uInt objectCount = m_Objects.size();
	file.write((Char*)&objectCount, sizeof(uInt));

	for (uInt i = 0; i < objectCount; ++i) {
		file.write((Char*)&m_Objects[i]->ObjectType, sizeof(ObjectTypes));

		if (m_Objects[i]->ObjectType == ObjectTypes::ENTITY) {
			const EntityTypes entityType = UpcastObjectToEntity(m_Objects[i])->GetEntityType();
			file.write((Char*)&entityType, sizeof(EntityTypes));
		}

		m_Objects[i]->Write(file);

		std::string scriptFilePath;
		if (m_Objects[i]->GetScript() != nullptr)
			scriptFilePath = m_Objects[i]->GetScript()->GetFilePath();

		const uInt scriptFilePathLength = scriptFilePath.length();
		file.write((Char*)&scriptFilePathLength, sizeof(uInt));

		if (scriptFilePathLength > 0) {
			file.write(scriptFilePath.data(), scriptFilePath.length());
			Nt::Serialization::WriteAll(file, m_Objects[i]->GetScript()->GetScriptData());
		}
	}

	file.close();
}

void Scene::Copy(Selector* selectorPtr) {
	if (selectorPtr == nullptr) {
		Raise("Selection pointer is null.");
		return;
	}

	if (selectorPtr->IsEmpty())
		return;
	
	ClearBuffer();

	for (Object* pObject : selectorPtr->GetObjectContaiter()) {
		Object* pCopiedObject = pObject->GetCopy();

		pCopiedObject->SetForce({ });
		pCopiedObject->SetLinearAcceleration({ });
		pCopiedObject->SetLinearVelocity({ });

		m_Clipboard.push_back(pCopiedObject);
	}
}
void Scene::Cut(Selector* selectorPtr) {
	if (selectorPtr == nullptr) {
		Raise("Selection pointer is null.");
		return;
	}
	
	if (selectorPtr->IsEmpty())
		return;

	Copy(selectorPtr);
	RemoveSelected(selectorPtr);
}
void Scene::Paste(Selector* selectorPtr) {
	if (selectorPtr == nullptr) {
		Raise("Selection pointer is null.");
		return;
	}
	
	if (m_Clipboard.empty())
		return;

	selectorPtr->AllDeselect();

	for (Object* pObject : m_Clipboard) {
		pObject->Translate({ 1.f, 1.f, 1.f });

		Object* pCopiedObject = pObject->GetCopy();
		selectorPtr->AddSelect(pCopiedObject);
		AddObject(pCopiedObject);
	}
}
void Scene::ClearBuffer() {
	for (Object* pObject : m_Clipboard)
		delete(pObject);

	m_Clipboard.clear();
}

Object* Scene::operator [] (const uInt& index) const {
	return m_Objects[index];
}

Lua* Scene::GetLua() const noexcept {
	return m_pLua;
}
const ObjectContainer& Scene::GetObjects() const {
	return m_Objects;
}
Object* Scene::GetObjectPtr(const uInt& index) const {
	return m_Objects[index];
}
Object* Scene::GetObjectPtrByName(const Nt::String& name) const {
	for (Object* pObject : m_Objects) {
		if (pObject->GetName() == name)
			return pObject;
	}

	Nt::Log::Warning("objects named \"" + name + "\" not found.");

	return nullptr;
}
GameSound* Scene::GetSoundPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameSound(UpcastObjectToEntity(GetObjectPtrByName(name)));
}
GameModel* Scene::GetModelPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameModel(UpcastObjectToEntity(GetObjectPtrByName(name)));
}
GameCamera* Scene::GetCameraPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameCamera(UpcastObjectToEntity(GetObjectPtrByName(name)));
}


void Scene::_Initialize() {
	if (m_pLua != nullptr) {
		Nt::Log::Warning("Scene already initialized");
		return;
	}

	m_pLua = new Lua;
	m_pLua->Initialize(this);
}