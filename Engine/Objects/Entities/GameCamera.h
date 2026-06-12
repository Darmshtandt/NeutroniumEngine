#pragma once

#include <Nt/Graphics/RenderWindow.h>
#include <Objects/ObjectRegistrar.h>

class GameCamera : public Entity {
public:
	GameCamera(const Nt::String& name);
	GameCamera(const GameCamera& camera);

	void Render(NotNull<Nt::Renderer*> pRenderer) const override;

	void Set(NotNull<Nt::RenderWindow*> windowPtr);

	NT_NODISCARD virtual GameCamera* GetCopy() const override;
	NT_NODISCARD static std::string GetClassToken() noexcept;
	NT_NODISCARD std::string GetToken() const noexcept override;

	void SetOrigin(const Nt::Float3D& origin) override;
	void SetPosition(const Nt::Float3D& position) override;
	void SetAngle(const Nt::Float3D& angle) override;

private:
	using Entity::SetModel;

private:
	Nt::Camera m_Camera;
	Icon3D m_Icon3D;
};