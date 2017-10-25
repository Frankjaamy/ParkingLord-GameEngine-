#pragma once
#pragma once
#include <vector>
#include "SmartPointer.h"
#include "CollisionObjects.h"
#include "JobSystem.h"
#include "IJob.h"

class BasicActor;
class MyHeapManager;
class CollisionSystem {
	friend class CollisionJob;
public:
	typedef void(*CollisionHandler_t)(StrongPtr<CollisionObject> o2, StrongPtr<CollisionObject> ob2);
public:
	~CollisionSystem() { clean(); };
	CollisionSystem(){};
	
	inline void SetCollisionHandler(CollisionHandler_t handler) { my_handler_ = handler; }

	StrongPtr<CollisionObject> registerCollision(const StrongPtr<BasicActor> & actor, const StrongPtr<PhysicActor> & physicActor);
	void updateCollideInfo(float dt);
	void CheckCollide(float dt);
	inline void clean() { m_strP_collisionActors_.clear(); }

	bool DetectCollision(const StrongPtr<CollisionObject>  object1, const StrongPtr<CollisionObject> object2, Vector3D axis);
	CollisionHandler_t my_handler_;

public:
	class CollisionJob : public Engine::JobSystem::IJob
	{
	public:
		CollisionJob(CollisionSystem * renderer, size_t compare_index, size_t start_index, size_t how_many_to_process) 
			:IJob("DrawObjects"), 
			m_pCollisionSystem_(renderer), 
			m_compared_index(compare_index),
			m_start_index_(start_index), 
			m_number_(how_many_to_process) 
		{
			
		}
		void Run();
	private:
		size_t m_compared_index;
		size_t m_start_index_;
		size_t m_number_;

		CollisionSystem * m_pCollisionSystem_;
	};
private:
	std::vector<StrongPtr<CollisionObject>> m_strP_collisionActors_;
};