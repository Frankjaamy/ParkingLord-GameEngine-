#pragma once
#include "Engine\TimeBasedEvents.h"
#include "Engine\EngineOperation.h"
#include "Game.h"
#include "GameUI.h"
namespace Game {
	class LoadingProcess
	{
	public:
		typedef void(*EventHandler)(LoadingProcess * self);

		static LoadingProcess * LoadingWithFinishEvent(float timeCount, const char * filetoload,EventHandler eventHandler) {
			return new LoadingProcess(timeCount, filetoload,eventHandler);
		}
	private:
		LoadingProcess(float time, const char * filetoload, EventHandler handler) :
			m_internal_timer(time), m_handler_(handler)
		{
			m_thead_ = std::thread(&LoadingProcess::Thread_Call, this, filetoload);
		}
		virtual void Thread_Call(const char * filetoload) {
				std::chrono::seconds sec((int)m_internal_timer);
				std::this_thread::sleep_for(sec);

				std::lock_guard<std::mutex> lk(g_game_mutex);
				Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_START_GAME").GetHashedValue(), nullptr, 0);
				ReadLevelFromLua(filetoload);
				LoadGameUI();
				m_handler_(this);
		}
	private:
		std::thread m_thead_;
		EventHandler m_handler_;
		float m_internal_timer;
	};
}
