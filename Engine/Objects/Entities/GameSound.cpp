// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Objects/Entities/GameSound.h>
#include <ResourceLoader.h>
#include <Core/Icon3D.h>

static EntityRegistrar<GameSound> g_Registrar;
static ResourceLoader<Nt::Texture> g_Loader { "Texture.Sound", "Images\\Sound.tga" };

GameSound::GameSound(const Nt::String& name) :
	Entity(name, Class<GameSound>::ID()),
	m_IsPlayingAtStart(false),
	m_Icon3D(new Icon3D)
{
	SetMesh(m_Icon3D->GetMesh().Get());
	SetTexture(g_Loader.Get());

	DisableCollider();
}

GameSound::GameSound(const GameSound& sound) :
	Entity(sound),
	m_Sound(sound.m_Sound),
	m_IsPlayingAtStart(sound.m_IsPlayingAtStart),
	m_Icon3D(new Icon3D)
{
	SetMesh(m_Icon3D->GetMesh().Get());
	SetTexture(g_Loader.Get());
}

void GameSound::Start() {
	Object::Start();

	if (m_IsPlayingAtStart)
		m_Sound.Play();
}

void GameSound::Stop() {
	Object::Start();
	m_Sound.Stop();
}

void GameSound::Load(const Nt::String& filePath) {
	m_Sound.LoadFromFile(filePath);
}

void GameSound::Unload() {
	m_Sound.Release();
}

void GameSound::EnablePlayingAtStart() noexcept {
	m_IsPlayingAtStart = true;
}

void GameSound::DisablePlayingAtStart() noexcept {
	m_IsPlayingAtStart = false;
}

void GameSound::Play() {
	m_Sound.Play();
}

void GameSound::ToggleLooping() {
	m_Sound.ToggleLooping();
}

void GameSound::ToggleLooping(const Bool& isLoop) {
	m_Sound.ToggleLooping(isLoop);
}

GameSound* GameSound::GetCopy() const {
	return new GameSound(*this);
}

std::string GameSound::GetClassToken() noexcept {
	return "Sound";
}

std::string GameSound::GetToken() const noexcept {
	return GetClassToken();
}

void GameSound::SetPosition(const Nt::Float3D& position) {
	Object::SetPosition(position);
	m_Sound.SetPosition(position);
}

Bool GameSound::IsPlayingAtStart() const noexcept {
	return m_IsPlayingAtStart;
}

Bool GameSound::IsPlaying() const noexcept {
	return m_Sound.IsPlaying();
}

Bool GameSound::IsLooping() const noexcept {
	return m_Sound.IsLooping();
}

Nt::String GameSound::GetFilePath() const {
	return m_Sound.GetFilePath();
}

Float GameSound::GetRolloffFactor() const noexcept {
	return m_Sound.GetRolloffFactor();
}

Float GameSound::GetReferenceDistance() const noexcept {
	return m_Sound.GetReferenceDistance();
}

Float GameSound::GetMaxDistance() const noexcept {
	return m_Sound.GetMaxDistance();
}

Float GameSound::GetGain() const noexcept {
	return m_Sound.GetGain();
}

void GameSound::SetRolloffFactor(const Float& factor) {
	m_Sound.SetRolloffFactor(factor);
}

void GameSound::SetReferenceDistance(const Float& distance) {
	m_Sound.SetReferenceDistance(distance);
}

void GameSound::SetMaxDistance(const Float& distance) {
	m_Sound.SetMaxDistance(distance);
}

void GameSound::SetGain(const Float& gain) {
	m_Sound.SetGain(gain);
}
