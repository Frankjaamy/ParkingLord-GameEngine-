#include <math.h>
#include "CollisionObjects.h"
#include "BasicActor.h"
#include "Utilities\MyOwnAssert.h"
CollisionObject::CollisionObject(const StrongPtr<BasicActor> & actor, const StrongPtr<PhysicActor> & physicActor):m_Object_(actor),m_physic_object_(physicActor), is_data_dirty_(true){
	
}

CollisionObject::~CollisionObject() {

}