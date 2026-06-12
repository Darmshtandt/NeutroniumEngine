// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Main.h>

#include <Objects/Entities/GameLight.h>

#include <Selector.h>
#include <Scene.h>
#include <Nt/Core/EventBus.h>


Scene::Scene(const std::weak_ptr<Nt::EventBus>& pBus) :
	m_pEventBus(pBus),
	m_LightBuffer(Nt::Buffer::Target::UNIFORM)
{
	Assert(!m_pEventBus.expired(), "EventBus pointer is null");
}
Scene::Scene(const Scene& scene) :
	m_pEventBus(scene.m_pEventBus),
	m_LightBuffer(Nt::Buffer::Target::UNIFORM)
{
	for (const Object* pObject : scene.m_Objects) {
		Object* pCopiedObject = RequireNotNull(pObject->GetCopy());
		pCopiedObject->SetForce({ });

		m_Objects.push_back(pCopiedObject);
	}
}
Scene::~Scene() {
	Clear();
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
	Float shortestDistance = FLT_MAX;
	Object* pNearestObject = nullptr;

	for (Object* pObject : m_Objects) {
		const Float objectDistance = (ray.Start - pObject->GetPosition()).LengthSquare();
		const Int faceIndex = pObject->RayCastTest(ray, pResultIntersectionPoint);

		if (faceIndex != -1 && shortestDistance > objectDistance) {
			shortestDistance = objectDistance;
			pNearestObject = pObject;
		}
	}

	return pNearestObject;
}

void Scene::AddObject(NotNull<Object*> pObject) {
	m_Objects.push_back(pObject);
	if (pObject->GetToken() == GameLight::GetClassToken())
		m_Lights.push_back(pObject.DynamicCast<GameLight*>()->GetData());

	if (!m_pEventBus.expired())
		m_pEventBus.lock()->Emmit<EventAddObject>({ pObject });
}
void Scene::RemoveObject(NotNull<const Object*> pObject) {
	const auto iterator = std::find(m_Objects.begin(), m_Objects.end(), pObject);
	if (iterator == m_Objects.end()) {
		Nt::MessageWindow("Scene::RemoveObject: This object not funded.", "Error").ShowError();
		return;
	}

	if (!m_pEventBus.expired())
		m_pEventBus.lock()->Emmit<EventRemoveObject>({ *iterator });

	delete(*iterator);
	m_Objects.erase(iterator);
}
void Scene::RemoveSelected(NotNull<Selector*> pSelector) {
	for (Object* pObject : pSelector->GetObjectContainer())
		RemoveObject(pObject);
	pSelector->RemoveSelected();
}

void Scene::Clear() {
	for (Object* pObject : m_Objects)
		delete(pObject);

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

	for (Object* pObject : m_Objects) {
		pObject->Update(time);
		if (!(pObject->IsActivePhysics() && pObject->IsEnabledCollision()))
			continue;

		for (Object* pOtherObject : m_Objects) {
			if (pOtherObject == pObject)
				continue;

			if (!(pOtherObject->IsActivePhysics() && pOtherObject->IsEnabledCollision()))
				continue;

			if (pObject->IsDirty())
				handleCollisionIfAllowed(pObject, pOtherObject);
			else if (pOtherObject->IsDirty())
				handleCollisionIfAllowed(pOtherObject, pObject);
		}

		pObject->StaticUpdate();
	}

	const uInt offset = sizeof(Nt::Float4D) + sizeof(Nt::Float3D);
	for (uInt i = 0; i < m_Lights.size(); ++i)
		m_LightBuffer.SetSubData(sizeof(Nt::LightData) * i + offset, sizeof(Nt::Float3D), &m_Lights[i].Position);
}
void Scene::Render(Nt::Renderer* pRenderer) const {
	for (Object* pObject : m_Objects)
		pObject->Render(pRenderer);
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
	return m_Objects[index];
}

Lua* Scene::GetLua() const noexcept {
	return m_pLua;
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
	return m_Objects[index];
}
Object* Scene::GetObjectPtrByName(const Nt::String& name) const {
	for (Object* pObject : m_Objects) {
		if (pObject->GetName() == name)
			return pObject;
	}

	Nt::Log::Instance().Warning("objects named \"" + name + "\" not found.");
	return nullptr;
}