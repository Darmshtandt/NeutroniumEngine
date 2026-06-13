#pragma once

#include <Nt/Graphics/Sound/Sound.h>
#include <Objects/ObjectRegistrar.h>

class Icon3D;

class GameSound : public Entity {
public:
	GameSound(const Nt::String& name);
	GameSound(const GameSound& sound);
	~GameSound() noexcept override = default;

	void Start() override;
	void Stop() override;

	void Load(const Nt::String& filePath);
	void Unload();

	void EnablePlayingAtStart() noexcept;
	void DisablePlayingAtStart() noexcept;

	void Play();

	void ToggleLooping();
	void ToggleLooping(const Bool& isLoop);

	[[nodiscard]] virtual GameSound* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;

	void SetPosition(const Nt::Float3D& position) override;

	[[nodiscard]] Bool IsPlayingAtStart() const noexcept;
	[[nodiscard]] Bool IsPlaying() const noexcept;
	[[nodiscard]] Bool IsLooping() const noexcept;
	[[nodiscard]] Nt::String GetFilePath() const;
	[[nodiscard]] Float GetRolloffFactor() const noexcept;
	[[nodiscard]] Float GetReferenceDistance() const noexcept;
	[[nodiscard]] Float GetMaxDistance() const noexcept;
	[[nodiscard]] Float GetGain() const noexcept;

	void SetRolloffFactor(const Float& factor);
	void SetReferenceDistance(const Float& distance);
	void SetMaxDistance(const Float& distance);
	void SetGain(const Float& gain);

private:
	std::unique_ptr<Icon3D> m_Icon3D;
	Nt::Sound m_Sound;
	Bool m_IsPlayingAtStart;
};