#include <Editor/Commands/SceneCommands.h>

#include <Scene.h>
#include <Nt/Core/EventBus.h>

namespace Edit {
	AddObjectCommand::AddObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectPtr& object) :
		m_pEventBus(pEventBus),
		m_pObject(object)
	{
	}

	void AddObjectCommand::Execute() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::AddObjectCommand>({ m_pObject });
	}
	void AddObjectCommand::Undo() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::RemoveObjectCommand>({ m_pObject });
	}


	RemoveObjectCommand::RemoveObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectPtr& object) :
		m_pEventBus(pEventBus),
		m_pObject(object)
	{
	}

	void RemoveObjectCommand::Execute() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::RemoveObjectCommand>({ m_pObject });
	}
	void RemoveObjectCommand::Undo() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::AddObjectCommand>({ m_pObject });
	}


	MultiAddObjectCommand::MultiAddObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectContainer& objects) :
		m_pEventBus(pEventBus),
		m_Objects(objects)
	{
	}

	void MultiAddObjectCommand::Execute() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::MultiAddObjectsCommand>({ m_Objects });
	}
	void MultiAddObjectCommand::Undo() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::MultiRemoveObjectsCommand>({ m_Objects });
	}


	MultiRemoveObjectCommand::MultiRemoveObjectCommand(const std::weak_ptr<Nt::EventBus>& pEventBus, const ObjectContainer& objects) :
		m_pEventBus(pEventBus),
		m_Objects(objects)
	{
	}

	void MultiRemoveObjectCommand::Execute() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::MultiRemoveObjectsCommand>({ m_Objects });
	}
	void MultiRemoveObjectCommand::Undo() {
		if (const auto bus = m_pEventBus.lock())
			bus->Emmit<Scene::MultiAddObjectsCommand>({ m_Objects });
	}
}