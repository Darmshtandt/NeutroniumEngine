#pragma once

#include <Settings.h>
#include <Selector.h>
#include <Scene.h>

#include <Objects/Entities/Entity.h>
#include <Objects/Primitives/Primitive.h>

#include <Game.h>

#include <memory>
#include <Controllers/CameraController.h>
#include <Editor/Camera3D.h>
#include <Nt/Graphics/System/WindowElements.h>

class RenderEngine;
class InputContext;
class CameraController;
class WorldEditor;
class Clipboard;

class Engine {
private:
	enum class ViewMode {
		PERSPECTIVE,
		TOP, BOTTOM,
		LEFT, RIGHT,
		BACK, FORWARD
	};

	enum ControlElements {
		CONTROL_COMBOBOX_PROJECTION,
		CONTROL_TEXTEDIT_GRIDCELLSIZE,
		CONTROL_BUTTON_TOGGLEGRID,
	};

	struct Listener;

public:
	explicit Engine(const std::weak_ptr<Nt::EventBus>& pBus);
	~Engine();

	void Initialize(const Settings& settings, const Nt::String& defaultInitialPath);

	void StartTestGame() const;
	void CloseTestGame();

	void Update();
	void Render();

	void Show();

	void DeactivateWindow();

	void EnableFly() noexcept;
	void DisableFly() noexcept;
	void ToggleFly() noexcept;

	std::weak_ptr<Scene> GetScene() const noexcept;
	WorldEditor* GetWorldEditor() const noexcept;
	Selector* GetSelector() const noexcept;
	Bool IsFly() const noexcept;

	void SetRect(const Nt::IntRect& rect) noexcept;
	void SetParent(Nt::HandleWindow& window) noexcept;
	void SetTheme(const Style& style);
	void SetLanguage(const Language& language);

private:
	std::unique_ptr<RenderEngine> m_RenderEngine;
	std::shared_ptr<InputContext> m_InputContext;
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	Nt::RenderWindow m_Window;
	NtEx::Camera3D m_OrthoCamera;
	Nt::Keyboard m_Keyboard;
	Nt::Mouse m_Mouse;
	std::shared_ptr<Listener> m_Listener;

	std::unique_ptr<Nt::ComboBox> m_pComboBoxView;
	std::unique_ptr<Nt::Button> m_pGridButton;
	std::unique_ptr<Nt::TextEdit> m_pGridTextEdit;

	std::unique_ptr<CameraController> m_CameraController;
	std::unique_ptr<WorldEditor> m_WorldEditor;
	std::unique_ptr<Game> m_Game;

	std::weak_ptr<Scene> m_pScene;
	Settings m_Settings;
	Selector* m_pSelector = nullptr;
	ViewMode m_Projection = ViewMode::PERSPECTIVE;
	Float m_Zoom = 0.1f;

	Bool m_IsFly = false;
	Bool m_IsOrthoEnabled = false;

private:
	void _InitializeUI(const std::string& defaultInitialPath);

	void _SetPerspective() noexcept;
	void _SetOrtho() noexcept;

	void _Control();
	void _CSG();
	Long _Procedure(const uInt& uMsg, const DWord& param_1, const DWord& param_2);
	void _UpdateProjection();


	struct Listener : Nt::WindowListener, Nt::MouseListener {
		friend class Engine;

		explicit Listener(NotNull<Engine*> pEngine) : EnginePtr(pEngine)
		{
		}

		void Resize(const Nt::ResizeType& r, const Nt::Int2D& s) override {
			(void)r;
			(void)s;

			if (EnginePtr->m_Projection == ViewMode::PERSPECTIVE)
				EnginePtr->_SetPerspective();
			else
				EnginePtr->_SetOrtho();
		}
		void Wheel(const Nt::MouseMode& m, const Short& delta, const Nt::Int2D& s) override {
			(void)m;
			(void)s;

			if (delta < 0.f)
				EnginePtr->m_CameraController->SpeedDown();
			else if (delta > 0.f)
				EnginePtr->m_CameraController->SpeedUp();
		}

		Engine* EnginePtr;
	};
};