#pragma once
#include<stdint.h>
#include "ActorController.h"
#include "SmartPointer.h"

class DefaultController:public ActorController, public ClassNameWrapper
{
public:
	DefaultController():ClassNameWrapper("DefaultController") {};
	~DefaultController() {};

	void UpdateActor(float dt) {};
	virtual size_t GetControllerType() { return GetThisClassName(); };
};
