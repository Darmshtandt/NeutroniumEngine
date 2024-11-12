#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Scene.h>
#include <Grid.h>
#include <Selector.h>


void Selector::Initialize(Scene* pScence, const Nt::String& defaultInitialPath) {
	if (pScence == nullptr)
		Raise("Scence pointer is null");

	m_ScencePtr = pScence;
	m_AxisModel = Nt::Model(defaultInitialPath + "Models\\Axis.obj");
	m_AxisScaleModel = Nt::Model(defaultInitialPath + "Models\\AxisScale.obj");

	m_AxisObject.SetModel(m_AxisModel);
}

void Selector::Control(const Nt::RenderWindow* pWindow, const Nt::Float3D& cameraPosition, const Nt::Float3D& cameraAngle, Nt::Keyboard& keyboard, Nt::Mouse& mouse) {
	if (pWindow == nullptr)
		Raise("Window pointer is null");

	if ((!keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL, false)) && m_SelectedAxis == SelectedAxis::NONE) {
		if (keyboard.IsKeyPressed(Nt::Keyboard::KEY_T, true))
			SetTransformationMode(MODE_TRANSLATE);
		else if (keyboard.IsKeyPressed(Nt::Keyboard::KEY_S, true))
			SetTransformationMode(MODE_SCALE);
	}

	const Nt::Float2D windowSize = Nt::GetClientRect(pWindow->GetHandle()).RightBottom;
	if (mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, false)) {
		if (mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, true)) {
			const Nt::Float2D cursorPositionOnClient = pWindow->ScreenToClient(mouse.GetCursorPosition());

			const Nt::Ray ray = Nt::RayFromPoint2D(cursorPositionOnClient, cameraPosition,
				windowSize, pWindow->GetProjection(), pWindow->GetView());

			if (Nt::FloatRect(Nt::Float2D(), windowSize).Intersect(cursorPositionOnClient)) {
				Int axisFaceIndex = -1;
				if (m_SelectedObjects.size() > 0)
					axisFaceIndex = m_AxisObject.RayCastTest(ray);

				if (axisFaceIndex != -1) {
					Nt::Float3D facePoint = Nt::abs(m_AxisObject.GetColliderPointContainer()[axisFaceIndex * 3]);
					facePoint += Nt::abs(m_AxisObject.GetColliderPointContainer()[axisFaceIndex * 3 + 1]);
					facePoint += Nt::abs(m_AxisObject.GetColliderPointContainer()[axisFaceIndex * 3 + 2]);

					const Bool isSelectedAxisX = (facePoint.x > facePoint.y && facePoint.x > facePoint.z);
					const Bool isSelectedAxisY = (facePoint.y > facePoint.x && facePoint.y > facePoint.z);
					const Bool isSelectedAxisZ = (facePoint.z > facePoint.x && facePoint.z > facePoint.y);

					if (isSelectedAxisX)
						m_SelectedAxis = SelectedAxis::X;
					else if (isSelectedAxisY)
						m_SelectedAxis = SelectedAxis::Y;
					else if (isSelectedAxisZ)
						m_SelectedAxis = SelectedAxis::Z;

					if (m_SelectedAxis != SelectedAxis::NONE) {
						m_StartMovingCursor = pWindow->ScreenToClient(mouse.GetCursorPosition());
						m_TotalMovingOffset = Nt::Float3D();
					}
				}
				else {
					Object* pNearestObject = m_ScencePtr->RayCastObject(ray, new Nt::Float3D());
					if (pNearestObject != nullptr) {
						if (keyboard.IsKeyPressed(Nt::Keyboard::KEY_CONTROL, false))
							AddSelect(pNearestObject);
						else
							Select(pNearestObject);
					}
				}
			}
		}

		if (m_SelectedAxis != SelectedAxis::NONE) {
			const Nt::Float2D cursorPosition = pWindow->ScreenToClient(mouse.GetCursorPosition());
			if (m_StartMovingCursor != cursorPosition) {
				const Nt::Float2D cursorOffset = (cursorPosition - m_StartMovingCursor) / windowSize;

				Nt::Float3D offset;
				switch (m_SelectedAxis) {
				case SelectedAxis::X:
					offset.x = cosf(cameraAngle.y * RADf) * cursorOffset.x + sinf(cameraAngle.y * RADf) * cursorOffset.y;
					break;
				case SelectedAxis::Y:
					offset.y = -cursorOffset.y;
					break;
				case SelectedAxis::Z:
					if (m_TransformationMode == MODE_TRANSLATE)
						offset.z = -sinf(cameraAngle.y * RADf) * cursorOffset.x + cosf(cameraAngle.y * RADf) * cursorOffset.y;
					else
						offset.z = sinf(cameraAngle.y * RADf) * cursorOffset.x - cosf(cameraAngle.y * RADf) * cursorOffset.y;
					break;
				}

				const Float cameraToAxisDistance = (m_AxisObject.GetPosition() - cameraPosition).Length();
				const Float tanFOV = pWindow->GetProjection()._22;
				offset *= cameraToAxisDistance * tanFOV;

				m_TotalMovingOffset += offset;

				Bool isWillBeMoved = true;
				if (m_IsSnapToGrid) {
					isWillBeMoved = (m_TotalMovingOffset.LengthSquare() >= (m_GridCellSize * m_GridCellSize));
					if (isWillBeMoved) {
						m_TotalMovingOffset =
							Nt::Float3D(Nt::Int3D(m_TotalMovingOffset / m_GridCellSize)) * m_GridCellSize;
					}
				}

				if (isWillBeMoved) {
					switch (m_TransformationMode) {
					case MODE_TRANSLATE:
						m_AxisObject.Translate(m_TotalMovingOffset);
						for (Object* pObject : m_SelectedObjects)
							pObject->Translate(m_TotalMovingOffset);
						break;
					case MODE_SCALE:
						for (Object* pObject : m_SelectedObjects)
							pObject->Scale(m_TotalMovingOffset);
						break;
					}

					m_TotalMovingOffset = Nt::Float3D();
				}

				m_StartMovingCursor = cursorPosition;
			}
		}
	}
	else {
		m_SelectedAxis = SelectedAxis::NONE;
	}
}

