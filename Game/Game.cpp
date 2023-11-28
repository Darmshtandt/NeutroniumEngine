#include <Main.h>
#include <Sound.h>
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


void Game::End() {
	m_IsLaunched = false;
	while (m_IsThreadTerminated)
		Sleep(10);

	m_pGameScence->Stop();
	SAFE_DELETE(&m_pGameScence);
}