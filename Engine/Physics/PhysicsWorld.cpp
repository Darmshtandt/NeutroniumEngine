#include <Physics/PhysicsWorld.h>

#include <Scene.h>
#include <Nt/Core/EventBus.h>

PhysicsWorld::PhysicsWorld(const std::weak_ptr<Nt::EventBus>& pBus) :
	m_pEventBus(pBus)
{
	Assert(!m_pEventBus.expired(), "EventBus pointer is null");

	const auto bus = m_pEventBus.lock();
	bus->Subscribe<Scene::EventAddObject>([this] (const Scene::EventAddObject& e) {
		AddObject(e.pObject);
		});
	bus->Subscribe<Scene::EventRemoveObject>([this] (const Scene::EventRemoveObject& e) {
		RemoveObject(e.pObject.get());
		});
	bus->Subscribe<Scene::EventClear>([this] (const Scene::EventClear& e) {
		m_StaticObjects.clear();
		m_DynamicObjects.clear();
		});
}

void PhysicsWorld::AddObject(const ObjectPtr& object) {
	assert(object);
	if (!object)
		return;

	PhysicObjectPtr physObject(new PhysicObject);
	physObject->Transform = object->GetTransform();
	physObject->Collider = object->GetCollider();
	physObject->RigidBody = object->GetRigidBody();
	physObject->Object = object;

	if (!physObject->Transform || !physObject->RigidBody)
		return;

	if (object->GetScript() == nullptr)
		m_StaticObjects.emplace_back(std::move(physObject));
	else
		m_DynamicObjects.emplace_back(std::move(physObject));
}

void PhysicsWorld::RemoveObject(NotNull<Object*> pObject) {
	if (pObject->GetScript() == nullptr) {
		std::erase_if(m_StaticObjects, [pObject] (const PhysicObjectPtr& physObject) {
			return pObject == physObject->Object.get();
			});
	}
	else {
		std::erase_if(m_DynamicObjects, [pObject] (const PhysicObjectPtr& physObject) {
			return pObject == physObject->Object.get();
			});
	}
}

void PhysicsWorld::Update(Float deltaTime) {
	const auto findLayerByName = [&] (LayerContainer& container, const Nt::String& layerName) {
		return std::find_if(container.begin(), container.end(), [&] (const Layer& layer) {
			return layer.Name == layerName;
			});
		};

	const auto handleCollisionIfAllowed = [&] (Object* pPrimaryObject, Object* pSecondaryObject) {
		const auto layerIterator = findLayerByName(m_DisjointLayers, pPrimaryObject->GetLayerName());
		if (layerIterator != m_DisjointLayers.end()) {
			if (findLayerByName(layerIterator->Layers, pSecondaryObject->GetLayerName()) != layerIterator->Layers.end())
				return;
		}

		pPrimaryObject->Collision(pSecondaryObject);
		};

	//for (const PhysicObjectPtr& object : m_DynamicObjects)
	//	object->RigidBody->Update(deltaTime);

	for (const PhysicObjectPtr& object : m_DynamicObjects) {
		if (!object->Object->EnabledCollider())
			continue;

		if (object->Transform->IsDirty()) {
			for (const PhysicObjectPtr& otherObject : m_StaticObjects) {
				if (object->Object->EnabledCollider())
					handleCollisionIfAllowed(object->Object.get(), otherObject->Object.get());
			}
		}

		for (const PhysicObjectPtr& otherObject : m_DynamicObjects) {
			if (otherObject == object)
				continue;

			if (!otherObject->Object->EnabledCollider())
				continue;

			if (object->Transform->IsDirty())
				handleCollisionIfAllowed(object->Object.get(), otherObject->Object.get());
			else if (otherObject->Transform->IsDirty())
				handleCollisionIfAllowed(otherObject->Object.get(), object->Object.get());
		}

		object->Object->StaticUpdate();
	}
}