#include <Main.h>
#include <Style.h>
#include <Language.h>
#include <Settings.h>

#include <Lua.h>
#include <Script.h>
#include <Object.h>
#include <Entity.h>
#include <Grid.h>
#include <Selector.h>
#include <Scence.h>

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