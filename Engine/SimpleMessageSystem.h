#pragma once

#include <vector>
#include <map>
#include <functional>
#include <utility>

template <typename EngineEvent>
class SimpleMessageSystem 
{
public:
	template <typename Observer>
	void RegisterObserver(EngineEvent && event,Observer && observer)
	{
		observers_[std::move(event)].push_back(std::forward<Observer>(observer));
	}
	void NotifyAll(const EngineEvent & event, void * additional_data, int sizeofdata)
	{
		for (const auto& obs : observers_.at(event)) 
		{
			obs(additional_data, sizeofdata);
		}
	}
private:
	std::map<EngineEvent, std::vector<std::function<void(void *, int)>>> observers_;
};
