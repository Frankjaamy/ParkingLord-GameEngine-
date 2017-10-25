#pragma once
#include "Engine\BasicActor.h"
#include "Engine\SmartPointer.h"
namespace Game {
	
	struct GameUILayout {

		StrongPtr<BasicActor> m_wheel_ui_;
		StrongPtr<BasicActor> m_SpeedPanel_ui_;
		StrongPtr<BasicActor> m_SpeedIndicator_ui_;

		StrongPtr<BasicActor> m_current_game_status_ui_;

		GameUILayout();
		void UpdateGameHudUI(void *, int);
		void OnGameStartUI(void *, int);
		void OnGameOverUI(void *, int);
		void OnGameNextUI(void *, int);
		void SetCurrentGameStatusUI(StrongPtr<BasicActor> actor) { m_current_game_status_ui_ = actor; }
	};

	void LoadGameUI();
	extern GameUILayout * g_pGameUIManager;
}