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
#include <Editor/EditingHistory.h>
#include <Editor/Commands/TransformCommands.h>
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
			m_pManipulator->SetPosition(m_SelectedObjects[0].lock()->GetPosition());
			m_pManipulator->Show();
		}
		else {
			m_pManipulator->Hide();
		}
	}

	m_pManipulator->Update(m_Ray);
}

void Selector::RayCastTest(const Nt::Ray& ray, const Nt::Float2D& cursorPosition, const Bool isMulti) {
	//if (m_EnabledDebug) {
	//	Nt::Shape shape = m_pRayMesh->GetShape();
	//	shape.Vertices[0].Position.xyz = ray.Start;
	//	shape.Vertices[1].Position.xyz = ray.End;
	//	m_pRayMesh->SetShape(shape);
	//}

	//m_SelectedAxis = m_pManipulator->RayCastTest(ray);
	//if (!m_SelectedObjects.empty() && m_SelectedAxis != Axis::NONE) {
	//	m_pManipulator->BeginEditing(cursorPosition);
	//	return;
	//}

	//Object* pNearestObject = m_pScene->RayCastObject(ray);
	//if (pNearestObject == nullptr)
	//	return;

	//if (isMulti)
	//	AddSelect(pNearestObject);
	//else
	//	Select(pNearestObject);
}

void Selector::AddSelect(const WeakObjectPtr& pObject) {
	Assert(!pObject.expired(), "Object pointer is expired");

	m_IsChanged = true;
	auto iterator = FindObject(m_SelectedObjects, pObject);
	if (iterator != m_SelectedObjects.end()) {
		Deselect(iterator);
		return;
	}

	const auto& sharedObject = pObject.lock();
	sharedObject->EnableOutline();
	m_SelectedObjects.emplace_back(pObject);

	m_pManipulator->Show();
	MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ADD_SELECTION, sharedObject.get());
}
void Selector::Select(const WeakObjectPtr& weakObject) {
	if (auto object = weakObject.lock()) {
		const Bool isSelected = object->IsSelected();

		AllDeselect();
		if (!isSelected)
			AddSelect(object);
	}
}
void Selector::Deselect(const WeakObjectPtr& weakObject) {
	auto iterator = FindObject(m_SelectedObjects, weakObject);
	Deselect(iterator);
}
void Selector::Deselect(const WeakObjectContainer::const_iterator& iterator) {
	if (iterator == m_SelectedObjects.end()) {
		Nt::Log::Instance().Warning(L"This object is not selected.");
		return;
	}
	if (iterator->expired()) {
		m_SelectedObjects.erase(iterator);
		return;
	}

	const auto& object = iterator->lock();

	m_IsChanged = true;

	if (m_SelectedObjects.size() <= 1) {
		m_pManipulator->Hide();
		MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_ALL_DESELECT, object.get());
	}
	else {
		MessageBus<Object*>::Instance().Publish(TOPIC_SELECTOR_DESELECT, object.get());
	}

	object->DisableOutline();
	m_SelectedObjects.erase(iterator);
}
void Selector::AllDeselect() {
	if (m_SelectedObjects.empty())
		return;

	m_IsChanged = true;
	while (!m_SelectedObjects.empty()) {
		if (const auto& object = m_SelectedObjects.front().lock())
			object->DisableOutline();
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

const WeakObjectContainer& Selector::GetObjectContainer() const noexcept {
	return m_SelectedObjects;
}
const WeakObjectPtr& Selector::GetObjectPtr(const uInt& index) const {
	if (index >= m_SelectedObjects.size())
		Raise("Out of range");
	assert(!m_SelectedObjects[index].expired());
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
Bool Selector::IsContained(const WeakObjectPtr& pObject) const {
	assert(!pObject.expired());

	auto iterator = FindObject(m_SelectedObjects, pObject);
	return iterator != m_SelectedObjects.end();
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
		ObjectPtr pNearestObject = m_pScene->RayCastObject(m_Ray);
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

	auto sharedBus = m_pEventBus.lock();
	assert(sharedBus);

	if (!mouse.IsButtonPressed(Nt::BUTTON_LEFT, false)) {
		Nt::Float3D startPoint = m_pManipulator->GetStartPoint();
		Nt::Float3D endPoint = m_pManipulator->GetPosition();
		if (m_IsSnapToGrid) {
			startPoint = m_pGrid->Snap(startPoint);
			endPoint = m_pGrid->Snap(endPoint);
		}

		if (startPoint == endPoint) {
			m_pManipulator->EndEditing();
			return;
		}
		
		switch (m_pManipulator->GetState()) {
		case Manipulator::TRANSLATE:
			for (auto it = m_SelectedObjects.cbegin(); it != m_SelectedObjects.cend(); ++it) {
				sharedBus->Emmit<AddToHistoryCommand>({
					new Edit::MoveCommand(it->lock(), endPoint, startPoint) });
			}
			break;

		case Manipulator::SCALE:
			const Nt::Float3D delta = endPoint - startPoint;

			for (auto it = m_SelectedObjects.cbegin(); it != m_SelectedObjects.cend(); ++it) {
				const auto object = it->lock();
				const Nt::Float3D scale = object->GetSize();

				sharedBus->Emmit<AddToHistoryCommand>({
					new Edit::SizeCommand(object, scale, scale - delta) });
			}
			break;
		}

		m_pManipulator->EndEditing();
		return;
	}

	m_pManipulator->Control(pWindow, camera, mouse);

	Nt::Float3D axisPoint = m_pManipulator->GetPosition();
	Nt::Float3D moveDelta = m_pManipulator->GetLocalMoveDelta();

	if (m_IsSnapToGrid) {
		moveDelta = m_pGrid->Snap(moveDelta);
		if (moveDelta.LengthSquare() == 0.f)
			return;

		axisPoint = m_pGrid->Snap(axisPoint);
	}
	else if (moveDelta.LengthSquare() == 0.f) {
		return;
	}

	switch (m_pManipulator->GetState()) {
	case Manipulator::TRANSLATE: {
		for (auto it = m_SelectedObjects.cbegin(); it != m_SelectedObjects.cend();) {
			if (it->expired()) {
				it = m_SelectedObjects.erase(it);
				continue;
			}

			const auto object = it->lock();
			object->SetPosition(axisPoint);
			object->StaticUpdate();

			++it;
		}

		if (m_SelectedObjects.size() == 1) {
			const auto& object = m_SelectedObjects.front().lock();

			sharedBus->Emmit<UpdateObjectTransformEvent>({
				object->GetPosition(), TransforType::POSITION, this });
		}
	}
		break;

	case Manipulator::SCALE:
		for (auto it = m_SelectedObjects.cbegin(); it != m_SelectedObjects.cend();) {
			if (it->expired()) {
				it = m_SelectedObjects.erase(it);
				continue;
			}
			const auto& object = it->lock();

			const Nt::Float3D size = object->GetSize() + moveDelta;
			if (m_IsSnapToGrid)
				object->SetSize(m_pGrid->Snap(size));
			else
				object->SetSize(size);
			object->StaticUpdate();

			++it;
		}

		if (m_SelectedObjects.size() == 1) {
			const auto& object = m_SelectedObjects.front().lock();

			sharedBus->Emmit<UpdateObjectTransformEvent>({
				object->GetSize(), TransforType::SIZE, this });
		}
		break;
	}

	m_pManipulator->ResetLocalMoveDelta();
}