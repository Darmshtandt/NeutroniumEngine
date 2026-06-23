#pragma once

#include <Nt/Core/Utilities.h>
#include <Objects/Object.h>

namespace Nt {
	class EventBus;
}

class PhysicsWorld {
	struct PhysicObject final {
		Nt::IObject* Transform = nullptr;
		Nt::Collider* Collider = nullptr;
		NtEx::RigidBody* RigidBody = nullptr;
		ObjectPtr Object;
	};

	using PhysicObjectPtr = std::unique_ptr<PhysicObject>;

	struct Layer {
		std::vector<Layer> Layers;
		Nt::String Name;
	};

	using LayerContainer = std::vector<Layer>;

public:
	explicit PhysicsWorld(const std::weak_ptr<Nt::EventBus>& pBus);

	void AddObject(const ObjectPtr& object);
	void RemoveObject(NotNull<Object*> pObject);

	void Update(Float deltaTime);

private:
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	std::vector<PhysicObjectPtr> m_StaticObjects;
	std::vector<PhysicObjectPtr> m_DynamicObjects;
	std::vector<Layer> m_DisjointLayers;
};