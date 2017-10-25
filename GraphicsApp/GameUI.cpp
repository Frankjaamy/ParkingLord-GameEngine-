#include "Engine\EngineOperation.h"
#include "Engine\TimeBasedEvents.h"
#include "GameUI.h"
#include "CrazyParkingGameControll.h"

namespace Game {
	GameUILayout * g_pGameUIManager = nullptr;


	GameUILayout::GameUILayout() {
		
	}
	void GameUILayout::UpdateGameHudUI(void * data, int sizeofdata) {
		float wheel_angle = reinterpret_cast<float *>(data)[0];
		float current_speed = reinterpret_cast<float *>(data)[1];
		//LogOutPutStandard("wheelAngle is %f and Speed is %f\n",wheel_angle,current_speed);

		m_wheel_ui_->SetRotationZInRadius(wheel_angle/180.0f * PI);
		m_SpeedIndicator_ui_->SetRotationZInRadius(-current_speed * PI*2.0f + 0.6f);
	}

	void GameUILayout::OnGameStartUI(void * data, int sizeofdata) {
		Engine::GetEngine()->DisableActor(m_current_game_status_ui_);
	}

	void GameUILayout::OnGameOverUI(void * data, int sizeofdata) {
		Engine::GetEngine()->DisableActor(m_current_game_status_ui_);
		m_current_game_status_ui_ = Engine::CreateActorFromLua("data/UILayout/UI_GameOver.lua", Vector3D(0, -80));
		g_currentState = eGamePause;
		TimedEvents * event = TimedEvents::SetTimerWithEvent(2.0f, [](TimedEvents * self){
				StartGame("data/Level_1.lua",true);
			}
		);
	}
	void GameUILayout::OnGameNextUI(void * data, int sizeofdata) {
		Engine::GetEngine()->DisableActor(m_current_game_status_ui_);
		m_current_game_status_ui_ = Engine::CreateActorFromLua("data/UILayout/UI_GameNext.lua", Vector3D(0, -80));
		g_currentState = eGamePause;
		TimedEvents * event = TimedEvents::SetTimerWithEvent(2.0f, [](TimedEvents * self) {
			StartGame("data/Level_2.lua", true);
		}
		);
	}

	void LoadGameUI() {
		StrongPtr<BasicActor> wheel = Engine::CreateActorFromLua("data/UILayout/UI_Wheel.lua", Vector3D(-320, 180));
		StrongPtr<BasicActor> speed_panel = Engine::CreateActorFromLua("data/UILayout/UI_DashBoard.lua", Vector3D(-160, 180));
		StrongPtr<BasicActor> speed_indicator = Engine::CreateActorFromLua("data/UILayout/UI_Hand.lua", Vector3D(-160, 180));
		if (!g_pGameUIManager) {
			g_pGameUIManager = new GameUILayout();
			g_pGameUIManager->m_current_game_status_ui_ = StrongPtr<BasicActor>(nullptr);
		}
		g_pGameUIManager->m_SpeedPanel_ui_ = speed_panel;
		g_pGameUIManager->m_SpeedIndicator_ui_ = speed_indicator;
		g_pGameUIManager->m_wheel_ui_ = wheel;

		Engine::GetEngine()->GetMessageSystem()->RegisterObserver(HashString("EVENT_UPDATE_GAME_UI").GetHashedValue(), std::bind(&GameUILayout::UpdateGameHudUI, g_pGameUIManager, std::placeholders::_1, std::placeholders::_2));
	}
}