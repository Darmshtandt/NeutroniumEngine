#pragma once

#include <Objects/Object.h>
#include <Nt/Light.h>

namespace Nt {
	class EventBus;
}

class Selector;

class Scene {
public:
	struct AddObjectCommand final {
		ObjectPtr Object;
	};
	struct RemoveObjectCommand final {
		ObjectPtr Object;
	};
	struct MultiAddObjectCommand final {
		ObjectContainer Objects;
	};
	struct MultiRemoveObjectCommand final {
		ObjectContainer Objects;
	};
	struct ClearCommand final
	{
	};

	struct EventAddObject final {
		ObjectPtr pObject;
	};
	struct EventRemoveObject final {
		ObjectPtr pObject;
	};
	struct EventClear final
	{
	};

	struct Layer {
		std::vector<Layer> Layers;
		Nt::String Name;
	};

	using LayerContainer = std::vector<Layer>;

	enum class Versions {
		TEST_0_1,
		LAST
	};

public:
	explicit Scene(const std::weak_ptr<Nt::EventBus>& pBus);
	Scene(const Scene& scene);
	~Scene();

	void Start();
	void Stop();

	ObjectPtr RayCastObject(const Nt::Ray& ray, Nt::Float3D* pResultIntersectionPoint = nullptr);

	void AddObject(NotNull<Object*> pObject);
	void AddObject(const ObjectPtr& pObject);
	void RemoveObject(NotNull<const Object*> pObject);
	void RemoveSelected(NotNull<Selector*> pSelector);

	void Clear();

	void Update(const Float& time);

	void AllowLayerOverlap(const Nt::String& layerName_1, const Nt::String& layerName_2, const Bool& isAllow);

	Object* operator [] (const uInt& index) const;

	template <class _Ty>
	_Ty* GetObjectPtrByType() {
		return GetObjectPtrByType<_Ty>(m_Objects.begin());
	}

	template <class _Ty>
	_Ty* GetObjectPtrByType(ObjectContainer::iterator iterator) {
		for (; iterator != m_Objects.end(); ++iterator) {
			_Ty* pObject = dynamic_cast<_Ty*>(iterator->get());
			if (pObject != nullptr)
				return pObject;
		}

		return nullptr;
	}

	template <class _Ty>
	Object* GetObjectPtrByTypeAndName(const Nt::String& name) {
		Object* pObject = nullptr;
		for (auto iterator = m_Objects.begin(); iterator != m_Objects.end(); ++iterator) {
			pObject = GetObjectPtrByType<_Ty>(iterator);
			if (pObject == nullptr || pObject->GetName() == name)
				break;
		}
		return pObject;
	}

	const ObjectContainer& GetObjects() const;
	const Nt::Buffer& GetLightBuffer() const noexcept;
	uInt GetLightsCount() const noexcept;
	Lua* GetLua() const noexcept;
	Object* GetObjectPtr(const uInt& index) const;
	Object* GetObjectPtrByName(const Nt::String& name) const;

	void SetEventBus(const std::weak_ptr<Nt::EventBus>& pEventBus) noexcept;

private:
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	std::vector<Layer> m_DisjointLayers;
	std::vector<Nt::LightData> m_Lights;
	Nt::Buffer m_LightBuffer;

	Lua* m_pLua = new Lua(this);
	Versions m_Version = Versions::LAST;
	ObjectContainer m_Objects;
};