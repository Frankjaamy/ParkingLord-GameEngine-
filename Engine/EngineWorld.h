#pragma once
#include <stdlib.h>
#include <vector>
#include <thread>
#include <mutex>

#include "Utilities\MyOwnAssert.h"
#include "SmartPointer.h"
#include "AllocatorOperation.h"
#include "ActorController.h"

//needs to be included because it is template based;
#include "SimpleMessageSystem.h"
class BasicActor;
class SimplePhysicSystem;
class SimpleRenderingSystem;
class CollisionSystem;

namespace Engine 
{
	class EngineWorld 
	{
	public:
		~EngineWorld();
		EngineWorld();
		void cleanSelf();

		void addActor(StrongPtr<BasicActor>);
		void AddController(StrongPtr<PlayerController> controller) { m_all_controllers_.push_back(controller); }

		SimplePhysicSystem * getPhysics();
		SimpleRenderingSystem * getRenderer();
		CollisionSystem * getCollider(); 
		SimpleMessageSystem<hashed_string_value> * GetMessageSystem();

		inline void SetActorController(int index) { active_controller_index_ = index; }
		inline StrongPtr<PlayerController> GetActorController() const { if (active_controller_index_ >= 0) return m_all_controllers_[active_controller_index_]; else return StrongPtr<PlayerController>(nullptr); }

		static std::condition_variable render_cv;
		static std::mutex render_cv_mutex_;
		static bool is_ready_to_render_;	

	public:
		void DisableActor(int its_unique_id);
		void DisableActor(StrongPtr<BasicActor> actor);

	private:
		std::vector<StrongPtr<BasicActor>> m_allGameActors;
		std::vector<StrongPtr<PlayerController>> m_all_controllers_;

		
		SimplePhysicSystem * p_PhysicSystem;
		SimpleRenderingSystem * p_Rendering;
		CollisionSystem * p_collision_system_;
		SimpleMessageSystem<hashed_string_value> * p_message_system_;

		int active_controller_index_;
	};
}
