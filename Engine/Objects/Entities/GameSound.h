#pragma once

#include <Nt/Graphics/Sound/Sound.h>
#include <Objects/ObjectRegistrar.h>

class GameSound : public Entity {
public:
	GameSound(const Nt::String& name);
	GameSound(const GameSound& sound);

	void Start() override;
	void Stop() override;

	void Render(NotNull<Nt::Renderer*> pRenderer) const override;

	void Load(const Nt::String& filePath);
	void Unload();

	void EnablePlayingAtStart() noexcept;
	void DisablePlayingAtStart() noexcept;

	void Play();

	void ToggleLooping();
	void ToggleLooping(const Bool& isLoop);

	_NODISCARD virtual GameSound* GetCopy() const override;
	_NODISCARD static std::string GetClassToken() noexcept;
	_NODISCARD std::string GetToken() const noexcept override;

	void SetPosition(const Nt::Float3D& position) override;

	_NODISCARD Bool IsPlayingAtStart() const noexcept;
	_NODISCARD Bool IsPlaying() const noexcept;
	_NODISCARD Bool IsLooping() const noexcept;
	_NODISCARD Nt::String GetFilePath() const;
	_NODISCARD Float GetRolloffFactor() const noexcept;
	_NODISCARD Float GetReferenceDistance() const noexcept;
	_NODISCARD Float GetMaxDistance() const noexcept;
	_NODISCARD Float GetGain() const noexcept;

	void SetRolloffFactor(const Float& factor);
	void SetReferenceDistance(const Float& distance);
	void SetMaxDistance(const Float& distance);
	void SetGain(const Float& gain);

private:
	using Entity::SetModel;

private:
	Nt::Sound m_Sound;
	Icon3D m_Icon3D;
	Bool m_IsPlayingAtStart;
};