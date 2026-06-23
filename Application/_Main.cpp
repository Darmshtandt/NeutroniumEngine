// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Script/Lua.h>
#include <Script/Script.h>
#include <Objects/Object.h>
#include <Objects/Primitives/Primitive.h>
#include <Objects/Entities/Entity.h>
#include <Core/Grid.h>
#include <Selector.h>
#include <ObjectsTree.h>
#include <Scene.h>
#include <Tools/PropertyWindow.h>

#include <FileExplorer.h>
#include <ProjectManager/Project.h>
#include <ProjectManager/ProjectMainPage.h>
#include <ProjectManager/ProjectCreationPage.h>
#include <ProjectManager/ProjectManager.h>

#include <Game.h>
#include <Engine.h>
#include <ProgramMenu.h>
#include <Application.h>
#include <Function.h>
#include <Nt/Core/Console.h>

int main() {
	//Nt::Console::Hide();
	std::ofstream lastedLogFile("Lasted.log");

	MultiStream errorStream;
	errorStream.AddStream(std::cerr);
	errorStream.AddStream(lastedLogFile);

	Nt::Log::Instance().SetErrorStream(errorStream);
	Nt::Log::Instance().SetWarningStream(lastedLogFile);
	Nt::Log::Instance().SetMessageStream(lastedLogFile);

	try {
		SetConsoleOutputCP(1251);

		Settings settings;
		settings.Load();

		std::string initialPath = std::current_path().string();

		for (std::string::reverse_iterator iterator = initialPath.rbegin(); iterator != initialPath.rend(); ++iterator) {
			if ((*iterator) != '\\')
				continue;

			initialPath.erase(iterator.base(), initialPath.end());
			break;
		}

		settings.Language.InitialPath = initialPath;
		settings.Language.LoadFromFile("en.json");
		settings.Style.InitialPath = initialPath;
		settings.Style.UpdateThemeList();
		settings.Style.LoadFromFile("DefaultTheme.json");

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

		lastedLogFile.flush();

		Nt::Console::Show();
		Nt::Console::Pause();
		lastedLogFile.close();
	}
	return 0;
}