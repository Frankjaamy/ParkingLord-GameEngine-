#pragma once
#include<stdint.h>
#include "ActorController.h"
#include "SmartPointer.h"
#include "PhysicActor.h"
const uint8_t MoveForward = 0x01;
const uint8_t MoveBackward = 0x02;

const uint8_t WheelLeft = 0x03;
const uint8_t WheelRight = 0x04;
const uint8_t WheelStop = 0x05;

const uint8_t PedalZeroPosition = 0x00;
const uint8_t WheelZeroPosition = 0xff;

class ViehcleController:public PlayerController,public ClassNameWrapper
{
public:
	ViehcleController(float maxWheelAngle, float maxViehcleSpeed);
	~ViehcleController();

	void UpdateActor(float dt);

	virtual size_t GetControllerType() { return GetThisClassName();} ;

	StrongPtr<BasicActor> GetBasicActor() override { return m_physic_actor_->m_Object.Acquire(); }
	inline StrongPtr<PhysicActor> GetPhysicActor() { return m_physic_actor_; }
	
	inline void SetActor(StrongPtr<PhysicActor> actor) { m_physic_actor_ = actor; }
	inline void setUIActor(StrongPtr<BasicActor> ui_actor) { m_ui_actor = ui_actor; }
	inline void PedalDown(uint8_t b) { m_pedal_down_flag = b; }
	inline void Wheel(uint8_t direction) { m_wheel_direction_ = direction;}

	void Possess();
public:
	void ControllCommandCallBack(unsigned int i_VKeyID, bool bWentDown);

private:
	StrongPtr<PhysicActor> m_physic_actor_;
	StrongPtr<BasicActor> m_ui_actor;

private:
	uint8_t m_pedal_down_flag;
	uint8_t m_wheel_direction_;

	float m_current_wheel_angle_;
	float m_maximum_wheel_angle_;

	float m_current_speed_;
	float m_maxmum_speed_;
};
