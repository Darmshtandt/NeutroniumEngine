#pragma once

#include <Nt/Core/EventBus.h>
#include <Main.h>
#include <Scene.h>
#include <Objects/Entities/GameCamera.h>

#include <Nt/Graphics/Sound/SoundDevice.h>
#include <Nt/Graphics/Sound/Listener.h>

class Game {
public:
	struct Config : public Nt::ISerialization {
		void Write(std::ostream& stream) const override;
		void Read(std::istream& stream) override;

		constexpr uInt Sizeof() const noexcept override {
			return sizeof(*this);
		}
		constexpr uInt ClassType() const noexcept override {
			return 0;
		}

		Nt::String WindowName;
		Nt::String ScenePath;
	};

public:
	Game() noexcept = default;
	~Game();

	void InitializeGame(NotNull<Nt::RenderWindow*> pWindow, const Nt::String& scenePath);
	void InitializeTestGame(NotNull<Nt::RenderWindow*> pWindow, NotNull<Scene*> pScene);

	Bool Start();
	void End();

	void Update(const Float& time);
	void Render() const;

	[[nodiscard]]
	const Bool& IsLaunched() const noexcept;

private:
	Nt::SoundDevice m_SoundDevice;
	Nt::Listener m_Listener;

	std::shared_ptr<Nt::EventBus> m_EventBus = std::make_shared<Nt::EventBus>();
	std::thread m_Thread;
	Nt::Camera m_DefaultCamera;
	const Lua* m_pLua = nullptr;
	Nt::RenderWindow* m_pWindow = nullptr;
	GameCamera* m_CameraPtr = nullptr;
	Scene* m_pEngineScene = nullptr;
	Scene* m_pGameScene = nullptr;
	
	Bool m_IsInitialized = false;
	Bool m_IsTestGame = false;
	Bool m_IsLaunched = false;
	Bool m_IsThreadTerminated = false;

private:
	void _SetCamera();
};