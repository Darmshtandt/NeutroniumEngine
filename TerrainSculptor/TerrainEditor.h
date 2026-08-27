#pragma once

class TerrainEditor {
public:
	TerrainEditor() :
		m_Window(true),
		m_Terrain("NewTerrain")
	{
	}

	void Initialize() {
		m_Window.Create({ 1910, 1270 }, "TerrainSculptor");

		m_Window.SetPerspectiveProjection(90.f, 0.01f, 1000.f);
		m_Window.SetClearColor(Nt::Colors::LightBlue);
		m_Window.SetCurrentCamera(&m_Camera);

		m_Camera.SetOrigin({ 0.f, 0.f, -5.f });

		m_Cube.SetMesh(Nt::Geometry::Cube({ 1, 1, 1 }, Nt::Colors::White));
	}

	void Run() {
		m_Window.Show();

		Nt::Event event;
		while (m_Window.IsOpened()) {
			m_Window.PeekMessages();
			while (m_Window.PopEvent(&event)) {
				switch (event.Type) {
				case Nt::Event::KEY_UP:
					switch (event.Value) {
					case Nt::Keyboard::KEY_ESCAPE:
						m_Window.Close();
						break;
					}
					break;
				}
			}

			if (GetActiveWindow() != nullptr) {
				m_Window.Clear();

				_Control();
				m_Window.Update();

				m_Cube.Render(&m_Window);

				m_Window.Display();
			}
		}
	}

private:
	Nt::RenderWindow m_Window;
	Nt::Keyboard m_Keyboard;
	Nt::Mouse m_Mouse;
	Nt::Camera m_Camera;
	Nt::Model m_Cube;
	Terrain m_Terrain;

private:
	void _Control() {
		m_Keyboard.Update();
		m_Mouse.Update();

		const Bool isButtonPressed = false;
		const Bool isButtonOnce = false;

		if (m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, false)) {
			Nt::_ShowCursor(false);

			const Nt::Float2D screenCenter = (Nt::GetMonitorSize() / 2);

			if (m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, true))
				m_Mouse.SetCursorPosition(screenCenter);

			const Nt::Float2D cursorPosition = Nt::GetCursorPosition();
			const Nt::Float2D angleRotation = -((cursorPosition - screenCenter) / 6.f);

			m_Camera.RotateAroundOrigin({ angleRotation.y, angleRotation.x, 0.f });

			m_Mouse.SetCursorPosition(screenCenter);
		}
		else {
			Nt::_ShowCursor(true);
		}
	}
};