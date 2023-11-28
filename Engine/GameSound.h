#pragma once

class GameSound : public Entity {
public:
	GameSound(const Nt::String& name) : 
		Entity(name, EntityTypes::SOUND),
		m_IsPlayingAtStart(false)
	{
		Nt::Shape doubleSideQuad = Nt::Geometry::Quad({ 1.f, 1.f }, Nt::Colors::White);
		for (uInt i = 0; i < 6; ++i)
			doubleSideQuad.Indices.push_back(doubleSideQuad.Indices[i]);

		std::swap(doubleSideQuad.Indices[1], doubleSideQuad.Indices[4]);
		for (Nt::Vertex& vertex : doubleSideQuad.Vertices)
			std::swap(vertex.Position.x, vertex.Position.z);

		m_Model.SetMesh(doubleSideQuad);

		if (g_TextureSoundIndex != uInt(-1))
			m_Model.SetTexture(g_TextureSoundIndex);
		else
			Nt::Log::Warning("Texture sound index is -1.");

		_UpdateColliders();
		DisableCollider();
	}
	GameSound(const GameSound& sound) :
		Entity(sound),
		m_Sound(sound.m_Sound),
		m_IsPlayingAtStart(sound.m_IsPlayingAtStart)
	{
	}

	void Start() override {
		Object::Start();
		if (m_IsPlayingAtStart)
			m_Sound.Play();
	}
	void Stop() override {
		Object::Start();
		m_Sound.Stop();
	}

	void Write(std::ostream& stream) const override {
		Entity::Write(stream);
		Nt::Serialization::WriteAll(stream, m_Sound.GetFilePath(), m_IsPlayingAtStart, m_Sound.IsLooping(), m_Sound.GetGain(), m_Sound.GetRolloffFactor(), m_Sound.GetReferenceDistance(), m_Sound.GetMaxDistance());
	}
	void Read(std::istream& stream) override {
		Entity::Read(stream);
		
		std::string filePath;
		Nt::Serialization::ReadAll(stream, filePath);
		if (filePath != "")
			Load(filePath.c_str());

		Bool isLooping;
		Float gain;
		Float rolloffFactor;
		Float referenceDistance;
		Float maxDistance;
		Nt::Serialization::ReadAll(stream, m_IsPlayingAtStart, isLooping, gain, rolloffFactor, referenceDistance, maxDistance);
		m_Sound.ToggleLooping(isLooping);
		m_Sound.SetGain(gain);
		m_Sound.SetRolloffFactor(rolloffFactor);
		m_Sound.SetReferenceDistance(referenceDistance);
		m_Sound.SetMaxDistance(maxDistance);
	}
	constexpr uInt Sizeof() const noexcept override {
		return sizeof(*this);
	}

	void Render(Nt::Renderer* pRenderer) const override {
		if (!m_IsStarted)
			Entity::Render(pRenderer);
	}

	void Load(const Nt::String& filePath) {
		m_Sound.LoadFromFile(filePath);
	}
	void Unload() {
		m_Sound.Release();
	}

	void EnablePlaingAtStart() noexcept {
		m_IsPlayingAtStart = true;
	}
	void DisablePlaingAtStart() noexcept {
		m_IsPlayingAtStart = false;
	}

	void Play() {
		m_Sound.Play();
	}

	void ToggleLooping() {
		m_Sound.ToggleLooping();
	}
	void ToggleLooping(const Bool& isLoop) {
		m_Sound.ToggleLooping(isLoop);
	}

	virtual Object* GetCopy() const override {
		return new GameSound(*this);
	}

	void SetPosition(const Nt::Float3D& position) override {
		Object::SetPosition(position);
		m_Sound.SetPosition(position);
	}

	Bool IsPlayingAtStart() const noexcept {
		return m_IsPlayingAtStart;
	}
	Bool IsPlaying() const noexcept {
		return m_Sound.IsPlaying();
	}
	Bool IsLooping() const noexcept {
		return m_Sound.IsLooping();
	}
	Nt::String GetFilePath() const {
		return m_Sound.GetFilePath();
	}
	Float GetRolloffFactor() const noexcept {
		return m_Sound.GetRolloffFactor();
	}
	Float GetReferenceDistance() const noexcept {
		return m_Sound.GetReferenceDistance();
	}
	Float GetMaxDistance() const noexcept {
		return m_Sound.GetMaxDistance();
	}
	Float GetGain() const noexcept {
		return m_Sound.GetGain();
	}

	void SetRolloffFactor(const Float& factor) {
		m_Sound.SetRolloffFactor(factor);
	}
	void SetReferenceDistance(const Float& distance) {
		m_Sound.SetReferenceDistance(distance);
	}
	void SetMaxDistance(const Float& distance) {
		m_Sound.SetMaxDistance(distance);
	}
	void SetGain(const Float& gain) {
		m_Sound.SetGain(gain);
	}

private:
	using Entity::SetModel;

private:
	Nt::Sound m_Sound;
	Bool m_IsPlayingAtStart;
};

__inline GameSound* UpcastEntityToGameSound(Entity* pEntity) {
	if (pEntity == nullptr)
		Raise("Entity pointer is null");
	if (pEntity->GetEntityType() != EntityTypes::SOUND)
		Raise("Entity pointer is not GameSound");

	GameSound* pGameSound = dynamic_cast<GameSound*>(pEntity);
	if (pGameSound == nullptr)
		Raise("Failed to upcast Entity to GameSound.");
	return pGameSound;
}