// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Main.h>

#include <Script/Lua.h>
#include <Objects/Object.h>
#include <Scene.h>
#include <Core/Grid.h>
#include <Selector.h>

#include <MessageBus.h>
#include <ObjectEvents.h>
#include <Nt/Core/EventBus.h>

Selector::Selector(const std::weak_ptr<Nt::EventBus>& pBus, NotNull<Scene*> pScene, NotNull<Grid*> pGrid) :
	m_pEventBus(pBus),
	m_pScene(pScene),
	m_pGrid(pGrid),
	m_pManipulator(new Manipulator),
	m_pRayMesh(new Nt::Mesh)
{
	Assert(!m_pEventBus.expired(), "EventBus pointer is null");

	Nt::Shape shape;
	shape.Vertices.emplace_back(Nt::Vertex({ }, { }, { }, Nt::Colors::White));
	shape.Vertices.emplace_back(Nt::Vertex({ }, { }, { }, Nt::Colors::White));
	shape.Indices.push_back(0);
	shape.Indices.push_back(1);

	m_pRayMesh->SetShape(shape);
	m_pManipulator->Hide();

	auto sharedBus = m_pEventBus.lock();
	sharedBus->Subscribe<UpdateObjectTransformEvent>([this] (const UpdateObjectTransformEvent& e) {
		if (e.pEmmiter == this)
			return;

		if (e.Type == TransforType::POSITION)
			m_pManipulator->SetPosition(e.Value);
		});
}

void Selector::Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Keyboard& keyboard, Nt::Mouse& mouse) {
	const Nt::Float2D cursorPosition = pWindow->ScreenToClient(mouse.GetCursorPosition());
	m_Ray = Nt::RayFromPoint2D(cursorPosition, -camera.GetPosition(),
		pWindow->GetClientSize(), pWindow->GetProjection(), pWindow->GetView());

	_MouseControl(pWindow, keyboard, mouse);
	_AxisControl(pWindow, camera, mouse);
}

void Selector::Update() {
	if (m_pManipulator->IsVisible()) {
		//const Float newSize = (m_pManipulator->GetPosition() - cameraPosition / 2.f).Length() / 5.f;
		//m_AxisObject.SetSize({ newSize, newSize, newSize });
	}

	if (m_IsChanged) {
		if (!m_SelectedObjects.empty()) {
			m_pManipulator->SetPosition(m_SelectedObjects[0]->GetPosition());
			m_pManipulator->Show();
		}
		else {
			m_pManipulator->Hide();
		}
	}

	m_pManipulator->Update(m_Ray);
}

void Selector::RayCastTest(const Nt::Ray& ray, const Nt::Float2D& cursorPosition, const Bool isMulti) {
	if (m_EnabledDebug) {
		Nt::Shape shape = m_pRayMesh->GetShape();
		shape.Vertices[0].Position.xyz = ray.Start;
		shape.Vertices[1].Position.xyz = ray.End;
		m_pRayMesh->SetShape(shape);
	}

	m_SelectedAxis = m_pManipulator->RayCastTest(ray);
	if (!m_SelectedObjects.empty() && m_SelectedAxis != Axis::NONE) {
		m_pManipulator->BeginEditing(cursorPosition);
		return;
	}

	Object* pNearestObject = m_pScene->RayCastObject(ray);
	if (pNearestObject == nullptr)
		return;

	if (isMulti)
		AddSelect(pNearestObject);
	else
		Select(pNearestObject);
}

void Selector::AddSelect(NotNull<Object*> pObject) {
	m_IsChanged = true;
	auto iterator = std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject);
	if (iterator != m_SelectedObjects.end()) {
		Deselect(iterator);
	}
	else {
		pObject->EnableOutline();
		m_SelectedObjects.push_back(pObject);

		m_pManipulator->Show();
		MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ADD_SELECTION, pObject);
	}
}
void Selector::Select(NotNull<Object*> pObject) {
	const Bool isSelected = pObject->IsSelected();

	AllDeselect();
	if (!isSelected)
		AddSelect(pObject);
}
void Selector::Deselect(NotNull<Object*> pObject) {
	Deselect(std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject));
}
void Selector::Deselect(const ObjectContainer::const_iterator& iterator) {
	if (iterator == m_SelectedObjects.end()) {
		Nt::Log::Instance().Warning(L"This object is not selected.");
		return;
	}

	m_IsChanged = true;

	if (m_SelectedObjects.size() <= 1) {
		m_pManipulator->Hide();
		MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ALL_DESELECT, *iterator);
	}
	else {
		MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_DESELECT, *iterator);
	}

	(*iterator)->DisableOutline();
	m_SelectedObjects.erase(iterator);
}
void Selector::AllDeselect() {
	if (m_SelectedObjects.empty())
		return;

	m_IsChanged = true;
	while (!m_SelectedObjects.empty()) {
		m_SelectedObjects.front()->DisableOutline();
		m_SelectedObjects.erase(m_SelectedObjects.begin());
	}

	m_pManipulator->Hide();
	MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ALL_DESELECT, nullptr);
}

