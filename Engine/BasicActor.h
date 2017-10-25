#pragma once
#ifndef _BASIC_ACTOR_H
#define _BASIC_ACTOR_H
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include<string.h>
#include"ClassNameWrapper.h"
#include "Vector.h"
//this class represents BasocActor.
//It has a Point member, position
//and a limited-length name.

const int MAX_CHAR_SIZE = 32;
using namespace CYNData;

struct AABB
{
	Vector3D center_;
	Vector3D extends_;
	AABB():center_(Vector3D(0,0,0)), extends_(Vector3D(0,0,0)) { ; }
	AABB(Vector3D vec_1, Vector3D vec_2) :
		center_(vec_1),
		extends_(vec_2)
	{}
	AABB(const AABB & box) 
	{
		this->center_ = box.center_;
		this->extends_ = box.extends_;
	}
};

class BasicActor:public ClassNameWrapper
{
public:
	virtual ~BasicActor() {
		;
	}
	BasicActor():ClassNameWrapper("BasicActor"),m_index_in_the_world(0), m_Position_(0, 0), m_Name(""),marked_as_disabled(false) {
	}
	BasicActor(float x, float y, AABB box):ClassNameWrapper("BasicActor"), m_index_in_the_world(0),m_Position_(x,y),bounding_box_(box),m_Name(""), marked_as_disabled(false) {
	}
	BasicActor(char i_name[], float x, float y, AABB box):ClassNameWrapper("BasicActor"), m_index_in_the_world(0), m_Position_(x, y),bounding_box_(box), marked_as_disabled(false) {
		memcpy(m_Name, i_name, MAX_CHAR_SIZE);
	}
	BasicActor(const BasicActor & actor):ClassNameWrapper("BasicActor"){
		m_Position_ = actor.m_Position_;
		memcpy(m_Name, actor.getName(), MAX_CHAR_SIZE);
		m_index_in_the_world = actor.m_index_in_the_world;
		bounding_box_ = actor.bounding_box_;
		marked_as_disabled = actor.marked_as_disabled;
	}

	inline float getPosX() const { return m_Position_.X(); }
	inline float getPosY() const { return m_Position_.Y(); }
	inline const char * getName() const{ return m_Name; }
	inline Vector3D getPosition() const { return m_Position_; }
	inline Vector3D getRotation() const { return m_Rotation_; }
	inline AABB getBoundingBox() const { return bounding_box_; }
	inline void setPosition(Vector3D p) { m_Position_ = p;}
	inline void RotateAlongZInRadius(float radius) { m_Rotation_ = m_Rotation_ + Vector3D(0, 0, radius); }
	inline void SetRotationZInRadius(float radius) { m_Rotation_ = Vector3D(0, 0, radius); }

	inline void SetUniqueID(int life) { m_index_in_the_world = life; }
	inline int GetUniqueID() const { return m_index_in_the_world; }
	inline void MarkDisabled(bool b) { marked_as_disabled = b; }
	inline bool IsDisabled() { return marked_as_disabled; }
private:
	
	AABB bounding_box_;

	Vector3D m_Position_;
	Vector3D m_Rotation_;

	char	m_Name[MAX_CHAR_SIZE];
	int		m_index_in_the_world;
	bool	marked_as_disabled;
};
#endif // !_BASIC_ACTOR_H