void Selector::Update(const Nt::Float3D& cameraPosition) {
	if (m_AxisObject.IsRenderEnabled()) {
		const Float newSize = (m_AxisObject.GetPosition() - cameraPosition / 2.f).Length() / 5.f;
		//m_AxisObject.SetSize({ newSize, newSize, newSize });
	}

	if (m_IsChanged) {
		if (!m_SelectedObjects.empty()) {
			m_AxisObject.SetPosition(m_SelectedObjects[0]->GetPosition());
			m_AxisObject.EnableRender();
		}
		else {
			m_AxisObject.DisableRender();
		}
	}
}
void Selector::Render(Nt::RenderWindow* pWindow) {
	pWindow->DisableDepthBuffer();
	m_AxisObject.Render(pWindow);
	pWindow->EnableDepthBuffer();
}

void Selector::AddSelect(Object* pObject) {
	if (pObject) {
		m_IsChanged = true;
		auto it = std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject);
		if (it != m_SelectedObjects.end()) {
			pObject->DisableSelectionColor();
			m_SelectedObjects.erase(it);
		}
		else {
			pObject->EnableSelectionColor();
			m_SelectedObjects.push_back(pObject);
		}
	}
	else {
		ERROR_MSG(L"Selector::AddSelect: Object is nullptr.", L"Error");
	}
}
void Selector::Select(Object* pObject) {
	const Bool isObjectSelected = pObject->IsSelected();
	const uInt objectCount = m_SelectedObjects.size();

	AllDeselect();
	if ((!isObjectSelected) || objectCount > 1)
		AddSelect(pObject);
}
void Selector::Deselect(Object* pObject) {
	auto objectIterator =
		std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject);
	if (objectIterator != m_SelectedObjects.end()) {
		m_IsChanged = true;
		(*objectIterator)->DisableSelectionColor();
		m_SelectedObjects.erase(objectIterator);
	}
	else {
		Nt::Log::Warning(L"This object is not selected.");
	}
}
void Selector::AllDeselect() {
	m_IsChanged = true;
	while (m_SelectedObjects.size() > 0) {
		m_SelectedObjects[0]->DisableSelectionColor();
		m_SelectedObjects.erase(m_SelectedObjects.begin());
	}
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

const ObjectContainer& Selector::GetObjectContaiter() const noexcept {
	return m_SelectedObjects;
}
Object* Selector::GetObjectPtr(const uInt& index) const noexcept {
	if (index >= m_SelectedObjects.size()) {
		Raise("Out of range");
		return nullptr;
	}

	return m_SelectedObjects[index];
}

uInt Selector::GetObjectCount() const noexcept {
	return m_SelectedObjects.size();
}
Bool Selector::IsContained(const Object* pObject) const {
	auto iterator = std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), pObject);
	return (iterator != m_SelectedObjects.end());
}
Bool Selector::IsChanged() const noexcept {
	return m_IsChanged;
}
Bool Selector::IsEmpty() const noexcept {
	return (m_SelectedObjects.size() == 0);
}

void Selector::SetGridCellSize(const Float& gridCellSize) noexcept {
	if (m_GridCellSize != gridCellSize)
		m_GridCellSize = gridCellSize;
}
void Selector::SetTransformationMode(const TransformationMode& mode) noexcept {
	if (m_TransformationMode != mode) {
		switch (mode) {
		case MODE_TRANSLATE:
			m_AxisObject.SetModel(m_AxisModel);
			break;
		case MODE_SCALE:
			m_AxisObject.SetModel(m_AxisScaleModel);
			break;
		}
		m_TransformationMode = mode;
	}
}