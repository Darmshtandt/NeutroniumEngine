// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ResourceLoader.h>
#include <Objects/Entities/GameCamera.h>

#include <Core/Icon3D.h>

static EntityRegistrar<GameCamera> g_Registrar;
static ResourceLoader<Nt::Texture> g_Loader { "Texture.Camera", "Images\\Camera.tga" };

GameCamera::GameCamera(const Nt::String& name) :
	Entity(name, Class<GameCamera>::ID()),
	m_Icon3D(new Icon3D)
{
	SetMesh(m_Icon3D->GetMesh().Get());
	SetTexture(g_Loader.Get());

	m_Camera.SetPosition({ 0.5f, -0.5f, 0.5f });

	DisableCollider();
}

GameCamera::GameCamera(const GameCamera& camera) :
	Entity(camera),
	m_Camera(camera.m_Camera),
	m_Icon3D(new Icon3D)
{
	SetMesh(m_Icon3D->GetMesh().Get());
	SetTexture(g_Loader.Get());
}

void GameCamera::Set(NotNull<Nt::RenderWindow*> windowPtr) {
	windowPtr->SetCamera(&m_Camera);
}

GameCamera* GameCamera::GetCopy() const {
	return new GameCamera(*this);
}

std::string GameCamera::GetClassToken() noexcept {
	return "Camera";
}

std::string GameCamera::GetToken() const noexcept {
	return GetClassToken();
}

void GameCamera::SetOrigin(const Nt::Float3D& origin) {
	Entity::SetOrigin(origin);
	m_Camera.SetOrigin(-origin);
}

void GameCamera::SetPosition(const Nt::Float3D& position) {
	Entity::SetPosition(position);
	m_Camera.SetPosition(-position + Nt::Float3D(0.5f, -0.5f, 0.5f));
}

void GameCamera::SetAngle(const Nt::Float3D& angle) {
	Entity::SetAngle(angle);
	m_Camera.SetAngle(-angle);
}