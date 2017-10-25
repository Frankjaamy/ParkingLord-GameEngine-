#pragma once
#include "Vector.h"
#include "SmartPointer.h"
class BasicActor;
class SimplePhysicSystem;

using namespace CYNData;
struct PhysicInfo
{
	float m_fMass;
	float m_fDrag;

	float m_rotate_ratio;

	PhysicInfo() :m_fMass(0.f), m_fDrag(0.f), m_rotate_ratio(0.0f){
	}
	PhysicInfo(float mass, float drag, float wheel_ratio) :
		m_fMass(mass), m_fDrag(drag),m_rotate_ratio(wheel_ratio) {
	}
	PhysicInfo(const PhysicInfo & info):PhysicInfo(info.m_fMass,info.m_fDrag,info.m_rotate_ratio) {

	}
};

class PhysicActor {
friend class SimplePhysicSystem;
public:
	void RotateActorAlongZAxis(float radius);
	void applyForce(const Vector3D force);
	void ClearForce();
	void Stop();
	void update(float dt);

	void WheelRotate(float angle_delta);
public:
	PhysicInfo m_physicInfo;
	WeakPtr<BasicActor> m_Object;
	Vector3D m_velocity;
	Vector3D m_acceleration;
	Vector3D m_appliedforce;

	float m_wheel_angle_;
private:
	PhysicActor() :m_physicInfo(),m_velocity(0,0),m_acceleration(0,0),m_appliedforce(0,0){ ; }
	PhysicActor(const StrongPtr<BasicActor> & actor, PhysicInfo * physic);
};