#pragma once
#ifndef _POINT_H_
#define _POINT_H_
#include <xmmintrin.h>
#include <stdint.h>
#include "Utilities\MyOwnAssert.h"
#include "Utilities\UtilFunctions.h"

using namespace CYNTools;
namespace CYNData
{
class Vector3D {
	friend class Vector4D;
	public:
		//Default Constructor does nothing; Speed comes first;
		Vector3D():x_(0),y_(0),z_(0) {};
		Vector3D(float x, float y) :x_(x), y_(y), z_(0.0f) {}
		Vector3D(float x, float y, float z) :x_(x), y_(y), z_(z) {}
		Vector3D(const Vector3D & otherVec):x_(otherVec.x_),y_(otherVec.y_),z_(otherVec.z_)
		{}
	public:
		inline float X() const { return x_; }
		inline float Y() const { return y_; }
		inline float Z() const { return z_; }
	public:
		inline void SetZeroVec() { x_ = y_ = z_ = 0.0f; }
		void Normalize();
	//operator override
	public:
		Vector3D & operator = (const Vector3D & other_vec);
		Vector3D operator-();

		Vector3D & operator += (const Vector3D & vec);
		Vector3D & operator -= (const Vector3D & vec);

		Vector3D operator * (float mul);
		Vector3D operator / (float mul);

		Vector3D& operator *= (float mul);
		Vector3D& operator /= (float mul);

		bool operator == (const Vector3D & vec);
		bool operator != (const Vector3D & vec);
	private:
		float x_, y_, z_;
	};
Vector3D operator + (const Vector3D & v1, const Vector3D & v2);
Vector3D operator - (const Vector3D & v1, const Vector3D & v2);

Vector3D CrossProduct(const Vector3D & v1, const Vector3D & v2);
float DotProduct(const Vector3D & v1, const Vector3D & v2);
float VectorMod(const Vector3D & vec);
float Distance(const Vector3D & v1, const Vector3D & v2);

extern const Vector3D gZeroVector;


class Vector4D{
	friend class Vector3D;
public:
	Vector4D():x_(0),y_(0),z_(0),t_(1) { ; }
	Vector4D(float x, float y, float z, float t = 1.0f) :x_(x), y_(y), z_(z), t_(t) { ; }
	Vector4D(const Vector3D vec):x_(vec.x_),y_(vec.y_),z_(vec.z_),t_(1.0f) {;}
public:
	Vector3D GetVector3D() { return Vector3D(x_,y_,z_); }
public:
	float x_, y_, z_,t_;
};
}
#endif // !_POINT_H_
