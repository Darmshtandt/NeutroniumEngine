#pragma once

#include <Nt/Light.h>
#include <Objects/ObjectRegistrar.h>

class GameLight : public Entity {
public:
	GameLight(const Nt::String& name);
	GameLight(const GameLight& light);

	void Render(NotNull<Nt::Renderer*> pRenderer) const override;

	_NODISCARD virtual GameLight* GetCopy() const override;
	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

	_NODISCARD Nt::LightData GetData() const noexcept;

	void SetOrigin(const Nt::Float3D& origin) override;
	void SetPosition(const Nt::Float3D& position) override;
	void SetAngle(const Nt::Float3D& angle) override;

private:
	using Entity::SetModel;

private:
	Nt::Light m_Light;
};