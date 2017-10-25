#include "ControllerFactory.h"
#include "EngineOperation.h"
#include "Engine\EngineWorld.h"
namespace Engine {
	ActorController * ControllerFactory::CreateController(const ControllerInfo & info) {	
		if (&info) {
			HashString hash_value(info.controller_name_);
			if (hash_value == HashString("ViehcleController")) {
				ViehcleController * vController = new ViehcleController(info.controller_info.data_1_,info.controller_info.data_2_);
				StrongPtr<PlayerController> ptr(reinterpret_cast<PlayerController *>(vController));
				Engine::GetEngine()->AddController(ptr);
				return (ActorController *)vController;
			}
			else {
				return new DefaultController();
			}
		}
		else {
			return new DefaultController();
		}
	}

	bool ControllerFactory::RegisterController(ActorController * controller, void * bind_object) {
		if (controller->GetControllerType() == HashString("ViehcleController").GetHashedValue()) {
			ViehcleController * c  = (ViehcleController *)(controller);
			StrongPtr<PhysicActor> * actor_to_bind = (StrongPtr<PhysicActor> *)(bind_object);
			c->SetActor(*actor_to_bind);
			return true;
		}
		return false;
	}
}