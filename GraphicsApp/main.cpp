#include <assert.h>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _DEBUG
#include "Game.h"
#include "Engine\AllocatorOperation.h"
#include "Engine\EngineOperation.h"

void MyGame(HINSTANCE i_hInstance, int i_CmdShow) {
	if (Engine::InitializeWindow(i_hInstance, i_CmdShow, "GameStep1", 1, 800, 600)) {
		if (Game::Startup()) {
			Game::MainLoop();
			Game::ShutDown();
		}
	}
	Engine::ShutDown();
}

#if 1
int WINAPI wWinMain(HINSTANCE i_hInstance, HINSTANCE i_hPrevInstance, LPWSTR i_lpCmdLine, int i_nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(873);
	MyGame(i_hInstance, i_nCmdShow);
#if defined _DEBUG
	_CrtDumpMemoryLeaks();
#endif // _DEBUG
}
#endif // 1


