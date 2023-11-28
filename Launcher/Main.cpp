#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Entity.h>
#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>
#include <Selector.h>
#include <ObjectsTree.h>
#include <Scence.h>
#include <Game.h>


int main() {
	try {
		Nt::Console::Hide();

		Game::Config config = { };
		std::ifstream configFile(".gameconf");
		if (configFile.is_open())
			config.Read(configFile);

		std::string rootPath = std::current_path().string() + "\\..\\";
		std::current_path(rootPath);

		const Nt::Int2D windowSize = Nt::Float2D(Nt::GetMonitorSize()) / 2.f;
		Nt::RenderWindow window(windowSize, "Game", true);

		Game game;
		game.InitializeGame(&window, config.ScencePath);
		if (!game.Start())
			Raise("Failed to start game.");

		window.SetClearColor(Nt::Colors::LightBlue);
		window.ShowMaximized();

		Nt::Event event;
		while (window.IsOpened()) {
			window.PeekMessages();
			while (window.PopEvent(&event)) {
				switch (event.Type) {
				case Nt::Event::WINDOW_CLOSE:
					window.Close();
					break;
				}
			}
			window.Clear();

			const Float time = window.GetFPS() / 1000.f;
			game.Update(time);
			game.Render();

			window.Display();
		}
	}
	catch (const Nt::Error& error) {
		error.Show();
	}
	return 0;
}