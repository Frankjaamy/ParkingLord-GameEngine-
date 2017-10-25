#include <assert.h>
#include <math.h>
#include "Vector.h"
#include "smmintrin.h"
namespace CYNData
{
	const Vector3D gZeroVector = Vector3D(0, 0, 0);
	void Vector3D::Normalize() {
		float temp_modulo_value = (x_*x_ + y_*y_ + z_*z_);
		ASSERTNL(temp_modulo_value > 0)
		float modulo_sqrt = sqrtf(temp_modulo_value);
		x_ /= modulo_sqrt;
		y_ /= modulo_sqrt;
		z_ /= modulo_sqrt;
	}

	Vector3D & Vector3D::operator= (const Vector3D & other_vec) {
		x_ = other_vec.x_;
		y_ = other_vec.y_;
		z_ = other_vec.z_;
		return *this;
	}

	Vector3D Vector3D::operator- () {
		return Vector3D(-x_,-y_,-z_);
	}

	Vector3D & Vector3D::operator+=(const Vector3D & vec) {
		x_ += vec.x_;
		y_ += vec.y_;
		z_ += vec.z_;
		return *this;
	}

	Vector3D & Vector3D::operator-=(const Vector3D & vec) {
		x_ -= vec.x_;
		y_ -= vec.y_;
		z_ -= vec.z_;
		return *this;
	}

	Vector3D Vector3D::operator* (float multi) {
		return Vector3D(-x_*multi, -y_*multi, -z_*multi);
	}
	Vector3D Vector3D::operator/ (float divide) {
		return Vector3D(-x_/ divide, -y_/ divide, -z_/ divide);
	}

	Vector3D & Vector3D::operator*= (float multi) {
		x_ *= multi;
		y_ *= multi;
		z_ *= multi;
		return *this;
	}
	Vector3D & Vector3D::operator/= (float divide) {
		ASSERTNL(divide!=0.0f)
		x_ /= divide;
		y_ /= divide;
		z_ /= divide;
		return *this;
	}

	bool Vector3D::operator==(const Vector3D & vec) {
		return x_ == vec.X()&&y_ == vec.Y()&&z_ == vec.Z();
	}
	bool Vector3D::operator!=(const Vector3D & vec) {
		return x_ != vec.X()||y_ != vec.Y() ||z_ != vec.Z();
	}

	Vector3D operator + (const Vector3D & v1,const Vector3D & v2) {
		return Vector3D(v1.X() + v2.X(), v1.Y() + v2.Y(), v1.Z() + v2.Z());
	}

	Vector3D operator - (const Vector3D & v1, const Vector3D & v2) {
		return Vector3D(v1.X() - v2.X(), v1.Y() - v2.Y(), v1.Z() - v2.Z());
	}

	Vector3D CrossProduct(const Vector3D & v1, const Vector3D & v2) {
		float x_result = v1.Y()*v2.Z() - v1.Z()*v2.Y();
		float y_result = v1.Z()*v2.X() - v1.X()*v2.Z();
		float z_result = v1.X()*v2.Y() - v1.Y()*v2.X();

		return Vector3D(x_result, y_result, z_result);
	}
	float DotProduct(const Vector3D & v1, const Vector3D & v2) {
		__m128 m_v1 = _mm_set_ps(0, v1.Z(), v1.Y(), v1.X());
		__m128 m_v2 = _mm_set_ps(0, v2.Z(), v2.Y(), v2.X());

		__m128 value = _mm_dp_ps(m_v1, m_v2, uint8_t(0x71));

		return value.m128_f32[0];
	}
	float VectorMod(const Vector3D & vec) {
		return sqrtf(vec.X()*vec.X() + vec.Y()*vec.Y() + vec.Z()*vec.Z());
	}
	float Distance(const Vector3D & v1, const Vector3D & v2) {
		return VectorMod(v1 - v2);
	}
}
