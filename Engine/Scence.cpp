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
#include <Scence.h>


Scence::Scence() {
	_Initialize();
}
Scence::Scence(const Scence& scence) {
	_Initialize();

	for (const Object* pObject : scence.m_Objects) {
		Object* pCopiedObject = pObject->GetCopy();
		if (pCopiedObject == nullptr)
			Raise("Failed to crete object.");

		pCopiedObject->SetForce({ });

		const Script* pScript = pObject->GetScript();
		if (pScript) {
			pCopiedObject->AttachScript(m_pLua, this, pScript->GetFilePath());
			pCopiedObject->GetScript()->SetScriptData(pScript->GetScriptData());
		}
		m_Objects.push_back(pCopiedObject);
	}
}
Scence::~Scence() {
	Clear();
	ClearBuffer();
}

void Scence::Start() {
	for (Object* pObject : m_Objects)
		pObject->Start();
}
void Scence::Stop() {
	for (Object* pObject : m_Objects)
		pObject->Stop();
}

Object* Scence::RayCastObject(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint) {
	Float nearestSquareDistance = -FLT_MAX;
	Object* pNearestObject = nullptr;
	for (uInt i = 0; i < m_Objects.size(); ++i) {
		Object* pObject = m_Objects[i];
		if (pObject == nullptr)
			Raise("Object pointer is null.");

		const Float objectSquareDistance = (ray.Start - pObject->GetPosition()).LengthSquare();
		const Int faceIndex = pObject->RayCastTest(ray, pResultIntersectionPoint);
		if (faceIndex != -1 && nearestSquareDistance < objectSquareDistance) {
			nearestSquareDistance = objectSquareDistance;
			pNearestObject = pObject;
		}
	}
	return pNearestObject;
}

void Scence::BindObjectsTree(ObjectsTree* objectsTreePtr) noexcept {
	m_ObjectsTreePtr = objectsTreePtr;
	if (m_ObjectsTreePtr) {
		m_ObjectsTreePtr->Clear();
		for (uInt i = 0; i < m_Objects.size(); ++i)
			m_ObjectsTreePtr->Add(m_Objects[i]);
	}
}

void Scence::AddObject(Object* pObject) {
	if (pObject == nullptr)
		Raise("Object pointer is null.");

	if (m_ObjectsTreePtr)
		m_ObjectsTreePtr->Add(pObject);
	m_Objects.push_back(pObject);
}
void Scence::RemoveObject(const Object* pObject) {
	auto iterator = std::find(m_Objects.begin(), m_Objects.end(), pObject);
	if (iterator != m_Objects.end()) {
		delete(*iterator);
		m_Objects.erase(iterator);
	}
	else {
		ERROR_MSG(L"Scence::RemoveObject: This object is not finded.", L"Error");
	}
}
void Scence::RemoveSelected(Selector* pSelector) {
	for (Object* pObject : pSelector->GetObjects())
		RemoveObject(pObject);
	pSelector->AllDeselect();

	if (m_ObjectsTreePtr) {
		m_ObjectsTreePtr->Clear();
		for (Object* pObject : m_Objects)
			m_ObjectsTreePtr->Add(pObject);
	}
}

void Scence::Clear() {
	for (Object* pObject : m_Objects)
		delete(pObject);

	m_Objects.clear();
	if (m_ObjectsTreePtr)
		m_ObjectsTreePtr->Clear();
}

void Scence::Update(const Float& time) {
	for (Object* pObject : m_Objects) {
		pObject->Update(time);
		if (pObject->IsActive() && pObject->IsEnabledCollision()) {
			for (Object* pOtherObject : m_Objects) {
				if (pOtherObject->IsActive() && pOtherObject->IsEnabledCollision()) {
					auto layerIterator = _FindLayer(m_NonIntersectingLayers, pObject->GetLayerName());
					auto otherLayerIterator = _FindLayer(m_NonIntersectingLayers, pOtherObject->GetLayerName());

					if (layerIterator != m_NonIntersectingLayers.end()) {
						if (_FindLayer(layerIterator->Layers, pOtherObject->GetLayerName()) != layerIterator->Layers.end())
							continue;
					}
					if (otherLayerIterator != m_NonIntersectingLayers.end()) {
						if (_FindLayer(otherLayerIterator->Layers, pObject->GetLayerName()) != otherLayerIterator->Layers.end())
							continue;
					}

					if (pObject->IsChanged())
						pObject->Collision(pOtherObject);
					else if (pOtherObject->IsChanged())
						pOtherObject->Collision(pObject);
				}
			}
		}
		pObject->UnmarkChanged();
	}
}
void Scence::Render(Nt::Renderer* pRenderer) const {
	for (Object* pObject : m_Objects)
		pObject->Render(pRenderer);
}

