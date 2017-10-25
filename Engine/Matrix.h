#pragma once
#include "Vector.h"
extern const float PI;
namespace Matrix {
	union Matrix4x4
	{
	public:
		Matrix4x4() :matrix_data_() { ; }
		Matrix4x4(const float * matrixData, const size_t matrixDataLength);
		Matrix4x4(const Matrix4x4 & matrix);
	public:
		inline float getMatrixValue(int row, int column) { return matrix_data_[column * 4 + row]; }
	public:

		float CalDet(float * data, int size = 9);
		float Det();
		Matrix4x4 GetAdjMatrix();
		Matrix4x4 Inverse();
		Matrix4x4 Transpose();
		void TransposingOperation(int index, float * i_matrix, bool * i_matrix_flag, int matrix_size = 16);
	public:
		float operator[] (unsigned int index) const;
		Matrix4x4 & operator = (const Matrix4x4 & m);
	public:
		void PrintMatrix();
	private:
		int GetNextDuringTransPosing(int index);
		int GetPrevDuringTransPosing(int index);
	private:
		// no need to use __m128 here....Too annoying when it comes to calculations...
		// in unreal they just use a [16] float array.
		// data stored in the order of column 
		float matrix_data_[16];
		__m128 matrix_data_sde_[4];
	};


	Matrix4x4 operator + (const Matrix4x4 & m1, const Matrix4x4 & m2);
	Matrix4x4 operator - (const Matrix4x4 & m1, const Matrix4x4 & m2);
	Matrix4x4 operator * (const Matrix4x4 & m1, const Matrix4x4 & m2);
	Matrix4x4 operator / (const Matrix4x4 & m1, const float value);

	CYNData::Vector4D operator * (const CYNData::Vector4D & vec, const Matrix4x4 & matrix);


	Matrix4x4 GetRotationMatrix(const CYNData::Vector3D rotation);
	Matrix4x4 GetTrasnsformMatrix(const CYNData::Vector3D move);
};
