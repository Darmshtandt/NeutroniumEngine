#include <Game.h>


void Game::Config::Write(std::ostream& stream) const {
	Nt::Serialization::WriteAll(stream, WindowName, ScenePath);
}

void Game::Config::Read(std::istream& stream) {
	Nt::Serialization::ReadAll(stream, WindowName, ScenePath);
}

Game::~Game() {
	SAFE_DELETE(&m_pGameScene);
}

void Game::InitializeGame(NotNull<Nt::RenderWindow*> pWindow, const Nt::String& scenePath) {
	if (m_IsInitialized) {
		Nt::Log::Instance().Warning("Game already initialized");
		return;
	}

	if (scenePath.empty())
		Raise("Scene path is empty.");

	m_pWindow = pWindow;
	m_pGameScene = new Scene(m_EventBus);
	//m_pGameScene->Load(scenePath);
	assert(0);

	m_IsInitialized = true;
}

void Game::InitializeTestGame(NotNull<Nt::RenderWindow*> pWindow, NotNull<Scene*> pScene) {
	m_IsTestGame = true;

	if (m_IsInitialized) {
		Nt::Log::Instance().Warning("Game already initialized");
		return;
	}

	m_pWindow = pWindow;
	m_pEngineScene = pScene;
	m_IsInitialized = true;
}

Bool Game::Start() {
	if (m_IsLaunched) {
		Nt::MessageWindow("The game is already running", "Warning").ShowWarning();
		return true;
	}

	if (m_IsTestGame) {
		if (!m_IsInitialized)
			Raise("Game not initialized");

		if (m_pGameScene != nullptr) {
			Nt::MessageWindow(L"The game is already running", L"Warning").ShowWarning();
			return false;
		}

		m_pGameScene = new Scene(*m_pEngineScene);
	}

	try {
		m_pGameScene->Start();
	}
	catch (const std::exception& except) {
		Nt::MessageWindow(except.what(), "Error").ShowError();
		if (m_IsTestGame)
			SAFE_DELETE(&m_pGameScene);
		return false;
	}
	_SetCamera();

	m_IsLaunched = true;

	m_IsThreadTerminated = false;
	//m_Thread = std::thread([&]() {
	//	Nt::Timer loopTimeStamp;
	//	Float deltaTime = 0;
	//	while (m_IsLaunched) {
	//		loopTimeStamp.Restart();
	//		m_pGameScene->Update(deltaTime);
	//		deltaTime = Float(loopTimeStamp.GetElapsedTimeMs()) / 1000.f;

	//		Int delay = (1000 / m_pWindow->GetFPSLimit()) - loopTimeStamp.GetElapsedTimeMs();
	//		if (delay < 0)
	//			delay = 1;

	//		Sleep(delay);
	//	}
	//	m_IsThreadTerminated = true;
	//	});
	//m_Thread.detach();

	return true;
}

void Game::End() {
	m_IsLaunched = false;
	while (m_IsThreadTerminated)
		Sleep(10);

	m_pGameScene->Stop();
	SAFE_DELETE(&m_pGameScene);
}

void Game::Update(const Float& time) {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	m_pGameScene->Update(time);
	m_pWindow->Update();

	if (m_CameraPtr != nullptr) {
		m_Listener.SetPosition(m_CameraPtr->GetPosition());
		m_Listener.SetRotation(m_CameraPtr->GetAngle());
	}
}

void Game::Render() const {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	m_pWindow->Clear();
	m_pGameScene->Render(m_pWindow);
	m_pWindow->Display();
}

const Bool& Game::IsLaunched() const noexcept {
	return m_IsLaunched;
}

void Game::_SetCamera() {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	try {
		for (Object* pObject : m_pGameScene->GetObjects()) {
			if (pObject->GetToken() == GameCamera::GetClassToken()) {
				m_CameraPtr = dynamic_cast<GameCamera*>(pObject);
				m_CameraPtr->Set(m_pWindow);
				return;
			}
		}
	}
	catch (const Nt::Error& error) {
		error.Show();
	}

	m_CameraPtr = nullptr;
	m_pWindow->SetCamera(&m_DefaultCamera);
}