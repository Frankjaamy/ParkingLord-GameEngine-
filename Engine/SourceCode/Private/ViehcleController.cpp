#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _DEBUG
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include "EngineOperation.h"
#include "PhysicActor.h"
#include "SimplePhysicSystem.h"
#include "ViehcleController.h"

#include "Utilities\MyOwnAssert.h"
#include "GLib\GLib.h"
ViehcleController::ViehcleController(float maxWheelAngle,float maxViehcleSpeed):
	ClassNameWrapper("ViehcleController"),
	m_maximum_wheel_angle_(maxWheelAngle),
	m_current_wheel_angle_(0),
	m_current_speed_(0),
	m_maxmum_speed_(maxViehcleSpeed)
	{
	Possess();
}
ViehcleController::~ViehcleController() {
	//Engine::g_World->SetActorController(nullptr);
}
void ViehcleController::UpdateActor(float dt) {
	m_current_speed_ = VectorMod(GetPhysicActor()->m_velocity);
	if (m_current_speed_ <= m_maxmum_speed_) {
		if (m_pedal_down_flag == MoveForward) {
				m_physic_actor_->applyForce(Vector3D(0.0f, -0.015f*dt));
		}
		else if (m_pedal_down_flag == MoveBackward) {
			m_physic_actor_->applyForce(Vector3D(0.0f, 0.015f*dt));
		}
	}

	if(m_pedal_down_flag == PedalZeroPosition) {
		m_physic_actor_->ClearForce();
	}


	if (m_wheel_direction_ == WheelLeft) {
		if (m_current_wheel_angle_ <= m_maximum_wheel_angle_) {
			m_current_wheel_angle_ += 0.15f*dt;
			m_physic_actor_->WheelRotate(0.15f*dt);
		}

	}
	else if(m_wheel_direction_ == WheelRight){
		if (m_current_wheel_angle_ >= -m_maximum_wheel_angle_) {
			m_current_wheel_angle_ += -0.15f*dt;
			m_physic_actor_->WheelRotate(-0.15f*dt);
		}
	}
	else {
		if (m_current_wheel_angle_ < 0) {
			m_current_wheel_angle_ += m_current_speed_*0.15f;
			m_physic_actor_->WheelRotate(m_current_speed_*0.15f);
		}
		else if (m_current_wheel_angle_ > 0) {
			m_current_wheel_angle_ -= m_current_speed_ * 0.15f;
			m_physic_actor_->WheelRotate(-m_current_speed_ * 0.15f);
		}
	}

	float temp_data[2] = { m_current_wheel_angle_,m_current_speed_>m_maxmum_speed_?m_maxmum_speed_:m_current_speed_ };
	Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_UPDATE_GAME_UI").GetHashedValue(),temp_data,2);
}

void ViehcleController::Possess() {
	Engine::GetEngine()->SetActorController(0);
}
void ViehcleController::ControllCommandCallBack(unsigned int i_VKeyID, bool bWentDown){
	switch (i_VKeyID)
	{
	case 87://W
	{
		if (bWentDown) {
			PedalDown(MoveForward);
		}
		else {
			PedalDown(PedalZeroPosition);
		}
	}
	break;
	case 65://A
		if (bWentDown) {
			Wheel(WheelLeft);
		}
		else {
			Wheel(WheelStop);
		}
		break;
	case 68://D
		if (bWentDown) {
			Wheel(WheelRight);
		}
		else {
			Wheel(WheelStop);
		}
		break;
	case 83://S
	{
		if (bWentDown) {
			PedalDown(MoveBackward);
		}
		else {
			PedalDown(PedalZeroPosition);
		}
	}
		break;
	case 0x0020: 
	{
		m_physic_actor_->Stop();

		Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_CHECK_LEVEL_FINISH").GetHashedValue(), nullptr, 0);
		LogOutPutStandard("!!! %f, %f\n", GetPhysicActor()->m_Object.Acquire()->getPosition().X(), GetPhysicActor()->m_Object.Acquire()->getPosition().Y());
	}
		break;
	default:
		break;
	}
}