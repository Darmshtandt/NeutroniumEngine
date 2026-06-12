// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ResourceLoader.h>
#include <Core/Manipulator.h>

static ResourceLoader<Nt::Mesh> g_TranslateLoader { "Mesh.Translate", "Models\\TranslateArrow.obj" };
static ResourceLoader<Nt::Mesh> g_ScaleLoader { "Mesh.Scale", "Models\\ScaleArrow.obj" };

Manipulator::Arrow::Arrow(const Axis& axis) : Object("", Class<Arrow>::ID()) {
	m_AxisLine.SetMeshByPtr(&m_AxisLineMesh);
	SetDrawingMode(Nt::Renderer::DrawingMode::LINES);

	constexpr Float HalfPIf = PIf / 2.f;

	switch (axis) {
	case X:
		SetAngle({ HalfPIf, 0.f, 0.f });
		SetColor(Nt::Colors::Red);
		break;

	case Y:
		SetAngle({ 0.f, 0.f, -HalfPIf });
		SetColor(Nt::Colors::Green);
		break;

	case Z:
		SetAngle({ 0.f, HalfPIf, 0.f });
		SetColor(Nt::Colors::Blue);
		break;
	}
}
Manipulator::Arrow::~Arrow() noexcept {
	m_AxisLine.SetMeshByPtr(nullptr);
}

void Manipulator::Arrow::Render(NotNull<Nt::Renderer*> pRenderer) const noexcept {
	if (m_ShowingLine) {
		const Nt::Renderer::DrawingMode drawingMode = pRenderer->GetDrawingMode();

		pRenderer->SetDrawingMode(Nt::Renderer::DrawingMode::LINES);
		m_AxisLine.Render(pRenderer);
		pRenderer->SetDrawingMode(drawingMode);
	}

	Object::Render(pRenderer);
}

void Manipulator::Arrow::Select() noexcept {
	EnableOutline();
	m_ShowingLine = true;
}

void Manipulator::Arrow::Deselect() noexcept {
	DisableOutline();
	m_ShowingLine = false;
}

void Manipulator::Arrow::SetPosition(const Nt::Float3D& position) {
	Object::SetPosition(position);
	m_AxisLine.SetPosition(position);
}

void Manipulator::Arrow::SetColor(const Nt::Float4D& color) {
	Object::SetColor(color);
	m_AxisLine.SetColor(color);
}

void Manipulator::Arrow::SetAngle(const Nt::Float3D& angle) {
	Object::SetAngle(angle);
	m_AxisLine.SetAngle(angle);
}

Manipulator::Manipulator() :
	m_AxisArrows { Arrow(X), Arrow(Y), Arrow(Z) }
{
	m_StateMeshes.push_back(g_TranslateLoader.Get());
	m_StateMeshes.push_back(g_ScaleLoader.Get());

	for (Arrow& arrow : m_AxisArrows)
		arrow.SetMesh(m_StateMeshes[m_State]);
}

void Manipulator::Control(NotNull<const Nt::RenderWindow*> pWindow, const Nt::Camera& camera, Nt::Mouse& mouse) {
	const Nt::Float2D& cursorPosition = pWindow->ScreenToClient(mouse.GetCursorPosition());
	if (!m_StartedEditing || m_StartCursorPosition == cursorPosition)
		return;

	if (!mouse.IsButtonPressed(Nt::BUTTON_LEFT, false)) {
		EndEditing();
		return;
	}

#if 0
	const Nt::Float2D cursorOffset = (m_StartCursorPosition - cursorPosition) / pWindow->GetClientSize();
	const Float cameraPitch = camera.GetAngle().y;

	Nt::Float3D offset;
	switch (m_SelectedAxis) {
	case X:
		offset.x = -(cosf(cameraPitch) * cursorOffset.x + sinf(cameraPitch) * cursorOffset.y);
		break;
	case Y:
		offset.y = cursorOffset.y;
		break;
	case Z:
		offset.z = sinf(cameraPitch) * cursorOffset.x - cosf(cameraPitch) * cursorOffset.y;
		break;
	}

	const Float cameraToAxisDistance = (m_Position - camera.GetPosition()).Length();
	const Float tanFOV = pWindow->GetProjection()._22;

	m_MoveDelta += offset * cameraToAxisDistance * tanFOV;
#else
	Nt::Ray ray = Nt::RayFromPoint2D(cursorPosition, -camera.GetPosition(),
		pWindow->GetClientSize(), pWindow->GetProjection(), pWindow->GetView());
	CalcMoveDelta(ray);
#endif

	m_StartCursorPosition = cursorPosition;
}

