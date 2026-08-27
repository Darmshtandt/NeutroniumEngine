#pragma once

#include <IController.h>
#include <Nt/Core/Input.h>
#include <Nt/Core/Utilities.h>

namespace NtEx {
	class Camera3D;
}

class CameraController : public IController {
public:
	static constexpr Float MAX_SPEED = 1.f;
	static constexpr Float MIN_SPEED = 0.001f;

public:
	explicit CameraController(NotNull<NtEx::Camera3D*> pCamera) noexcept;

	void Update() override;
	void Toggle(Bool enabled) noexcept override;

	void SpeedUp() noexcept;
	void SpeedDown() noexcept;

	[[nodiscard]] Float GetSpeed() const noexcept;

private:
	NtEx::Camera3D* m_pCamera;
	Nt::Keyboard m_Keyboard;
	Nt::Mouse m_Mouse;
	Float m_Sensitivity = 1.25f;
	Float m_Speed = 0.05f;
	Bool m_Enabled = true;
};
