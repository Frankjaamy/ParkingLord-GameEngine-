#include <Windows.h>
#include "Engine\EngineOperation.h"
#include "CrazyParkingGameControll.h"
#include "Game.h"
namespace Game {
	GameState g_currentState = eGamePause;
	CYNData::Vector3D g_game_finish_position;

	void StartGame(const char * level_file, bool bNeedToClearDataFirst) 
	{
		std::lock_guard<std::mutex> lk(g_game_mutex);
		if (bNeedToClearDataFirst)
			Engine::ClearData();
		Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_START_GAME").GetHashedValue(), nullptr, 0);

		ReadLevelFromLua(level_file);
		LoadGameUI();
		
		Engine::SetCollisionHandler(Game::CollideHandle);
		g_currentState = eGameRun;
	}

	void CheckGameCondition() 
	{
		CYNData::Vector3D position = Engine::GetEngine()->GetActorController()->GetBasicActor()->getPosition();
		if (CYNData::Distance(position, g_game_finish_position) <= 10.0f) {
			Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_NEXT_LEVEL").GetHashedValue(), nullptr, 0);
		}
	}

}