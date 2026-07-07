#pragma once

#include <Editor/Camera3D.h>
#include <Nt/Graphics/RenderWindow.h>
#include <Objects/ObjectRegistrar.h>

class Icon3D;

class GameCamera : public Entity {
public:
	GameCamera(const Nt::String& name);
	GameCamera(const GameCamera& camera);
	~GameCamera() noexcept override = default;

	[[nodiscard]] GameCamera* GetCopy() const override;
	[[nodiscard]] NtEx::Camera3D* GetCamera() noexcept;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

	void SetPosition(const Nt::Float3D& position) override;
	void SetAngle(const Nt::Float3D& angle) override;

private:
	std::unique_ptr<Icon3D> m_Icon3D;
	NtEx::Camera3D m_Camera;
};