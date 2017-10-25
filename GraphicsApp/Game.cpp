#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _DEBUG
#include "Engine\SimplePhysicSystem.h"
#include "Engine\EngineOperation.h"
#include "Engine\PhysicActor.h"
#include "Engine\ThreadFileProcessor.h"
#include "Engine\LuaActorFile.h"
#include "Utilities\Timing.h"

#include "Game.h"
#include "GameUI.h"
#include "CrazyParkingGameControll.h"
#include "LoadingFile.h"

namespace Game {
	GLib::Texture * pGoodGuyTexture;
	GLib::Texture * pBadGuyTexture;
	std::mutex g_game_mutex;

	void ReadLevelFromLua(const char * luaFile) {
		lua_State * pLuaState = luaL_newstate();
		ASSERT(true, "getLuaState Failed!");
		luaL_openlibs(pLuaState);

		size_t sizeFile = 0;
		void * pLuaFile = Engine::File::LoadFile(luaFile, sizeFile);

		if (pLuaFile && sizeFile) {
			int result = 0;

			result = luaL_loadbuffer(pLuaState, reinterpret_cast<char *>(pLuaFile), sizeFile, "lua_initial_chunk");
			ASSERT(result == 0, "load chunk error!");

			lua_pcall(pLuaState, 0, result, 0);
			ASSERT(result == 0, "calling LuaFunc error!");

			int type = LUA_TNIL;
			result = lua_getglobal(pLuaState, "Level");
			ASSERT(result == LUA_TTABLE, "Level is not a table. Check Lua File.");

			lua_pushstring(pLuaState, "level_name");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "level_row_numbers");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
			lua_Number level_row_size = lua_tonumber(pLuaState, -1);
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "level_row_data");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");
			size_t index = 0;
			lua_pushnil(pLuaState);
			while (lua_next(pLuaState, -2)) {
				const char * row_data = lua_tostring(pLuaState, -1);
				type = lua_gettable(pLuaState, -4);
				ASSERTNL(type == LUA_TTABLE);

				lua_pushstring(pLuaState, "initial_position");
				type = lua_gettable(pLuaState, -2);
				ASSERTNL(type == LUA_TTABLE);
				float initial_pos[2] = { 0 };
				Engine::Lua::LoadFloatArray(pLuaState, -1, initial_pos, 2);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "number_slots");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number number_slots = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "distance_between");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number distance_between = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "empty_slot_size");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number empty_slot_size = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "empty_slot");
				type = lua_gettable(pLuaState, -2);
				ASSERTNL(type == LUA_TTABLE);
				float * empty_slots = new float[(unsigned int)empty_slot_size];
				Engine::Lua::LoadFloatArray(pLuaState, -1, empty_slots, (size_t)empty_slot_size);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "car");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");
				const char * actorName = lua_tostring(pLuaState, -1);
				lua_pop(pLuaState, 1);
				lua_pop(pLuaState, 1);

				CYNData::Vector3D car_position(initial_pos[0], initial_pos[1], 0);
				for (int i = 0; i < (int)number_slots; i++, car_position = car_position + CYNData::Vector3D((float)distance_between, 0, 0)) {
					bool b = false;
					for (int j = 0; j < empty_slot_size; j++) {
						if (i == empty_slots[j]) {
							b = true;
							break;
						}
					}
					if(!b)
						Engine::CreateActorFromLua(actorName, car_position);
				}
				if (++index == level_row_size) {
					lua_pop(pLuaState, 1);
					break;
				}

				delete[] empty_slots;
			}

			lua_pop(pLuaState, 1);
			lua_pushstring(pLuaState, "player_initial_position");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");
			float data_position[2] = { 0.0f };
			size_t sizeRead = Engine::Lua::LoadFloatArray(pLuaState, -1, data_position, 2);
			ASSERT(sizeRead == 2, "wrong size read!");
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "level_finish_position");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");
			float level_position[2] = { 0.0f };
			sizeRead = Engine::Lua::LoadFloatArray(pLuaState, -1, level_position, 2);
			ASSERT(sizeRead == 2, "wrong size read!");

			g_game_finish_position = Vector3D(level_position[0], level_position[1]);
			StrongPtr<BasicActor> game_start_ui = Engine::CreateActorFromLua("data/UILayout/UI_Parking.lua", Vector3D(level_position[0],level_position[1]));
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "player");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");
			Engine::CreateActorFromLua(lua_tostring(pLuaState, -1), CYNData::Vector3D(data_position[0], data_position[1], 0));
			lua_pop(pLuaState, 1);

			lua_close(pLuaState);
			delete[] pLuaFile;
		}
	}

	bool Startup() {
		Engine::GetEngine();
		Engine::Threads::ThreadedFileProcessor & fileProcessor = Engine::Threads::ThreadedFileProcessor::getInstance();
		
		g_currentState = eGamePause;
		StrongPtr<BasicActor> game_start_ui = Engine::CreateActorFromLua("data/UILayout/UI_GameStart.lua", Vector3D(0, -80));

		GLib::SetKeyStateChangeCallback(TestKeyCallback);
		if (!g_pGameUIManager) {
			g_pGameUIManager = new GameUILayout();
			g_pGameUIManager->SetCurrentGameStatusUI(game_start_ui);
			Engine::GetEngine()->GetMessageSystem()->RegisterObserver(HashString("EVENT_START_GAME").GetHashedValue(), std::bind(&GameUILayout::OnGameStartUI, g_pGameUIManager, std::placeholders::_1, std::placeholders::_2));
			Engine::GetEngine()->GetMessageSystem()->RegisterObserver(HashString("EVENT_END_GAME").GetHashedValue(), std::bind(&GameUILayout::OnGameOverUI, g_pGameUIManager, std::placeholders::_1, std::placeholders::_2));
			Engine::GetEngine()->GetMessageSystem()->RegisterObserver(HashString("EVENT_NEXT_LEVEL").GetHashedValue(), std::bind(&GameUILayout::OnGameNextUI, g_pGameUIManager, std::placeholders::_1, std::placeholders::_2));
		}
		Engine::GetEngine()->GetMessageSystem()->RegisterObserver(HashString("EVENT_CHECK_LEVEL_FINISH").GetHashedValue(), std::bind(Game::CheckGameCondition));
		
		
		LoadingProcess * event = LoadingProcess::LoadingWithFinishEvent(2.0f,"data/Level_1.lua", [](LoadingProcess * self)
		{
			Engine::SetCollisionHandler(Game::CollideHandle);
			g_currentState = eGameRun;
		});
		return true;
	}

	void MainLoop() {
		bool bQuit = false;
		do
		{
			// We need to let GLib do it's thing. 
			GLib::Service(bQuit);
			if (!bQuit)
			{
				float dt = Timing::CalLastTimeFrameTime_ms() / 1000.0f;
				
					std::lock_guard<std::mutex> lk(g_game_mutex);
					
					if(g_currentState == eGameRun)
						Engine::UpdateControllers(dt);
				
					Engine::UpdatePhysicStatus(dt);
					Engine::UpdateCollisionInfo(dt);

					if (g_currentState == eGameRun)
						Engine::CollisionCheck(dt);
				
				Engine::Draw(dt);
			}
		} while (bQuit == false);
	}

	void ShutDown() {
		Engine::Threads::ThreadedFileProcessor & fileProcessor = Engine::Threads::ThreadedFileProcessor::getInstance();
		fileProcessor.ShutDown();
		delete g_pGameUIManager;
	}
	void TestKeyCallback(unsigned int i_VKeyID, bool bWentDown)
	{
#ifdef _DEBUG
		const size_t	lenBuffer = 65;
		char			Buffer[lenBuffer];
		sprintf_s(Buffer, lenBuffer, "VKey 0x%04x went %s\n", i_VKeyID, bWentDown ? "down" : "up");
		OutputDebugStringA(Buffer);
#endif // __DEBUG

		StrongPtr<PlayerController> controller = Engine::GetEngine()->GetActorController();
		if (controller != nullptr) {
			controller->ControllCommandCallBack(i_VKeyID, bWentDown);
		}
	}
}

