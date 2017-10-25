#include "SimplePhysicSystem.h"
#include "PhysicActor.h"
#include "AllocatorOperation.h"
#include "Utilities\MyOwnAssert.h"

SimplePhysicSystem::SimplePhysicSystem(){

}
SimplePhysicSystem::~SimplePhysicSystem() {
	m_strP_physicActors.clear();
}
void SimplePhysicSystem::run(float dt) {
	for (std::vector<StrongPtr<PhysicActor>>::iterator iter = m_strP_physicActors.begin();
		iter != m_strP_physicActors.end(); ++iter) {
		StrongPtr<PhysicActor> strptr_actor = *iter;
		if(strptr_actor != nullptr)
			strptr_actor->update(dt);
	}
}
StrongPtr<PhysicActor> SimplePhysicSystem::registerActor(const StrongPtr<BasicActor> & actor, PhysicInfo * physicInfo) {
	if (physicInfo) {
		StrongPtr<PhysicActor> strongPtr(new PhysicActor(actor, physicInfo));
		m_strP_physicActors.push_back(strongPtr);
		return strongPtr;
	}
	else {
		return StrongPtr<PhysicActor>(nullptr);
	}

}
void SimplePhysicSystem::clean() {
	m_strP_physicActors.clear();
}