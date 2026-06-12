#include <Objects/Clipboard.h>
#include <Selector.h>
#include <Scene.h>

Clipboard::Clipboard(NotNull<Scene*> pScene, NotNull<Selector*> pSelector) noexcept :
	m_pSelector(pSelector),
	m_pScene(pScene)
{
}

void Clipboard::Copy() {
	if (m_pSelector->IsEmpty())
		return;

	Clear();

	for (const Object* pObject : m_pSelector->GetObjectContainer()) {
		ObjectPtr pCopiedObject(pObject->GetCopy());

		pCopiedObject->SetForce(Nt::Float3D());
		pCopiedObject->SetLinearAcceleration(Nt::Float3D());
		pCopiedObject->SetLinearVelocity(Nt::Float3D());

		m_Clipboard.emplace_back(std::move(pCopiedObject));
	}
}
void Clipboard::Cut() {
	if (m_pSelector->IsEmpty())
		return;

	Copy();
	m_pScene->RemoveSelected(m_pSelector);
}
void Clipboard::Paste() {
	if (m_Clipboard.empty())
		return;

	m_pSelector->AllDeselect();

	for (ObjectPtr& pObject : m_Clipboard) {
		pObject->Translate({ 1.f, 1.f, 1.f });

		Object* pCopiedObject = pObject->GetCopy();
		m_pSelector->AddSelect(pCopiedObject);
		m_pScene->AddObject(pCopiedObject);
	}
}
void Clipboard::Clear() {
	m_Clipboard.clear();
}