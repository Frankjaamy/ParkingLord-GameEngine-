#include "CollideHandler.h"
#include "Engine\BasicActor.h"
#include "Engine\PhysicActor.h"
#include "Engine\EngineOperation.h"
namespace Game {
	void CollideHandle(StrongPtr<CollisionObject> ob1, StrongPtr<CollisionObject> ob2) {

		StrongPtr<PhysicActor> p1 = ob1->m_physic_object_.Acquire();
		StrongPtr<PhysicActor> p2 = ob2->m_physic_object_.Acquire();

		if (p1 != nullptr && p2!=nullptr) {
			Engine::GetEngine()->GetMessageSystem()->NotifyAll(HashString("EVENT_END_GAME").GetHashedValue(), nullptr, 0);
		}
	}
}