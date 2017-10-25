#include "EngineWorld.h"
#include "SimplePhysicSystem.h"
#include "SimpleRenderingSystem.h"
#include "CollisionSystem.h"
#include "SimpleMessageSystem.h"

namespace Engine {
	std::condition_variable EngineWorld::render_cv;
	std::mutex EngineWorld::render_cv_mutex_;
	bool EngineWorld::is_ready_to_render_ = false;
	EngineWorld::~EngineWorld() {
		
	}

	void EngineWorld::cleanSelf() {
		delete p_PhysicSystem;
		delete p_Rendering;
		delete p_collision_system_;
		p_PhysicSystem = nullptr;
		p_Rendering = nullptr;
		p_collision_system_ = nullptr;

		m_allGameActors.clear();
		m_all_controllers_.clear();
	}

	EngineWorld::EngineWorld():p_PhysicSystem(nullptr),p_Rendering(nullptr),p_collision_system_(nullptr), p_message_system_(nullptr), active_controller_index_(-1){

	}

	void EngineWorld::addActor(StrongPtr<BasicActor> i_strptr) {
		m_allGameActors.push_back(i_strptr);

		i_strptr->SetUniqueID(m_allGameActors.size());
	}

	SimplePhysicSystem * EngineWorld::getPhysics() {
		if (!p_PhysicSystem) 
			p_PhysicSystem = new SimplePhysicSystem(); 
		return p_PhysicSystem;
	}

	SimpleRenderingSystem * EngineWorld::getRenderer() {
		if (!p_Rendering)
			p_Rendering = new SimpleRenderingSystem();
		return p_Rendering;
	}

	CollisionSystem * EngineWorld::getCollider() {
		if (!p_collision_system_)
			p_collision_system_ = new CollisionSystem();
		return p_collision_system_;
	}

	SimpleMessageSystem<hashed_string_value> * EngineWorld::GetMessageSystem() {
		if (!p_message_system_) {
			p_message_system_ = new SimpleMessageSystem<hashed_string_value>();
		}
		return p_message_system_;
	}

	void EngineWorld::DisableActor(int uuid) {
		for (auto str_actor : m_allGameActors ) {
			if (str_actor->GetUniqueID() == uuid) {
				str_actor->MarkDisabled(true);
			}
		}
	}

	void EngineWorld::DisableActor(StrongPtr<BasicActor> actor) {
		for (auto str_actor : m_allGameActors) {
			if (str_actor == actor) {
				str_actor->MarkDisabled(true);
			}
		}
	}
}
