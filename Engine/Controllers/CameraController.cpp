#include <Controllers/CameraController.h>
#include <Nt/Graphics/Objects/Camera.h>
#include <Nt/Graphics/System/HandleWindow.h>

#include <algorithm>

CameraController::CameraController(NotNull<Nt::Camera*> pCamera) noexcept :
	m_pCamera(pCamera)
{
}

void CameraController::Update() {
	if (!m_Enabled)
		return;

	m_Keyboard.Update();
	m_Mouse.Update();

	const Float cameraPitch = m_pCamera->GetAngle().y;

	Nt::Float3D move;
	if (m_Keyboard.IsKeyPressed(Nt::KEY_W, false)) {
		move.x += sinf(cameraPitch);
		move.z += cosf(cameraPitch);
	}
	else if (m_Keyboard.IsKeyPressed(Nt::KEY_S, false)) {
		move.x -= sinf(cameraPitch);
		move.z -= cosf(cameraPitch);
	}

	if (m_Keyboard.IsKeyPressed(Nt::KEY_A, false)) {
		move.x += sinf(cameraPitch + PIf * 0.5f);
		move.z += cosf(cameraPitch + PIf * 0.5f);
	}
	else if (m_Keyboard.IsKeyPressed(Nt::KEY_D, false)) {
		move.x += sinf(cameraPitch - PIf * 0.5f);
		move.z += cosf(cameraPitch - PIf * 0.5f);
	}

	if (!m_Keyboard.IsKeyPressed(Nt::KEY_CONTROL, false)) {
		if (m_Keyboard.IsKeyPressed(Nt::KEY_SPACE, false))
			move.y = -1.f;
		else if (m_Keyboard.IsKeyPressed(Nt::KEY_SHIFT, false))
			move.y = 1.f;
	}

	m_pCamera->Translate(move * m_Speed);

	const Nt::Float2D cursorPosition = Nt::GetCursorPosition();
	const Nt::Float2D screenCenter(Nt::GetMonitorSize() / 2);
	const Nt::Float2D angleRotation = ((screenCenter - cursorPosition) / 600.f);
	m_pCamera->Rotate({ angleRotation.y, angleRotation.x, 0.f });
	m_Mouse.SetCursorPosition(screenCenter);
}

void CameraController::Toggle(Bool enabled) noexcept {
	m_Enabled = enabled;
}

void CameraController::SpeedUp() noexcept {
	if (m_Speed != MAX_SPEED)
		m_Speed = std::min(m_Speed * m_Sensitivity, MAX_SPEED);
}

void CameraController::SpeedDown() noexcept {
	if (m_Speed != MIN_SPEED)
		m_Speed = std::max(m_Speed / m_Sensitivity, MIN_SPEED);
}

Float CameraController::GetSpeed() const noexcept {
	return m_Speed;
}