#include <Editor/Clipboard.h>
#include <Editor/EditingHistory.h>
#include <Editor/Commands/SceneCommands.h>
#include <Selector.h>
#include <Scene.h>
#include <Nt/Core/EventBus.h>

Clipboard::Clipboard(const std::weak_ptr<Nt::EventBus>& pBus, NotNull<Selector*> pSelector) noexcept :
	m_pEventBus(pBus),
	m_pSelector(pSelector)
{
}

void Clipboard::Copy() {
	if (m_pSelector->IsEmpty())
		return;

	Clear();

	for (const WeakObjectPtr& weakObject : m_pSelector->GetObjectContainer()) {
		const auto& object = weakObject.lock();
		if (object == nullptr)
			continue;

		ObjectPtr pCopiedObject(object->GetCopy());
		pCopiedObject->GetRigidBody()->Body.SetForce(Nt::Float3D());
		pCopiedObject->GetRigidBody()->Body.SetLinearVelocity(Nt::Float3D());

		m_Clipboard.emplace_back(std::move(pCopiedObject));
	}
}
void Clipboard::Cut() {
	if (m_pSelector->IsEmpty() || m_pEventBus.expired())
		return;

	Copy();

	const auto bus = m_pEventBus.lock();
	bus->Emmit<Scene::MultiRemoveWeakObjectsCommand>({ m_pSelector->GetObjectContainer() });
}
void Clipboard::Paste() {
	if (m_Clipboard.empty() || m_pEventBus.expired())
		return;

	const auto bus = m_pEventBus.lock();

	m_pSelector->AllDeselect();
	for (ObjectPtr& object : m_Clipboard) {
		object->Translate({ 1.f, 1.f, 1.f });

		ObjectPtr copy(object->GetCopy());
		m_pSelector->AddSelect(copy);
		bus->Emmit<AddToHistoryCommand>({
			new Edit::AddObjectCommand(m_pEventBus, copy) });
	}
}
void Clipboard::Clear() {
	m_Clipboard.clear();
}