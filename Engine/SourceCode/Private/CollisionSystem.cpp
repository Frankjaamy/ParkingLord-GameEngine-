#include <stdio.h>
#include <Windows.h>
#include <mutex>
#include "EngineWorld.h"
#include "CollisionSystem.h"
#include "PhysicActor.h"
#include "Matrix.h"
using namespace Matrix;
void CollisionSystem::CheckCollide(float dt) {
	Engine::EngineWorld::is_ready_to_render_ = false;
#if 1
#ifdef _DEBUG
	LARGE_INTEGER li;
	LONGLONG start, end, freq;
	QueryPerformanceFrequency(&li);
	freq = li.QuadPart;
	QueryPerformanceCounter(&li);
	start = li.QuadPart;
#endif

	size_t size = m_strP_collisionActors_.size();
	for (size_t i = 0; i < size; ++i) {
		StrongPtr<CollisionObject> objectA = m_strP_collisionActors_[i];
		for (size_t j = i + 1; j < size; ++j) {
			StrongPtr<CollisionObject> objectB = m_strP_collisionActors_[j];
			if (DetectCollision(objectA, objectB, Vector3D(1, 0, 0))) {
				if (DetectCollision(objectA, objectB, Vector3D(0, 1, 0))) {
					if(my_handler_)
						my_handler_(objectA, objectB);
				}
			}	
		}
		objectA->is_data_dirty_ = true;
	}
#ifdef _DEBUG
	QueryPerformanceCounter(&li);
	end = li.QuadPart;
	int useTime = (int)((end - start) * 1000 / freq);

	const size_t	lenBuffer = 65;
	char			Buffer[lenBuffer];
	sprintf_s(Buffer, lenBuffer, "%d\n",useTime);
	//OutputDebugStringA(Buffer);
#endif // __DEBUG
#endif

#if 0
	static bool b = false;
	if (!b) {
		Engine::JobSystem::CreateQueue("CollisionJob", 4);
		b = true;
	}
	size_t size = m_strP_collisionActors_.size();
	for (size_t i = 0; i < size; ++i) {
		StrongPtr<CollisionObject> objectA = m_strP_collisionActors_[i];

		int numbers = size - (i+1);
		int number_per_thread = 0;
		if (!(numbers % 4))
			number_per_thread = numbers / 4;
		else
			number_per_thread = numbers / 4 + 1;

		CollisionJob * jobs[4] = { nullptr };
		for (int j = 0; j < 4; ++j) {
			size_t start = i+1+number_per_thread*j;
			size_t numbers = (size - start) > number_per_thread ? number_per_thread : (size - start);
			if (start >= size - 1) {
				jobs[j] = nullptr;
				break;
			}
			jobs[j] = new CollisionJob(this, i, start, numbers);
			Engine::JobSystem::RunJob(*jobs[i], "CollisionJob");
		}
		Engine::JobSystem::WairForAllJobsDone("CollisionJob");
		objectA->is_data_dirty_ = true;
	}
#endif
}

StrongPtr<CollisionObject> CollisionSystem::registerCollision(const StrongPtr<BasicActor> & actor, const StrongPtr<PhysicActor> & physicActor) {
	StrongPtr<CollisionObject> strongPtr(new CollisionObject(actor, physicActor));
	m_strP_collisionActors_.push_back(strongPtr);
	return strongPtr;
}

void CollisionSystem::updateCollideInfo(float dt) { 

}

