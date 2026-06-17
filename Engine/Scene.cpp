// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Main.h>

#include <Objects/Entities/GameLight.h>

#include <Selector.h>
#include <Scene.h>
#include <Nt/Core/EventBus.h>


Scene::Scene(const std::weak_ptr<Nt::EventBus>& pBus) :
	m_pEventBus(pBus),
	m_LightBuffer(Nt::Buffer::Target::UNIFORM),
	m_Lua(new Lua(this))
{
	Assert(!m_pEventBus.expired(), "EventBus pointer is null");

	auto sharedBus = m_pEventBus.lock();
	sharedBus->Subscribe<AddObjectCommand>([this] (const AddObjectCommand& e) {
		AddObject(e.Object);
		});
	sharedBus->Subscribe<RemoveObjectCommand>([this] (const RemoveObjectCommand& e) {
		RemoveObject(e.Object.get());
		});

	sharedBus->Subscribe<MultiAddObjectsCommand>([this] (const MultiAddObjectsCommand& e) {
		for (const auto& object : e.Objects)
			AddObject(object);
		});
	sharedBus->Subscribe<MultiRemoveObjectsCommand>([this] (const MultiRemoveObjectsCommand& e) {
		for (const auto& object : e.Objects)
			RemoveObject(object.get());
		});
	sharedBus->Subscribe<MultiRemoveWeakObjectsCommand>([this] (const MultiRemoveWeakObjectsCommand& e) {
		for (const auto& weakObject : e.Objects) {
			const auto object = weakObject.lock();
			assert(object);

			RemoveObject(object.get());
		}
		});
}
Scene::Scene(const Scene& scene) :
	m_pEventBus(scene.m_pEventBus),
	m_LightBuffer(Nt::Buffer::Target::UNIFORM),
	m_Lua(new Lua(this))
{
	for (const ObjectPtr& object : scene.m_Objects) {
		Object* copiedObject = RequireNotNull(object->GetCopy());
		copiedObject->SetForce({ });

		Script* pScript = object->GetScript();
		if (pScript != nullptr) {
			std::string filePath = pScript->GetFilePath();
			copiedObject->AttachScript(m_Lua.get(), filePath, pScript->GetScriptData());
		}

		m_Objects.emplace_back(copiedObject);
	}
}
Scene::~Scene() {
	Stop();
	Clear();
}

void Scene::Start() {
	for (const ObjectPtr& object : m_Objects)
		object->Start();
}
void Scene::Stop() {
	for (const ObjectPtr& object : m_Objects)
		object->Stop();
}

ObjectPtr Scene::RayCastObject(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint) {
	Float shortestDistance = FLT_MAX;
	ObjectPtr pNearestObject = nullptr;

	for (const ObjectPtr& object : m_Objects) {
		Nt::Float3D intersectionPoint;
		const Int faceIndex = object->RayCastTest(ray, &intersectionPoint);
		const Float objectDistance = (ray.Start - intersectionPoint).LengthSquare();

		if (faceIndex != -1 && shortestDistance > objectDistance) {
			shortestDistance = objectDistance;
			pNearestObject = object;

			if (pResultIntersectionPoint != nullptr)
				*pResultIntersectionPoint = intersectionPoint;
		}
	}

	return pNearestObject;
}

void Scene::AddObject(NotNull<Object*> pObject) {
	AddObject(ObjectPtr(pObject.Get()));
}
void Scene::AddObject(const ObjectPtr& pObject) {
	m_Objects.emplace_back(RequireNotNull(pObject));
	if (pObject->GetToken() == GameLight::GetClassToken())
		m_Lights.push_back(static_cast<GameLight*>(pObject.get())->GetData());

	if (!m_pEventBus.expired())
		m_pEventBus.lock()->Emmit<EventAddObject>({ pObject });
}
void Scene::RemoveObject(NotNull<const Object*> pObject) {
	auto iterator = FindObject(m_Objects, pObject);
	if (iterator == m_Objects.end()) {
		Nt::MessageWindow("Scene::RemoveObject: This object not funded.", "Error").ShowError();
		return;
	}

	if (!m_pEventBus.expired())
		m_pEventBus.lock()->Emmit<EventRemoveObject>({ *iterator });
	m_Objects.erase(iterator);
}
void Scene::RemoveSelected(NotNull<Selector*> pSelector) {
	for (const WeakObjectPtr& object : pSelector->GetObjectContainer())
		RemoveObject(object.lock().get());
	pSelector->RemoveSelected();
}

void Scene::Clear() {
	if (!m_pEventBus.expired())
		m_pEventBus.lock()->Emmit<EventClear>({ });
	m_Objects.clear();
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


	if (m_Lights.empty()) {
		m_LightBuffer.SetData(m_LightBuffer.GetSize(), nullptr, Nt::USAGE_STREAMDRAW);
	}
	else {
		const uInt bufferSize = (m_Lights.size() * sizeof(Nt::LightData));
		m_LightBuffer.SetData(bufferSize, m_Lights.data(), Nt::USAGE_STREAMDRAW);
	}

	for (const ObjectPtr& object : m_Objects) {
		object->Update(time);
		if (!(object->IsActivePhysics() && object->IsEnabledCollision()))
			continue;

		for (const ObjectPtr& otherObject : m_Objects) {
			if (otherObject == object)
				continue;

			if (!(otherObject->IsActivePhysics() && otherObject->IsEnabledCollision()))
				continue;

			if (object->IsDirty())
				handleCollisionIfAllowed(object.get(), otherObject.get());
			else if (otherObject->IsDirty())
				handleCollisionIfAllowed(otherObject.get(), object.get());
		}

		object->StaticUpdate();
	}

	const uInt offset = sizeof(Nt::Float4D) + sizeof(Nt::Float3D);
	for (uInt i = 0; i < m_Lights.size(); ++i)
		m_LightBuffer.SetSubData(sizeof(Nt::LightData) * i + offset, sizeof(Nt::Float3D), &m_Lights[i].Position);
}

void Scene::AllowLayerOverlap(const Nt::String& firstLayerName, const Nt::String& secondLayerName, const Bool& isAllow) {
	if (firstLayerName == secondLayerName) {
		Nt::Log::Instance().Warning("Layer names are the same");
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
		layer.Layers.emplace_back(LayerContainer(), secondLayerName);

		m_DisjointLayers.push_back(layer);
	}
}

Object* Scene::operator [] (const uInt& index) const {
	return m_Objects[index].get();
}

Lua* Scene::GetLua() const noexcept {
	return m_Lua.get();
}
const ObjectContainer& Scene::GetObjects() const {
	return m_Objects;
}
const Nt::Buffer& Scene::GetLightBuffer() const noexcept {
	return m_LightBuffer;
}
uInt Scene::GetLightsCount() const noexcept {
	return m_Lights.size();
}
Object* Scene::GetObjectPtr(const uInt& index) const {
	return m_Objects[index].get();
}
Object* Scene::GetObjectPtrByName(const Nt::String& name) const {
	for (const ObjectPtr& object : m_Objects) {
		if (object->GetName() == name)
			return object.get();
	}

	Nt::Log::Instance().Warning("Objects named \"" + name + "\" not found.");
	return nullptr;
}

void Scene::SetEventBus(const std::weak_ptr<Nt::EventBus>& pEventBus) noexcept {
	m_pEventBus = pEventBus;
}