#pragma once
#include <vector>
#include "SmartPointer.h"
class PhysicActor;
class BasicActor;
class MyHeapManager;
struct PhysicInfo;
class SimplePhysicSystem {
public:
	~SimplePhysicSystem();
	SimplePhysicSystem();

	StrongPtr<PhysicActor> registerActor(const StrongPtr<BasicActor> & actor, PhysicInfo * physicInfo);
	inline StrongPtr<PhysicActor> GetPhysicActor(size_t index) { return m_strP_physicActors[index]; }
	void run(float dt);
	void clean();

private:
	std::vector<StrongPtr<PhysicActor>> m_strP_physicActors;
};
