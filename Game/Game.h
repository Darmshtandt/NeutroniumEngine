#pragma once

class Game {
public:
	struct Config : public Nt::ISerialization {
		void Write(std::ostream& stream) const override {
			Nt::Serialization::WriteAll(stream, WindowName, ScencePath);
		}
		void Read(std::istream& stream) override {
			Nt::Serialization::ReadAll(stream, WindowName, ScencePath);
		}
		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String WindowName;
		Nt::String ScencePath;
	};

public:
	Game() noexcept :
		m_pWindow(nullptr),
		m_CameraPtr(nullptr),
		m_pLua(nullptr),
		m_pEngineScence(nullptr),
		m_pGameScence(nullptr),
		m_IsInitialized(false),
		m_IsTestGame(false),
		m_IsLaunched(false)
	{ }
	~Game() {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_pAlContext);
		alcCloseDevice(m_pAlDevice);
		SAFE_DELETE(&m_pGameScence);
	}

	void InitializeGame(Nt::RenderWindow* pWindow, const Nt::String& scencePath) {
		if (m_IsInitialized) {
			Nt::Log::Warning("Game already initialized");
			return;
		}

		if (pWindow == nullptr)
			Raise("Window is nullptr");
		if (scencePath == "")
			Raise("Scence path is empty.");

		m_pWindow = pWindow;
		m_pGameScence = new Scence;
		m_pGameScence->Load(scencePath);

		_InitializeOpenAL();
		m_IsInitialized = true;
	}
	void InitializeTestGame(Nt::RenderWindow* pWindow, Scence* pScence) {
		m_IsTestGame = true;

		if (m_IsInitialized) {
			Nt::Log::Warning("Game already initialized");
			return;
		}

		if (pWindow == nullptr)
			Raise("Window is nullptr");
		if (pScence == nullptr)
			Raise("Scence is nullptr");

		m_pWindow = pWindow;
		m_pEngineScence = pScence;

		_InitializeOpenAL();
		m_IsInitialized = true;
	}

	Bool Start() {
		if (m_IsLaunched) {
			WarningBox(L"The game is already running", L"Warning");
			return true;
		}

		if (m_IsTestGame) {
			if (!m_IsInitialized)
				Raise("Game not initialized");

			if (m_pGameScence) {
				WarningBox(L"The game is already running", L"Warning");
				return false;
			}

			m_pGameScence = new Scence(*m_pEngineScence);
		}

		try {
			m_pGameScence->Start();
		}
		catch (const std::exception& except) {
			ErrorBoxA(except.what(), "Error");
			if (m_IsTestGame)
				SAFE_DELETE(&m_pGameScence);
			return false;
		}
		_SetCamera();

		m_IsLaunched = true;

		m_IsThreadTerminated = false;
		m_Thread = std::thread([&]() {
			while (m_IsLaunched) {
				m_pGameScence->Update(m_pWindow->GetFPS() / 1000.f);
				Sleep(1);
			}
			m_IsThreadTerminated = true;
			});
		m_Thread.detach();

		return true;
	}
	void End();

	void Update(const Float& time) {
		if (!m_IsInitialized)
			Raise("Game not initialized");
		
		//m_pGameScence->Update(time);
		m_pWindow->Update();
		if (m_CameraPtr) {
			alListener3f(AL_POSITION, m_CameraPtr->GetPosition().x, m_CameraPtr->GetPosition().y, m_CameraPtr->GetPosition().z);
			Nt::CheckAlErrors("Failed to change listener position.");

			Nt::Float3D up;
			Nt::Float3D forward;
			forward.x = sin(m_CameraPtr->GetAngle().x * RAD);
			forward.y = tan(m_CameraPtr->GetAngle().y * RAD);
			forward.z = cos(m_CameraPtr->GetAngle().z * RAD);
			forward = forward.GetNormalize();

			Nt::Float3D values[2] = { up, forward };
			alListenerfv(AL_ORIENTATION, reinterpret_cast<Float*>(values));
			Nt::CheckAlErrors("Failed to change listener orientation.");
		}
	}
	void Render() const {
		if (!m_IsInitialized)
			Raise("Game not initialized");

		m_pWindow->Clear();
		m_pGameScence->Render(m_pWindow);
		m_pWindow->Display();
	}

	[[nodiscard]]
	const Bool& IsLaunched() const noexcept {
		return m_IsLaunched;
	}

private:
	ALCdevice* m_pAlDevice = nullptr;
	ALCcontext* m_pAlContext = nullptr;

	std::thread m_Thread;
	const Lua* m_pLua;
	Nt::RenderWindow* m_pWindow;
	Nt::Camera m_DefaultCamera;
	GameCamera* m_CameraPtr;
	Scence* m_pEngineScence;
	Scence* m_pGameScence;
	
	Bool m_IsInitialized;
	Bool m_IsTestGame;
	Bool m_IsLaunched;
	Bool m_IsThreadTerminated;

private:
	void _InitializeOpenAL() {
		m_pAlDevice = alcOpenDevice(nullptr);
		if (m_pAlDevice == nullptr)
			Raise(Nt::String("Failed to open audio device.\nOpenAL error code: ") + Nt::String(alGetError()));

		m_pAlContext = alcCreateContext(m_pAlDevice, nullptr);
		if (m_pAlContext == nullptr)
			Raise(Nt::String("Failed to create audio context.\nOpenAL error code: ") + Nt::String(alGetError()));

		if (!alcMakeContextCurrent(m_pAlContext))
			Raise(Nt::String("Failed to make current audio context.\nOpenAL error code: ") + Nt::String(alGetError()));
		
		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		Nt::CheckAlErrors("Failed to change distance model.");
		alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
		Nt::CheckAlErrors("Failed to change listener position.");
		alListener3f(AL_VELOCITY, 1.f, 1.f, 1.f);
		Nt::CheckAlErrors("Failed to change listener velocity.");
		alListenerf(AL_GAIN, 0.5f);
		Nt::CheckAlErrors("Failed to change listener gain.");
	}
	void _SetCamera() {
		if (!m_IsInitialized)
			Raise("Game not initialized");

		try {
			for (Object* pObject : m_pGameScence->GetObjects()) {
				if (pObject == nullptr)
					Raise("Game object is nullptr");

				if (pObject->ObjectType == ObjectTypes::ENTITY) {
					Entity* pEntity = UpcastObjectToEntity(pObject);
					if (pEntity->GetEntityType() == EntityTypes::CAMERA) {
						m_CameraPtr = UpcastEntityToGameCamera(pEntity);
						m_CameraPtr->Set(m_pWindow);
						return;
					}
				}
			}
		}
		catch (const Nt::Error& error) {
			error.Show();
		}
		m_CameraPtr = nullptr;
		m_pWindow->SetCurrentCamera(&m_DefaultCamera);
	}
};