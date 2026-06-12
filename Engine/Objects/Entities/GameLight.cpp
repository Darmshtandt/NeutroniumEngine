// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ResourceLoader.h>
#include <Objects/Entities/GameLight.h>


static EntityRegistrar<GameLight> g_Registrar;
static ResourceLoader<Nt::Texture> g_Loader { "Texture.PointLight", "Images\\PointLight.tga" };

GameLight::GameLight(const Nt::String& name) :
	Entity(name, Class<GameLight>::ID()),
	m_Light(Nt::Light::POINT) 
{
	SetModel(Icon3D());
	SetTexture(g_Loader.Get());

	m_Light.SetPosition({ 0.5f, -0.5f, 0.5f });

	DisableCollider();
}

GameLight::GameLight(const GameLight& light) :
	Entity(light),
	m_Light(light.m_Light) 
{
}

void GameLight::Render(NotNull<Nt::Renderer*> pRenderer) const {
	if (!m_IsStarted)
		Entity::Render(pRenderer);
}

_NODISCARD GameLight* GameLight::GetCopy() const {
	return new GameLight(*this);
}

_NODISCARD std::string GameLight::GetClassToken() noexcept {
	return "Light";
}

std::string GameLight::GetToken() const noexcept {
	return GetClassToken();
}

_NODISCARD Nt::LightData GameLight::GetData() const noexcept {
	return m_Light.GetData();
}

void GameLight::SetOrigin(const Nt::Float3D& origin) {
	Entity::SetOrigin(origin);
	m_Light.SetOrigin(-origin);
}

void GameLight::SetPosition(const Nt::Float3D& position) {
	Entity::SetPosition(position);
	m_Light.SetPosition(position);
}

void GameLight::SetAngle(const Nt::Float3D& angle) {
	Entity::SetAngle(angle);
	m_Light.SetAngle(angle);
}