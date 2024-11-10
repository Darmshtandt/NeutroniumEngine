#include <Main.h>
#include <Sound.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Primitive.h>
#include <Entity.h>
#include <GameCamera.h>
#include <GameSound.h>
#include <GameModel.h>
#include <Grid.h>
#include <Selector.h>
#include <ObjectsTree.h>
#include <Scence.h>
#include <PropertyWindow.h>

#include <FileExplorer.h>
#include <Project.h>
#include <ProjectMainPage.h>
#include <ProjectCreationPage.h>
#include <ProjectManager.h>

#include <Game.h>
#include <Engine.h>
#include <Menu.h>
#include <Application.h>

#pragma comment(lib, "Comctl32")


int main() {
	try {
		Settings settings;
		settings.Load();

		ProjectManager projectManager;
		projectManager.Initialize(settings);
		projectManager.Run();

		if (projectManager.GetProjectPtr() != nullptr) {
			Application neutroniumEngine(&projectManager);
			neutroniumEngine.Initialize(settings);
			neutroniumEngine.Run();
		}
	}
	catch (const Nt::Error& Error) {
		Error.Show();
		Nt::Console::Show();
		Nt::Console::Pause();
	}
	return 0;
}