void Manipulator::Update(const Nt::Ray& ray) {
	if (!m_StartedEditing) {
		if (m_SelectedAxis != NONE)
			m_AxisArrows[m_SelectedAxis].Deselect();

		m_SelectedAxis = RayCastTest(ray);
		if (m_SelectedAxis != NONE)
			m_AxisArrows[m_SelectedAxis].Select();
	}

	for (Arrow& arrow : m_AxisArrows)
		arrow.Update(1.f);
}

void Manipulator::Render(NotNull<Nt::Renderer*> pRenderer) const noexcept {
	if (!m_IsVisible)
		return;

	for (const Arrow& arrow : m_AxisArrows)
		arrow.Render(pRenderer);
}

void Manipulator::Translate(const Nt::Float3D& offset) noexcept {
	if (offset.LengthSquare() != 0.f)
		SetPosition(m_Position + offset);
}

void Manipulator::Show() noexcept {
	if (!m_IsVisible)
		m_IsVisible = true;
}

void Manipulator::Hide() noexcept {
	if (m_IsVisible)
		m_IsVisible = false;
}

void Manipulator::BeginEditing(const Nt::Float2D& cursorPosition) noexcept {
	if (m_StartedEditing || m_SelectedAxis == NONE)
		return;

	m_StartCursorPosition = cursorPosition;
	m_MoveDelta = Nt::Float3D();
	m_StartPoint = m_Position;

	m_StartedEditing = true;
}

Nt::Float3D Manipulator::CalcMoveDelta(const Nt::Ray& ray) {
	assert(m_SelectedAxis != NONE);
	if (m_SelectedAxis == NONE)
		return { };

	Nt::Float3D axisDirection;
	axisDirection[m_SelectedAxis] = 1.f;

	const Nt::Float3D rayDirection = ray.Direction();
	const Nt::Float3D w0 = ray.Start - m_Position;

	const Float a = rayDirection.Dot(rayDirection);
	const Float b = rayDirection.Dot(axisDirection);
	const Float c = axisDirection.Dot(axisDirection);
	const Float d = rayDirection.Dot(w0);
	const Float e = axisDirection.Dot(w0);

	const Float denom = a * c - b * b;
	if (std::fabsf(denom) < FLT_EPSILON)
		return m_MoveDelta;

	const Float t = (a * e - b * d) / denom;
	const Nt::Float3D axisPoint = m_Position + axisDirection * t;

	m_MoveDelta += axisPoint - m_StartPoint;
	m_StartPoint = axisPoint;

	return m_MoveDelta;
}

void Manipulator::EndEditing() noexcept {
	if (m_StartedEditing)
		m_StartedEditing = false;
}

void Manipulator::ResetMoveDelta() noexcept {
	m_MoveDelta = { };
}

Manipulator::Axis Manipulator::RayCastTest(const Nt::Ray& ray) {
	for (uInt i = 0; i < 3; ++i) {
		if (m_AxisArrows[i].RayCastTest(ray, &m_RayCastPoint) != -1)
			return static_cast<Axis>(i);
	}
	return NONE;
}

Nt::Float3D Manipulator::GetPosition() const noexcept {
	return m_Position;
}

Nt::Float3D Manipulator::GetMoveDelta() const noexcept {
	return m_MoveDelta;
}
Nt::Float3D Manipulator::GetStartPoint() const noexcept {
	return m_StartPoint;
}

Manipulator::State Manipulator::GetState() const noexcept {
	return m_State;
}
Bool Manipulator::IsVisible() const noexcept {
	return m_IsVisible;
}
Bool Manipulator::StartedEditing() const noexcept {
	return m_StartedEditing;
}

void Manipulator::SetState(const State& state) noexcept {
	if (m_State == state)
		return;

	m_State = state;

	for (Arrow& arrow : m_AxisArrows)
		arrow.SetMesh(m_StateMeshes[m_State]);
}

void Manipulator::SetPosition(const Nt::Float3D& position) noexcept {
	if (m_Position == position)
		return;

	m_Position = position;
	for (Arrow& arrow : m_AxisArrows)
		arrow.SetPosition(m_Position);
}