bool CollisionSystem::DetectCollision(StrongPtr<CollisionObject>  objectA,StrongPtr<CollisionObject> objectB, Vector3D axis)
{
	if (objectA != nullptr && objectB != nullptr) {
		StrongPtr<BasicActor> basicA = objectA->m_Object_.Acquire();
		StrongPtr<BasicActor> basicB = objectB->m_Object_.Acquire();

		if (basicA == nullptr || basicB == nullptr || basicA->IsDisabled() || basicB->IsDisabled()) {
			return false;
		}

		StrongPtr<PhysicActor> physicA = objectA->m_physic_object_.Acquire();
		StrongPtr<PhysicActor> physicB = objectB->m_physic_object_.Acquire();

		if (objectA->is_data_dirty_) {
			Matrix4x4 rotation_A = Matrix::GetRotationMatrix(basicA->getRotation());
			Matrix4x4 transform_A = Matrix::GetTrasnsformMatrix(basicA->getPosition());
			Matrix4x4 t_A_to_World = rotation_A * transform_A;

			objectA->m_to_world_transform_ = t_A_to_World;
			objectA->m_to_world_transform_inverse = t_A_to_World.Inverse();

			objectA->is_data_dirty_ = false;
		}
		if (objectB->is_data_dirty_) {
			Matrix4x4 rotation_B = Matrix::GetRotationMatrix(basicB->getRotation());
			Matrix4x4 transform_B = Matrix::GetTrasnsformMatrix(basicB->getPosition());
			Matrix4x4 t_B_to_World = rotation_B*transform_B;

			objectB->m_to_world_transform_ = t_B_to_World;
			objectB->m_to_world_transform_inverse = t_B_to_World.Inverse();
			objectB->is_data_dirty_ = false;
		}


		Matrix4x4 t_A_to_B = objectA->m_to_world_transform_ *objectB->m_to_world_transform_inverse;

		Vector3D center_A = basicA->getBoundingBox().center_;
		Vector3D extend_A = basicA->getBoundingBox().extends_;
		Vector3D speed_A = physicA->m_velocity;

		Vector3D center_B = basicB->getBoundingBox().center_;
		Vector3D extend_B = basicB->getBoundingBox().extends_;
		Vector3D speed_B = physicB->m_velocity;

		Vector4D A_in_B = Vector4D(center_A) * t_A_to_B;
		Vector4D A_Extend_X_in_B = Vector4D(Vector3D(extend_A.X(), center_A.Y(), 0))*t_A_to_B;
		Vector4D A_Extend_Y_in_B = Vector4D(Vector3D(0, extend_A.Y()+ center_A.Y(),0))*t_A_to_B;
		Vector4D A_Speed_in_B = Vector4D(speed_A)*t_A_to_B;

		float f1 = DotProduct(A_in_B.GetVector3D(),axis);
		float f2 = fabsf(DotProduct((A_Extend_X_in_B.GetVector3D() - A_in_B.GetVector3D()), axis));
		float f3 = fabsf(DotProduct((A_Extend_Y_in_B.GetVector3D() - A_in_B.GetVector3D()), axis));
		float factor_1 = f1;
		float factor_2;
		if (DotProduct(A_in_B.GetVector3D(), axis) <= 0) {
			factor_2 = DotProduct(center_B, axis) - DotProduct(extend_B, axis)-f2-f3;
			return factor_1 >= factor_2;
		}
		else {
			factor_2 = DotProduct(center_B, axis) + DotProduct(extend_B, axis) + f2+f3;
			return factor_1 <= factor_2;
		}
	}
	return false;
}

void CollisionSystem::CollisionJob::Run() {
	size_t size = m_pCollisionSystem_->m_strP_collisionActors_.size();
	StrongPtr<CollisionObject> objectA = m_pCollisionSystem_->m_strP_collisionActors_[m_compared_index];
	for (size_t j = m_start_index_; j < m_number_; ++j) {
		StrongPtr<CollisionObject> objectB = m_pCollisionSystem_->m_strP_collisionActors_[j];
		if (m_pCollisionSystem_->DetectCollision(objectA, objectB, Vector3D(1, 0, 0))) {
			if (m_pCollisionSystem_->DetectCollision(objectA, objectB, Vector3D(0, 1, 0))) {
				if (m_pCollisionSystem_->my_handler_)
					m_pCollisionSystem_->my_handler_(objectA, objectB);
			}
		}
	}
}