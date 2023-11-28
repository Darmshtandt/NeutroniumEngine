#pragma once

namespace Nt {
	__inline void CheckAlErrors(const String& errorMessage) {
		Int errorCode = alGetError();
		if (errorCode != AL_NO_ERROR) {
			switch (errorCode) {
			case AL_INVALID_NAME:
				Raise(errorMessage + String("\nOpenAL error: Invalid name."));
			case AL_INVALID_ENUM:
				Raise(errorMessage + String("\nOpenAL error: Invalid enum."));
			case AL_INVALID_VALUE:
				Raise(errorMessage + String("\nOpenAL error: Invalid value."));
			case AL_INVALID_OPERATION:
				Raise(errorMessage + String("\nOpenAL error: Invalid operation."));
			case AL_OUT_OF_MEMORY:
				Raise(errorMessage + String("\nOpenAL error: Out of memory."));
			default:
				Raise(errorMessage + String("\nOpenAL error code: ") + String(errorCode));
			}
		}
	}

	class Sound : public IResource {
	public:
		Sound() : IResource(IResource::TYPE_SOUND)
		{
		}
		Sound(const Sound& sound) :
			IResource(sound),
			m_BufferID(sound.m_BufferID),
			m_SourceID(sound.m_SourceID),
			m_Position(sound.m_Position),
			m_Gain(sound.m_Gain),
			m_RolloffFactor(sound.m_RolloffFactor),
			m_ReferenceDistance(sound.m_ReferenceDistance),
			m_MaxDistance(sound.m_MaxDistance),
			m_IsPlaying(sound.m_IsPlaying)
		{
		}
		~Sound() {
			Stop();
			alDeleteSources(1, &m_SourceID);
		}

		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}

		void Play() {
			if (m_SourceID == 0)
				return;

			if (!m_IsPlaying) {
				alSourcePlay(m_SourceID);
				CheckAlErrors("Failed to play sound.");
				m_IsPlaying = true;
			}
		}
		void Stop() {
			if (m_SourceID == 0)
				return;

			alSourcePause(m_SourceID);
			CheckAlErrors("Failed to stop sound.");
			alSourceRewind(m_SourceID);
			CheckAlErrors("Failed to rewind sound.");

			m_IsPlaying = false;
		}

		void ToggleLooping() {
			ToggleLooping(!m_IsLooping);
		}
		void ToggleLooping(const Bool& isLoop) {
			if (m_SourceID == 0)
				return;

			alSourcei(m_SourceID, AL_LOOPING, isLoop);
			CheckAlErrors("Failed to change position.");
			m_IsLooping = isLoop;
		}

		Float GetRolloffFactor() const noexcept {
			return m_RolloffFactor;
		}
		Float GetReferenceDistance() const noexcept {
			return m_ReferenceDistance;
		}
		Float GetMaxDistance() const noexcept {
			return m_MaxDistance;
		}
		Float GetGain() const noexcept {
			return m_Gain;
		}
		Nt::Float3D GetPosition() const noexcept {
			return m_Position;
		}
		Bool IsPlaying() const noexcept {
			return m_IsPlaying;
		}
		Bool IsLooping() const noexcept {
			return m_IsLooping;
		}

		void SetRolloffFactor(const Float& factor) {
			if (m_SourceID != 0) {
				m_RolloffFactor = factor;
				alSourcef(m_SourceID, AL_ROLLOFF_FACTOR, m_RolloffFactor);
				CheckAlErrors("Failed to change rolloff factor.");
			}
		}
		void SetReferenceDistance(const Float& distance) {
			if (m_SourceID != 0) {
				m_ReferenceDistance = distance;
				alSourcef(m_SourceID, AL_REFERENCE_DISTANCE, m_ReferenceDistance);
				CheckAlErrors("Failed to change reference distance.");
			}
		}
		void SetMaxDistance(const Float& distance) {
			if (m_SourceID != 0) {
				m_MaxDistance = distance;
				alSourcef(m_SourceID, AL_MAX_DISTANCE, m_MaxDistance);
				CheckAlErrors("Failed to change AL_MAX_DISTANCE.");
			}
		}
		void SetGain(const Float& gain) {
			if (m_SourceID != 0) {
				m_Gain = gain;
				alSourcef(m_SourceID, AL_GAIN, m_Gain);
				CheckAlErrors("Failed to change AL_MAX_DISTANCE.");
			}
		}
		void SetPosition(const Float3D& position) {
			if (m_SourceID != 0) {
				m_Position = position;
				alSourcefv(m_SourceID, AL_POSITION, reinterpret_cast<Float*>(&m_Position));
				CheckAlErrors("Failed to change position.");
			}
		}

	private:
		Nt::Float3D m_Position;
		Float m_Gain = 1.f;
		Float m_RolloffFactor = 1.f;
		Float m_ReferenceDistance = 0.f;
		Float m_MaxDistance = 0.f;
		uInt m_BufferID = 0;
		uInt m_SourceID = 0;
		Wave m_Wave;
		Bool m_IsPlaying = false;
		Bool m_IsLooping = false;

	private:
		void _LoadFromFile() override {
			if (m_BufferID != 0) {
				if (m_SourceID != 0) {
					alSourcei(m_SourceID, AL_BUFFER, 0);
					CheckAlErrors("Failed to set buffer in source.");
					alDeleteSources(1, &m_SourceID);
					CheckAlErrors("Failed to delete source.");
				}
				alDeleteBuffers(1, &m_BufferID);
				CheckAlErrors("Failed to delete buffer.");
			}

			m_Wave.LoadFromFile(GetFilePath());

			ALenum format = -1;
			Bool isStereo = (m_Wave.GetHeader().NumChannels > 1);
			switch (m_Wave.GetHeader().BitsPerSample) {
			case 8:
				format = (isStereo) ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
				break;
			case 16:
				format = (isStereo) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
				break;
			}

			alGenBuffers(1, &m_BufferID);
			CheckAlErrors("Failed to create buffer.");

			m_pData = m_Wave.GetData();

			alBufferData(m_BufferID, format, m_pData, m_Wave.GetSize(), m_Wave.GetHeader().SampleRate);
			CheckAlErrors("Failed to set data in buffer.");

			alGenSources(1, &m_SourceID);
			CheckAlErrors("Failed to create source.");

			alSource3f(m_SourceID, AL_DIRECTION, 0.f, 1.f, 0.f);
			CheckAlErrors("Failed to create source.");

			alSourcei(m_SourceID, AL_BUFFER, m_BufferID);
			CheckAlErrors("Failed to set buffer in source.");

			if (m_IsLooping)
				alSourcei(m_SourceID, AL_LOOPING, m_IsLooping);
		}

		void _Release() override {
			m_pData = nullptr;
			m_Wave.Release();
		}
	};
}