void Scence::AllowIntersectionOfLayers(const Nt::String& layerName_1, const Nt::String& layerName_2, const Bool& isAllow) {
	if (layerName_1 == layerName_2) {
		Nt::Log::Warning("Layer names are the same");
		return;
	}

	auto beginLayer = m_NonIntersectingLayers.begin();
	auto endLayer = m_NonIntersectingLayers.end();
	auto findedLayerIterator = std::find_if(beginLayer, endLayer,
		[&](const Layer& layer) {
			return (layer.Name == layerName_1 || layer.Name == layerName_2);
		});

	if (isAllow) {
		if (findedLayerIterator != endLayer)
			m_NonIntersectingLayers.erase(findedLayerIterator);
	}
	else {
		if (findedLayerIterator != endLayer) {
			std::string unconflictedLayerName =
				(findedLayerIterator->Name == layerName_1) ? layerName_2 : layerName_1;

			beginLayer = findedLayerIterator->Layers.begin();
			endLayer = findedLayerIterator->Layers.end();
			auto unconflictedLayerIterator = std::find_if(beginLayer, endLayer,
				[&unconflictedLayerName](const Layer& layer) {
					return (layer.Name == unconflictedLayerName);
				});

			if (unconflictedLayerIterator == endLayer)
				findedLayerIterator->Layers.push_back({ { }, unconflictedLayerName });
		}
		else {
			Layer layer;
			layer.Name = layerName_1;
			layer.Layers.push_back({ { }, layerName_2 });
			m_NonIntersectingLayers.push_back(layer);
		}
	}
}

Bool Scence::Load(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	if (file.is_open()) {
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

					switch (entityType) {
					case EntityTypes::CAMERA:
						pObject = new GameCamera("");
						break;
					case EntityTypes::MODEL:
						pObject = new GameModel("");
						break;
					case EntityTypes::SOUND:
						pObject = new GameSound("");
						break;
					default:
						Raise("Non-existent entity type specified");
					}
				}
				else {
					pObject = dynamic_cast<Object*>(Object::New(uInt(objectType)));
				}

				if (pObject == nullptr)
					Raise("Failed to load scence");
				pObject->Read(file);

				std::string scriptFilePath;
				uInt pathLength;
				file.read((Char*)&pathLength, sizeof(uInt));
				if (pathLength > 0) {
					scriptFilePath.resize(pathLength);
					file.read(scriptFilePath.data(), pathLength);
				}

				if (scriptFilePath != "") {
					pObject->AttachScript(m_pLua, this, scriptFilePath);

					std::vector<Script::Data> data;
					Nt::Serialization::ReadAll(file, data);
					pObject->GetScript()->SetScriptData(data);
				}
				AddObject(pObject);
			}
			return true;
		}
		catch (const Nt::Error& error) {
			Clear();
			ErrorBoxA(Nt::String(error.what()), error.Caption);
		}
		file.close();
	}
	else {
		ErrorBox(L"Failed to open file", L"Error");
	}
	return false;
}
void Scence::Save(const std::string& fileName) {
	std::ofstream file(fileName, std::ios::out | std::ios::binary);
	if (file.is_open()) {
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
			if (m_Objects[i]->GetScript())
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
	else {
		ErrorBox(L"Failed to save file", L"Error");
	}
}

void Scence::Copy(Selector* selectorPtr) {
	if (selectorPtr == nullptr)
		Raise("Selection pointer is null.");
	if (selectorPtr->GetObjects().size() == 0)
		return;

	ClearBuffer();
	for (Object* pObject : selectorPtr->GetObjects()) {
		Object* pCopiedObject = pObject->GetCopy();

		pCopiedObject->SetForce({ });
		pCopiedObject->SetLinearAcceleration({ });
		pCopiedObject->SetLinearVelocity({ });

		m_Buffer.push_back(pCopiedObject);
	}
}
void Scence::Cut(Selector* selectorPtr) {
	if (selectorPtr == nullptr)
		Raise("Selection pointer is null.");
	if (selectorPtr->GetObjects().size() == 0)
		return;

	Copy(selectorPtr);
	RemoveSelected(selectorPtr);
}
void Scence::Paste(Selector* selectorPtr) {
	if (selectorPtr == nullptr)
		Raise("Selection pointer is null.");
	if (m_Buffer.size() == 0)
		return;

	selectorPtr->AllDeselect();
	for (Object* pObject : m_Buffer) {
		pObject->Translate({ 1.f, 1.f, 1.f });
		Object* pCopiedObject = pObject->GetCopy();
		selectorPtr->AddSelect(pCopiedObject);
		AddObject(pCopiedObject);
	}
}
void Scence::ClearBuffer() {
	for (Object* pObject : m_Buffer)
		delete(pObject);
	m_Buffer.clear();
}

Object* Scence::operator [] (const uInt& index) const {
	return m_Objects[index];
}

Lua* Scence::GetLua() const noexcept {
	return m_pLua;
}
const ObjectContainer& Scence::GetObjects() const {
	return m_Objects;
}
Object* Scence::GetObjectPtr(const uInt& index) const {
	return (*this)[index];
}
Object* Scence::GetObjectPtrByName(const Nt::String& name) const {
	for (Object* pObject : m_Objects)
		if (pObject->GetName() == name)
			return pObject;
	Nt::Log::Warning("objects named \"" + name + "\" not found.");
	return nullptr;
}
GameSound* Scence::GetSoundPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameSound(UpcastObjectToEntity(GetObjectPtrByName(name)));
}
GameModel* Scence::GetModelPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameModel(UpcastObjectToEntity(GetObjectPtrByName(name)));
}
GameCamera* Scence::GetCameraPtrByName(const Nt::String& name) const {
	return UpcastEntityToGameCamera(UpcastObjectToEntity(GetObjectPtrByName(name)));
}


void Scence::_Initialize() {
	if (m_pLua != nullptr) {
		Nt::Log::Warning("Scence already initialized");
		return;
	}

	m_pLua = new Lua;
	m_pLua->Initialize(this);
}

Scence::LayerContainer::iterator Scence::_FindLayer(LayerContainer& container, const Nt::String& layerName) {
	return std::find_if(container.begin(), container.end(),
		[&](const Layer& layer) {
			return (layer.Name == layerName);
		});
}