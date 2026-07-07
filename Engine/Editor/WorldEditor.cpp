#include <Editor/WorldEditor.h>

#include <InputContext.h>
#include <Editor/EditingHistory.h>

#include <Scene.h>
#include <Selector.h>
#include <Editor/WorldDocument.h>
#include <Editor/Clipboard.h>
#include <Core/Commands.h>
#include <Core/Grid.h>
#include <Objects/ObjectFactory.h>

WorldEditor::WorldEditor(const std::weak_ptr<Nt::EventBus>& pBus, const Nt::String& defaultInitialPath) :
	m_InputContext(new InputContext),
	m_EditingHistory(new EditingHistory),
	m_Grid(new Grid),
	m_Scene(new Scene(pBus)),
	m_Document(new WorldDocument(m_Scene->GetLua())),
	m_Selector(new Selector(pBus, m_Scene.get(), m_Grid.get())),
	m_Clipboard(new Clipboard(pBus, m_Selector.get()))
{
	assert(!pBus.expired());

	m_Grid->SetTarget(m_Camera.Transform());
	m_Document->SetDefaultPath(defaultInitialPath);

	auto sharedBus = pBus.lock();
	sharedBus->Subscribe<SelectObjectCommand>([this] (const SelectObjectCommand& e) {
		m_Selector->Select(e.pObject);
		});
	sharedBus->Subscribe<MultiSelectObjectCommand>([this] (const MultiSelectObjectCommand& e) {
		m_Selector->AddSelect(e.pObject);
		});
	sharedBus->Subscribe<DeselectObjectCommand>([this] (const DeselectObjectCommand& e) {
		m_Selector->Deselect(e.pObject);
		});
	sharedBus->Subscribe<DeselectAllObjectsCommand>([this] (const DeselectAllObjectsCommand& e) {
		(void)e;
		m_Selector->AllDeselect();
		});
	
	sharedBus->Subscribe<AddToHistoryCommand>([this] (const AddToHistoryCommand& e) {
		m_EditingHistory->AddEndExecute(e.Command);
		});


	sharedBus->Emmit<AddInputContextEvent>({ m_InputContext, "WorldEditor" });

	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_C }, [this] () { m_Clipboard->Copy(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_X }, [this] () { m_Clipboard->Cut(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_V }, [this] () { m_Clipboard->Paste(); });

	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_Z }, [this] () { m_EditingHistory->Undo(); });
	m_InputContext->AddHotKey(
		{ Nt::KEY_CONTROL, Nt::KEY_Y }, [this] () { m_EditingHistory->Redo(); });
}
WorldEditor::~WorldEditor() noexcept = default;

void WorldEditor::CreatePrimitive(const std::string& className) {
	Object* pObject = PrimitiveFactory::Instance().Create(className, className);
	m_Scene->AddObject(pObject);
	m_IsChanged = true;
}

void WorldEditor::CreateEntity(const std::string& className) {
	Object* pObject = EntityFactory::Instance().Create(className, className);
	m_Scene->AddObject(pObject);
	m_IsChanged = true;
}


void WorldEditor::ResetCamera() noexcept {
	m_Camera.Position({ 0.f, -3.f, -5.f });
	m_Camera.RotationEuler({ -35.f * RADf, 0.f, 0.f });
}

void WorldEditor::Update(Float deltaTime) {
	m_Selector->Update();
	m_Scene->Update(deltaTime);
	m_Grid->Update();
}

void WorldEditor::New() {
	ResetCamera();
	m_Scene->Clear();
	m_IsChanged = false;
}

void WorldEditor::Open() {
	m_Selector->AllDeselect();
	m_Selector->UnmarkChanged();

	if (m_Document->Open(m_Scene.get()))
		m_IsChanged = false;
}

void WorldEditor::Save() {
	if (m_Document->Save(m_Scene.get()))
		m_IsChanged = false;
}

void WorldEditor::SaveAs() {
	if (m_Document->SaveAs(m_Scene.get()))
		m_IsChanged = false;
}

NtEx::Camera3D& WorldEditor::GetCamera() noexcept {
	return m_Camera;
}

Grid* WorldEditor::GetGrid() const noexcept {
	return m_Grid.get();
}

std::weak_ptr<Scene> WorldEditor::GetScene() const noexcept {
	return m_Scene;
}

Selector* WorldEditor::GetSelector() const noexcept {
	return m_Selector.get();
}

Bool WorldEditor::IsChanged() const noexcept {
	return m_IsChanged;
}