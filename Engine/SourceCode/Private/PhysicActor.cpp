#include <math.h>
#include "PhysicActor.h"
#include "BasicActor.h"
#include "Matrix.h"
#include "Utilities\MyOwnAssert.h"
PhysicActor::PhysicActor(const StrongPtr<BasicActor> & actor, PhysicInfo * physic):m_physicInfo(*physic),m_Object(actor),m_velocity(0, 0), m_acceleration(0, 0),m_appliedforce(0, 0),m_wheel_angle_(0.f) {
	m_physicInfo = *physic;
}



void PhysicActor::RotateActorAlongZAxis(float radius) {
	m_appliedforce = m_appliedforce + Vector3D(0,0,radius);
}

void PhysicActor::WheelRotate(float angle_delta) {
	m_wheel_angle_ += angle_delta;
}

void PhysicActor::Stop() {
	m_appliedforce = m_velocity = Vector3D(0, 0, 0);

}
void PhysicActor::ClearForce() {
	m_appliedforce = Vector3D(0, 0, 0);

}

void PhysicActor::applyForce(const Vector3D force) {
	m_appliedforce = m_appliedforce + force;
}
const float PI = 3.1415926f;
const float PI_IN_DEGREE = 180.0f;

void PhysicActor::update(float dt) {
	StrongPtr<BasicActor> strptr_actor = m_Object.Acquire();
	if (strptr_actor != nullptr && (!strptr_actor->IsDisabled())) {
		if (m_velocity == gZeroVector && m_appliedforce== gZeroVector) {
			return;
		}
		Vector3D currentPosition = strptr_actor->getPosition();
		Matrix::Matrix4x4 rotation_matrix = Matrix::GetRotationMatrix(strptr_actor->getRotation());
		Matrix::Matrix4x4 move_matrix = Matrix::GetTrasnsformMatrix(currentPosition);
		Matrix::Matrix4x4 transform_matrix = rotation_matrix*move_matrix;


		Vector3D m_tmpVelocity = m_velocity;
		Vector3D velocityFactor = m_tmpVelocity;

		Vector4D actual_force = Vector4D(m_appliedforce)*rotation_matrix;
		Vector4D current_velocity = Vector4D(m_tmpVelocity)*rotation_matrix;

		Vector3D velocity_X = Vector3D(1, 0, 0) * DotProduct(current_velocity.GetVector3D(), Vector3D(1, 0, 0));
		Vector3D dragForce = m_tmpVelocity * m_physicInfo.m_fDrag;
		//Vector3D lateralForce = velocity_X * m_physicInfo.m_fDrag;

		m_acceleration = (m_appliedforce - dragForce) / m_physicInfo.m_fMass;
		
		m_velocity = m_velocity + m_acceleration;
		Vector3D actual_move = (m_tmpVelocity + m_velocity) / 2 * dt;

		Vector3D actual_move_car_direction = -actual_move*cos(m_wheel_angle_ / PI_IN_DEGREE*PI);
		float actual_move_car_rotation = -(actual_move*sin(m_wheel_angle_ / PI_IN_DEGREE*PI)).Y();

		Vector4D move_4D = Vector4D(actual_move)*transform_matrix;
		Vector3D nxtPosition = Vector3D(move_4D.x_, move_4D.y_, move_4D.z_);

		strptr_actor->setPosition(nxtPosition);
		strptr_actor->RotateAlongZInRadius(actual_move_car_rotation / PI_IN_DEGREE * m_physicInfo.m_rotate_ratio);

	}
}