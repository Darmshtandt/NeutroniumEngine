// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ResourceLoader.h>
#include <Core/Manipulator.h>

#include <Editor/Camera3D.h>

static ResourceLoader<Nt::Mesh> g_TranslateLoader { "Mesh.Translate", "Models\\TranslateArrow.obj" };
static ResourceLoader<Nt::Mesh> g_ScaleLoader { "Mesh.Scale", "Models\\ScaleArrow.obj" };

Manipulator::Arrow::Arrow(const Axis& axis) :
	Object("", Class<Arrow>::ID()),
	m_LineMesh(new Nt::Mesh(Nt::Primitive::Line(10000.f, Nt::Colors::White)))
{
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
Manipulator::Arrow::~Arrow() noexcept = default;

void Manipulator::Arrow::Select() noexcept {
	m_ShowingLine = true;
	ToggleOutline(m_ShowingLine);
}

void Manipulator::Arrow::Deselect() noexcept {
	m_ShowingLine = false;
	ToggleOutline(m_ShowingLine);
}

const Nt::Mesh* Manipulator::Arrow::GetLineMesh() const noexcept {
	return m_LineMesh.get();
}
Bool Manipulator::Arrow::IsShowedLine() const noexcept {
	return m_ShowingLine;
}

Manipulator::Manipulator() {
	for (uInt i = 0; i < 3; ++i)
		m_AxisArrows[i].reset(new Arrow(static_cast<Axis>(i)));

	m_StateMeshes.push_back(g_TranslateLoader.Get());
	m_StateMeshes.push_back(g_ScaleLoader.Get());

	for (const auto& arrow : m_AxisArrows)
		arrow->SetMesh(m_StateMeshes[m_State]);
}

void Manipulator::Control(NotNull<const Nt::RenderWindow*> pWindow, const NtEx::Camera3D& camera, Nt::Mouse& mouse) {
	const Nt::Float2D& cursorPosition = pWindow->ScreenToClient(mouse.GetCursorPosition());
	if (!m_StartedEditing || m_StartCursorPosition == cursorPosition)
		return;

	Nt::Ray ray = Nt::RayFromPoint2D(cursorPosition, -camera.Position(),
		pWindow->GetClientSize(), pWindow->GetProjection(), pWindow->GetView());
	CalcMoveDelta(ray);

	m_StartCursorPosition = cursorPosition;
}

void Manipulator::Update(const Nt::Ray& ray) {
	if (!m_StartedEditing) {
		if (m_SelectedAxis != NONE)
			m_AxisArrows[m_SelectedAxis]->Deselect();

		m_SelectedAxis = RayCastTest(ray);
		if (m_SelectedAxis != NONE)
			m_AxisArrows[m_SelectedAxis]->Select();
	}

	for (const auto& arrow : m_AxisArrows)
		arrow->Update(1.f);
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
	const Nt::Float3D newAxisPoint = m_Position + axisDirection * t;
	const Nt::Float3D delta = newAxisPoint - m_Position;

	m_LocalMoveDelta += delta;
	m_MoveDelta += delta;
	SetPosition(newAxisPoint);

	return m_MoveDelta;
}

void Manipulator::EndEditing() noexcept {
	if (m_StartedEditing)
		m_StartedEditing = false;
}

void Manipulator::ResetLocalMoveDelta() noexcept {
	m_LocalMoveDelta = { };
}

Manipulator::Axis Manipulator::RayCastTest(const Nt::Ray& ray) {
	for (uInt i = 0; i < 3; ++i) {
		if (m_AxisArrows[i]->RayCastTest(ray, &m_RayCastPoint) != -1)
			return static_cast<Axis>(i);
	}
	return NONE;
}

const Manipulator::Arrow* Manipulator::GetArrow(uInt axis) const noexcept {
	Assert(axis < 3, "Out or range");
	return m_AxisArrows[axis].get();
}

Nt::Float3D Manipulator::GetPosition() const noexcept {
	return m_Position;
}
Nt::Float3D Manipulator::GetLocalMoveDelta() const noexcept {
	return m_LocalMoveDelta;
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
	for (const auto& arrow : m_AxisArrows)
		arrow->SetMesh(m_StateMeshes[m_State]);
}

void Manipulator::SetPosition(const Nt::Float3D& position) noexcept {
	if (m_Position == position)
		return;

	m_Position = position;
	for (const auto& arrow : m_AxisArrows)
		arrow->SetPosition(m_Position);
}