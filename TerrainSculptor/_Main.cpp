#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Script/Lua.h>
#include <Script/Script.h>
#include <Objects/Object.h>
#include <Objects/Entity.h>
#include <Core/Grid.h>
#include <Selector.h>
#include <Scene.h>

#include <Terrain.h>
#include <TerrainEditor.h>

int main() {
	try {
		TerrainEditor editor;

		editor.Initialize();
		editor.Run();
	}
	catch (const Nt::Error& error) {
		error.Show();
		Nt::Console::Pause();
	}

	return EXIT_SUCCESS;
}