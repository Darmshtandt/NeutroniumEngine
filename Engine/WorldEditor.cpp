#include <WorldEditor.h>

#include <Scene.h>
#include <Selector.h>
#include <WorldDocument.h>
#include <Core/Commands.h>
#include <Core/Grid.h>

#include <Objects/ObjectFactory.h>


WorldEditor::WorldEditor(const std::weak_ptr<Nt::EventBus>& pBus, const Nt::String& defaultInitialPath) :
	m_pGrid(new Grid),
	m_pScene(new Scene(pBus)),
	m_pDocument(new WorldDocument(m_pScene->GetLua())),
	m_pSelector(new Selector(pBus, m_pScene.get(), m_pGrid.get()))
{
	assert(!pBus.expired());

	m_pGrid->SetTarget(&m_Camera);
	m_pDocument->SetDefaultPath(defaultInitialPath);

	auto sharedBus = pBus.lock();
	sharedBus->Subscribe<SelectObjectCommand>([this] (const SelectObjectCommand& e) {
		m_pSelector->Select(e.pObject);
		});
	sharedBus->Subscribe<MultiSelectObjectCommand>([this] (const MultiSelectObjectCommand& e) {
		m_pSelector->AddSelect(e.pObject);
		});
	sharedBus->Subscribe<DeselectObjectCommand>([this] (const DeselectObjectCommand& e) {
		m_pSelector->Deselect(e.pObject);
		});
	sharedBus->Subscribe<DeselectAllObjectsCommand>([this] (const DeselectAllObjectsCommand& e) {
		(void)e;
		m_pSelector->AllDeselect();
		});
}

void WorldEditor::CreatePrimitive(const std::string& className) {
	Object* pObject = PrimitiveFactory::Instance().Create(className, className);
	m_pScene->AddObject(pObject);
	m_IsChanged = true;
}

void WorldEditor::CreateEntity(const std::string& className) {
	Object* pObject = EntityFactory::Instance().Create(className, className);
	m_pScene->AddObject(pObject);
	m_IsChanged = true;
}


void WorldEditor::ResetCamera() noexcept {
	m_Camera.SetPosition({ 0.f, -3.f, -5.f });
	m_Camera.SetAngle({ -35.f * RADf, 0.f, 0.f });
}

void WorldEditor::Update(Float deltaTime) {
	m_pSelector->Update();
	m_pScene->Update(deltaTime);
	m_pGrid->Update();
}

void WorldEditor::Render(NotNull<Nt::Renderer*> pRenderer) {
	m_pGrid->Render(pRenderer);
	m_pScene->Render(pRenderer);
	m_pSelector->Render(pRenderer);
}

void WorldEditor::New() {
	ResetCamera();
	m_pScene->Clear();
	m_IsChanged = false;
}

void WorldEditor::Open() {
	m_pSelector->AllDeselect();
	m_pSelector->UnmarkChanged();

	if (m_pDocument->Open(m_pScene.get()))
		m_IsChanged = false;
}

void WorldEditor::Save() {
	if (m_pDocument->Save(m_pScene.get()))
		m_IsChanged = false;
}

void WorldEditor::SaveAs() {
	if (m_pDocument->SaveAs(m_pScene.get()))
		m_IsChanged = false;
}

Nt::Camera& WorldEditor::GetCamera() noexcept {
	return m_Camera;
}

Grid* WorldEditor::GetGrid() const noexcept {
	return m_pGrid.get();
}

Scene* WorldEditor::GetScene() const noexcept {
	return m_pScene.get();
}

Selector* WorldEditor::GetSelector() const noexcept {
	return m_pSelector.get();
}

Bool WorldEditor::IsChanged() const noexcept {
	return m_IsChanged;
}