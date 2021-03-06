#pragma once
#include <string>
#include <vector>
#include <map>
#include "Event.h"
#include "EventListener.h"

class EventManager
{
public:
	EventManager();
	~EventManager();
	void CreateNewEvent(std::string name);
	void Subscribe(std::string name, EventListener* listener);
	void UnSubscribe(std::string name, EventListener* listener);
	void Proc(std::string name);
private:
	std::map<std::string, Event*> events;
};

