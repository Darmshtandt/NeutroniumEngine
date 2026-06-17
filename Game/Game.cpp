#include <Game.h>
#include <RenderEngine.h>


void Game::Config::Write(std::ostream& stream) const {
	Nt::Serialization::WriteAll(stream, WindowName, ScenePath);
}

void Game::Config::Read(std::istream& stream) {
	Nt::Serialization::ReadAll(stream, WindowName, ScenePath);
}

Game::Game(Nt::RenderWindow* pWindow) noexcept :
	m_pWindow(pWindow)
{
	auto shader = new Nt::Shader;
	shader->Initialize();
	shader->Create();
	shader->CompileFromFile(Nt::Shader::VERTEX, "..\\Shaders\\Vert.glsl");
	shader->CompileFromFile(Nt::Shader::FRAGMENT, "..\\Shaders\\Frag.glsl");
	shader->Link();

	shader->DisableStrict();
	shader->SetUniform<Bool>("IsObjectSelected", false);
	shader->SetUniform<Bool>("IsLightsEnabled", true);

	m_RenderEngine.reset(new RenderEngine(pWindow, shader));
}

Game::~Game() = default;

void Game::InitializeGame(const Nt::String& scenePath) {
	if (m_IsInitialized) {
		Nt::Log::Instance().Warning("Game already initialized");
		return;
	}

	if (scenePath.empty())
		Raise("Scene path is empty.");

	m_GameScene.reset(new Scene(m_EventBus));
	//m_pGameScene->Load(scenePath);
	m_RenderEngine->SetScene(m_GameScene.get());

	assert(0);

	m_IsInitialized = true;
}

void Game::InitializeTestGame(NotNull<Scene*> pScene) {
	m_IsTestGame = true;

	if (m_IsInitialized) {
		Nt::Log::Instance().Warning("Game already initialized");
		return;
	}

	m_pEngineScene = pScene;
	m_IsInitialized = true;
}

Bool Game::Start() {
	if (m_IsLaunched) {
		Nt::Log::Instance().Warning("The game is already running");
		Nt::MessageWindow("The game is already running", "Warning").ShowWarning();
		return true;
	}

	if (!m_IsInitialized)
		Raise("Game not initialized");

	if (m_IsTestGame) {
		if (m_GameScene != nullptr) {
			Nt::Log::Instance().Warning("The game is already running");
			Nt::MessageWindow(L"The game is already running", L"Warning").ShowWarning();
			return false;
		}

		m_GameScene.reset(new Scene(*m_pEngineScene));
		m_GameScene->SetEventBus(m_EventBus);
		m_RenderEngine->SetScene(m_GameScene.get());
	}

	try {
		m_GameScene->Start();
	}
	catch (const std::exception& except) {
		Nt::Log::Instance().Error(except.what());
		Nt::MessageWindow(except.what(), "Error").ShowError();
		if (m_IsTestGame)
			m_GameScene.reset();
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
	m_GameScene.reset();
}

void Game::Update(const Float& time) {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	try {
		m_GameScene->Update(time);

		if (m_CameraPtr != nullptr) {
			m_Listener.SetPosition(m_CameraPtr->GetPosition());
			m_Listener.SetRotation(m_CameraPtr->GetAngle());
		}
	}
	catch (const Nt::Error& error) {
		Nt::MessageWindow(error.what(), "Error").ShowError();
		End();
	}
}

void Game::Render() const {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	m_pWindow->Clear();
	m_RenderEngine->Render();
	m_pWindow->Display();
}

const Bool& Game::IsLaunched() const noexcept {
	return m_IsLaunched;
}

void Game::_SetCamera() {
	if (!m_IsInitialized)
		Raise("Game not initialized");

	try {
		for (const ObjectPtr& object : m_GameScene->GetObjects()) {
			if (object->GetToken() != GameCamera::GetClassToken())
				continue;

			m_CameraPtr = static_cast<GameCamera*>(object.get());
			m_RenderEngine->SetCamera(m_CameraPtr->GetCamera());
			return;
		}
	}
	catch (const Nt::Error& error) {
		error.Show();
	}

	m_CameraPtr = nullptr;
	m_RenderEngine->SetCamera(&m_DefaultCamera);
}