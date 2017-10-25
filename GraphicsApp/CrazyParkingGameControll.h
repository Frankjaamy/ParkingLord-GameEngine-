#pragma once
#include "Engine\Vector.h"
#include "GameUI.h"
namespace Game {
	enum GameState {
		eGamePause = 0,
		eGamePauseWithControll,
		eGameRun
	};
	extern GameState g_currentState;
	extern CYNData::Vector3D g_game_finish_position;
	
	void StartGame(const char * level_file, bool bNeedToClearDataFirst = false);
	void CheckGameCondition();
}