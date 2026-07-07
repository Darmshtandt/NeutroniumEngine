#pragma once

#include <Nt/Light.h>
#include <Objects/ObjectRegistrar.h>

class Icon3D;

class GameLight : public Entity {
public:
	GameLight(const Nt::String& name);
	GameLight(const GameLight& light);
	~GameLight() noexcept override = default;

	[[nodiscard]] GameLight* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

	[[nodiscard]] Nt::LightData GetData() const noexcept;

	void SetPosition(const Nt::Float3D& position) override;
	void SetAngle(const Nt::Float3D& angle) override;

private:
	std::unique_ptr<Icon3D> m_Icon3D;
	Nt::Light m_Light;
};