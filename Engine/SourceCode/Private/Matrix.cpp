#include "Matrix.h"
#include"GLib\GLib.h"
#include <assert.h>
#include<iostream>
#include <iomanip>
#include <math.h>
const size_t MATRIX4X4_MAX_DEMENSION_SIZE = 4;
const size_t MATRIX4X4_MAX_FLOAT_SIZE = 16;
using namespace std;
namespace Matrix {
	Matrix4x4::Matrix4x4(const float * matrixData, const size_t matrixDataLength) {
		assert(matrixDataLength == MATRIX4X4_MAX_FLOAT_SIZE, "the data size is wrong!");

		for (int i = 0; i < 16; ++i) {
			matrix_data_[i] = matrixData[i];
		}
	}

	Matrix4x4::Matrix4x4(const Matrix4x4 & matrix) {
		for (int i = 0; i < 16; ++i) {
			matrix_data_[i] = matrix.matrix_data_[i];
		}
	}

	int Matrix4x4::GetNextDuringTransPosing(int i_index) {
		int colomn = i_index / 4;
		int row = i_index % 4;
		return row * 4 + colomn;
	}

	int Matrix4x4::GetPrevDuringTransPosing(int i_index) {
		int colomn = i_index / 4;
		int row = i_index % 4;
		return row * 4 + colomn;
	}
	float Matrix4x4::CalDet(float * data, int size) {

		return  data[0] * data[4] * data[8]
			+ data[3] * data[7] * data[2]
			+ data[6] * data[1] * data[5]
			- data[5] * data[7] * data[0]
			- data[8] * data[1] * data[3]
			- data[2] * data[4] * data[6]
			;
	}
	float Matrix4x4::Det() {
		float result = 0;
		int row = 0;
		for (int col = 0; col < 4; ++col) {
			float element = matrix_data_[col * 4 + row];
			float temp_data[9] = { 0 };
			int index = 0;

			for (int i = 0; i < 16; ++i) {
				if (i / 4 == col || i % 4 == row) {
					continue;
				}
				else {
					temp_data[index] = matrix_data_[i];
					index++;
				}
			}
			if ((col + row) % 2 == 0)
				result += element * CalDet(temp_data);
			else
				result -= element * CalDet(temp_data);
		}
		return result;
	}
	Matrix4x4 Matrix4x4::GetAdjMatrix() {
		float matrix_data[16] = { 0 };
		float tempMatrix[16];
		memcpy_s(tempMatrix, 16 * sizeof(float), matrix_data_, 16 * sizeof(float));

		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				float temp_data[9] = { 0 };
				int index = 0;
				for (int i = 0; i < 16; ++i) {
					if (i / 4 == col || i % 4 == row) {
						continue;
					}
					else {
						temp_data[index] = matrix_data_[i];
						index++;
					}
				}
				matrix_data[col * 4 + row] = CalDet(temp_data) * (((col + row) % 2 == 0) ? 1 : -1);
			}
		}

		return Matrix4x4(matrix_data, 16).Transpose();
	}

	Matrix4x4 Matrix4x4::Inverse() {
		float Det = this->Det();
		if (Det == 0) {
			//return a default matrix. For Now.
			return Matrix4x4();
		}
		else {
			Matrix4x4 matrix_adj = this->GetAdjMatrix();
			return matrix_adj / Det;
		}
	}

	Matrix4x4 Matrix4x4::Transpose() {
		float matrix_data[16];
		memcpy_s(matrix_data, 16 * sizeof(float), matrix_data_, 16 * sizeof(float));
		bool matrix_flag[16] = { false };

		for (int i = 0; i < 16; ++i) {
			if (!matrix_flag[i]) {
				TransposingOperation(i, matrix_data, matrix_flag);
			}
		}
		return Matrix4x4(matrix_data, 16);
	}

	void Matrix4x4::TransposingOperation(int index, float * i_matrix, bool * i_matrix_flag, int matrix_size) {
		float temp_value = i_matrix[index];
		int index_prev = GetPrevDuringTransPosing(index);
		while (index_prev != index) {
			i_matrix[index] = i_matrix[index_prev];
			i_matrix_flag[index] = true;
			int temp_prev = GetPrevDuringTransPosing(index_prev);
			if (temp_prev == index) {
				break;
			}
			else {
				index_prev = temp_prev;
			}
		}
		i_matrix[index_prev] = temp_value;
		i_matrix_flag[index_prev] = true;
	}

	void Matrix4x4::PrintMatrix() {
		for (int i = 0; i < 4; ++i) {
			cout << "| ";
			for (int j = 0; j < 4; ++j) {
				cout << setw(4) << matrix_data_[j * 4 + i] << " ";
			}
			cout << "|" << endl;
		}
		cout << "\n" << "\n" << endl;
	}
	//----------------------Operator--------------------------//
	//--------------------------------------------------------//
	float Matrix4x4::operator[] (unsigned int index) const {
		return matrix_data_[index];
	}
	Matrix4x4 & Matrix4x4::operator = (const Matrix4x4 & m) {
		for (unsigned int i = 0; i < 16; ++i) {
			matrix_data_[i] = m.matrix_data_[i];
		}
		return *this;
	}
	Matrix4x4 operator +(const Matrix4x4 & max1, const Matrix4x4 & max2) {
		float matrixData[16] = {};
		for (unsigned int i = 0; i < 16; i += 4) {
			__m128 m1 = _mm_set_ps(max1[i + 3], max1[i + 2], max1[i + 1], max1[i]);
			__m128 m2 = _mm_set_ps(max2[i + 3], max2[i + 2], max2[i + 1], max2[i]);
			m1 = _mm_add_ps(m1, m2);
			
			matrixData[i] = m1.m128_f32[0];
			matrixData[i + 1] = m1.m128_f32[1];
			matrixData[i + 2] = m1.m128_f32[2];
			matrixData[i + 3] = m1.m128_f32[3];
		}

		return Matrix4x4(matrixData, 16);
	}

	Matrix4x4 operator -(const Matrix4x4 & max1, const Matrix4x4 & max2) {
		float matrixData[16] = {};
		for (unsigned int i = 0; i < 16; i += 4) {
			__m128 m1 = _mm_set_ps(max1[i + 3], max1[i + 2], max1[i + 1], max1[i]);
			__m128 m2 = _mm_set_ps(-max2[i + 3], -max2[i + 2], -max2[i + 1], -max2[i]);
			m1 = _mm_add_ps(m1, m2);

			matrixData[i] = m1.m128_f32[0];
			matrixData[i + 1] = m1.m128_f32[1];
			matrixData[i + 2] = m1.m128_f32[2];
			matrixData[i + 3] = m1.m128_f32[3];
		}
		return Matrix4x4(matrixData, 16);
	}

	Matrix4x4 operator *(const Matrix4x4 & max1, const Matrix4x4 & max2) {
		float matrixData[16] = {};
		for (int i = 0; i < 4; ++i) {
			__m128 m1 = _mm_set_ps(max1[i % 4], max1[i % 4 + 4 * 1], max1[i % 4 + 4 * 2], max1[i % 4 + 4 * 3]);
			for (int j = 0; j < 4; ++j) {
				__m128 m2 = _mm_set_ps(max2[j * 4], max2[j * 4 + 1], max2[j * 4 + 2], max2[j * 4 + 3]);
				__m128 m3 = _mm_mul_ps(m1, m2);
				matrixData[i + j * 4] = m3.m128_f32[3] + m3.m128_f32[2] + m3.m128_f32[1] + m3.m128_f32[0];
			}
		}
		return Matrix4x4(matrixData, 16);
	}
	Matrix4x4 operator / (const Matrix4x4 & m, const float value) {
		ASSERTNL(value != 0.0f);
		float matrixData[16];
		for (int i = 0; i < 16; ++i) {
			matrixData[i] = m[i];
			matrixData[i] /= value;
		}
		return Matrix4x4(matrixData, 16);
	}

	CYNData::Vector4D operator * (const CYNData::Vector4D & vec, const Matrix4x4 & matrix) {
		float vec4d_data[4] = { 0 };
		for (int i = 0; i < 16; i += 4) {
			__m128 m1 = _mm_set_ps(vec.x_, vec.y_, vec.z_, vec.t_);
			__m128 m2 = _mm_set_ps(matrix[i + 0], matrix[i + 1], matrix[i + 2], matrix[i + 3]);

			__m128 result = _mm_mul_ps(m1, m2);
			vec4d_data[i / 4] = result.m128_f32[3] + result.m128_f32[2] + result.m128_f32[1] + result.m128_f32[0];
		}
		return CYNData::Vector4D(vec4d_data[0],vec4d_data[1],vec4d_data[2]);
	}

	Matrix4x4 GetTrasnsformMatrix(const CYNData::Vector3D move) {
		float matrixData_move[16] = {
			1,0,0,move.X(),
			0,1,0,move.Y(),
			0,0,1,move.Z(),
			0,0,0,1
		};
		return Matrix4x4(matrixData_move, 16);
	}
	Matrix4x4 GetRotationMatrix(const CYNData::Vector3D rotation) {
		float yaw = rotation.Z();
		float roll = rotation.X();
		float pitch = rotation.Y();
		float matrixData_yaw[16] = 
		{
			cosf(yaw),	-sinf(yaw),0,0,
			sinf(yaw),	cosf(yaw),0,0,
			0,				0			,1,0,
			0,				0			,0,1
		};
		float matrixData_roll[16] =
		{
			1,0,0,0,
			0,cosf(roll),-sinf(roll),0,
			0,sinf(roll),cosf(roll),0,
			0,0,0,1
		};
		float matrixData_pitch[16] =
		{
			cosf(pitch),0,sinf(pitch),0,
			0,1,0,0,
			-sinf(pitch),0,cosf(pitch),0,
			0,0,0,1
		};
		return Matrix4x4(matrixData_yaw, 16)*Matrix4x4(matrixData_pitch, 16)*Matrix4x4(matrixData_roll, 16);
	}
}
