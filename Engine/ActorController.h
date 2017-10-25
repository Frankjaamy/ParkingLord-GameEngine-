#pragma once
#include "SmartPointer.h"
#include"BasicActor.h"
class PhysicActor;

class ActorController {
public:
	virtual void UpdateActor(float dt) = 0;
	virtual ~ActorController() {};

	virtual size_t GetControllerType() = 0;
};

class PlayerController:public ActorController{
public:
	virtual ~PlayerController() {};
	virtual void SetActor(StrongPtr<PhysicActor> actor) = 0;
	virtual StrongPtr<BasicActor> GetBasicActor() = 0;
	virtual void ControllCommandCallBack(unsigned int i_VKeyID, bool bWentDown) = 0;
};

