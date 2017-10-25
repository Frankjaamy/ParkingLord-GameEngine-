#pragma once
#include <thread>
#include <mutex>
class TimedEvents 
{
public:
	typedef void(*TimedEventHandler)(TimedEvents * self);

	static TimedEvents * SetTimerWithEvent(float timeCount, TimedEventHandler eventHandler) {
		return new TimedEvents(timeCount, eventHandler);
	}
private:
	TimedEvents(float time,TimedEventHandler handler):
		m_internal_timer(time),m_handler_(handler)
	{
		m_thead_ = std::thread(&TimedEvents::Thread_Call,this);
	}
	void Thread_Call() {
		std::chrono::seconds sec((int)m_internal_timer);
		std::this_thread::sleep_for(sec);

		m_handler_(this);
	}
private:
	std::thread m_thead_;
	TimedEventHandler m_handler_;
	float m_internal_timer;
};