void Selector::RemoveSelected() {
	if (m_SelectedObjects.empty())
		return;

	m_IsChanged = true;
	m_SelectedObjects.clear();

	m_pManipulator->Hide();
	MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ALL_DESELECT, nullptr);
}

void Selector::ToggleSnapToGrid(const Bool& isSnapToGrid) noexcept {
	m_IsSnapToGrid = isSnapToGrid;
}
void Selector::ToggleSnapToGrid() noexcept {
	m_IsSnapToGrid = (!m_IsSnapToGrid);
}

void Selector::UnmarkChanged() noexcept {
	m_IsChanged = false;
}

const ObjectContainer& Selector::GetObjectContainer() const noexcept {
	return m_SelectedObjects;
}
Object* Selector::GetObjectPtr(const uInt& index) const {
	if (index >= m_SelectedObjects.size())
		Raise("Out of range");

	return m_SelectedObjects[index];
}

const Manipulator* Selector::GetManipulator() const noexcept {
	return m_pManipulator.get();
}
const Nt::Mesh* Selector::GetRayMesh() const noexcept {
	return m_pRayMesh.get();
}
uInt Selector::GetObjectCount() const noexcept {
	return m_SelectedObjects.size();
}
Bool Selector::IsContained(const NotNull<Object*> pObject) const {
	auto iterator = std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject);
	return (iterator != m_SelectedObjects.end());
}
Bool Selector::EnabledDebug() const noexcept {
	return m_EnabledDebug;
}
Bool Selector::IsChanged() const noexcept {
	return m_IsChanged;
}
Bool Selector::IsEmpty() const noexcept {
	return m_SelectedObjects.empty();
}

void Selector::SetTransformMode(const Manipulator::State& state) noexcept {
	if (m_pManipulator->IsVisible())
		m_pManipulator->SetState(state);
}

void Selector::_MouseControl(NotNull<const Nt::RenderWindow*> pWindow, Nt::Keyboard& keyboard, Nt::Mouse& mouse) {
	if (!mouse.IsButtonPressed(Nt::BUTTON_LEFT, true))
		return;

	const Nt::Float2D cursorPosition = pWindow->ScreenToClient(mouse.GetCursorPosition());
	if (!pWindow->GetClientRect().Intersect(cursorPosition))
		return;

	if (m_EnabledDebug) {
		Nt::Shape shape = m_pRayMesh->GetShape();
		shape.Vertices[0].Position.xyz = m_Ray.Start;
		shape.Vertices[1].Position.xyz = m_Ray.End;
		m_pRayMesh->SetShape(shape);
	}

	m_SelectedAxis = m_pManipulator->RayCastTest(m_Ray);
	if (!m_SelectedObjects.empty() && m_SelectedAxis != Axis::NONE) {
		m_pManipulator->BeginEditing(cursorPosition);
	}
	else {
		Object* pNearestObject = m_pScene->RayCastObject(m_Ray);
		if (pNearestObject != nullptr) {
			if (keyboard.IsKeyPressed(Nt::KEY_CONTROL, false))
				AddSelect(pNearestObject);
			else
				Select(pNearestObject);
		}
	}
}

void Selector::_AxisControl(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse) {
	if (!m_pManipulator->StartedEditing())
		return;

	m_pManipulator->Control(pWindow, camera, mouse);

	assert(!m_pEventBus.expired());
	auto sharedBus = m_pEventBus.lock();

	Nt::Float3D startPoint = m_pManipulator->GetStartPoint();
	Nt::Float3D moveDelta = m_pManipulator->GetMoveDelta();

	if (m_IsSnapToGrid) {
		moveDelta = m_pGrid->Snap(moveDelta);
		if (moveDelta.LengthSquare() == 0.f)
			return;

		startPoint = m_pGrid->Snap(startPoint);
	}
	else if (moveDelta.LengthSquare() == 0.f) {
		return;
	}

	const Nt::Float3D newPoint = startPoint + moveDelta;
	switch (m_pManipulator->GetState()) {
	case Manipulator::TRANSLATE:
		m_pManipulator->SetPosition(newPoint);
		for (Object* pObject : m_SelectedObjects) {
			pObject->SetPosition(newPoint);
			pObject->StaticUpdate();
		}

		if (m_SelectedObjects.size() == 1) {
			Object* pObject = m_SelectedObjects.front();
			sharedBus->Emmit<UpdateObjectTransformEvent>({
				pObject->GetPosition(), TransforType::POSITION, this });
		}
		break;

	case Manipulator::SCALE:
		for (Object* pObject : m_SelectedObjects) {
			if (m_IsSnapToGrid)
				pObject->SetSize(m_pGrid->Snap(pObject->GetSize() + moveDelta));
			else
				pObject->Scale(moveDelta);
			pObject->StaticUpdate();
		}

		if (m_SelectedObjects.size() == 1) {
			Object* pObject = m_SelectedObjects.front();

			sharedBus->Emmit<UpdateObjectTransformEvent>({
				pObject->GetSize(), TransforType::SIZE, this });
		}
		break;
	}

	m_pManipulator->ResetMoveDelta();
}