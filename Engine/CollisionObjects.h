#pragma once
#include "Matrix.h"
#include "SmartPointer.h"
#include"PhysicActor.h"
class BasicActor;
class CollisionSystem;

using namespace CYNData;
using namespace Matrix;
class CollisionObject {
friend class CollisionSystem;
public:
	~CollisionObject();
public:
	WeakPtr<BasicActor> m_Object_;
	WeakPtr<PhysicActor> m_physic_object_;
public:
	Matrix4x4 m_to_world_transform_;
	Matrix4x4 m_to_world_transform_inverse;
	bool is_data_dirty_;
private:
	CollisionObject(){ ; }
	CollisionObject(const StrongPtr<BasicActor> & actor, const StrongPtr<PhysicActor> & physicActor);
};