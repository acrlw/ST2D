#include "Matrix4x4.h"

namespace ST
{
	Matrix4x4::Matrix4x4() : data{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0 }
	{
	}

	Matrix4x4::Matrix4x4(const Vector4& col1, const Vector4& col2, const Vector4& col3, const Vector4& col4)
		: columns{ col1, col2, col3, col4 }
	{
	}
	Matrix4x4::Matrix4x4(const real& col1_x, const real& col1_y, const real& col1_z, const real& col1_w, const real& col2_x, const real& col2_y, const real& col2_z, const real& col2_w, const real& col3_x, const real& col3_y, const real& col3_z, const real& col3_w, const real& col4_x, const real& col4_y, const real& col4_z, const real& col4_w)
		: m11(col1_x), m21(col1_y), m31(col1_z), m41(col1_w),
		  m12(col2_x), m22(col2_y), m32(col2_z), m42(col2_w),
		  m13(col3_x), m23(col3_y), m33(col3_z), m43(col3_w),
	      m14(col4_x), m24(col4_y), m34(col4_z), m44(col4_w)
	{
	}
	Matrix4x4::Matrix4x4(const Matrix3x3& mat)
		: m11(mat.m11), m21(mat.m21), m31(mat.m31), m41(0),
		  m12(mat.m12), m22(mat.m22), m32(mat.m32), m42(0),
		  m13(mat.m13), m23(mat.m23), m33(mat.m33), m43(0),
		  m14(0), m24(0), m34(0), m44(0)
	{
	}

	Matrix4x4& Matrix4x4::operator=(const Matrix3x3& rhs)
	{
		columns[0] = rhs.columns[0];
		columns[1] = rhs.columns[1];
		columns[2] = rhs.columns[2];
		columns[3].clear();
		return *this;
	}

	Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs)
	{
		columns[0] = rhs.columns[0];
		columns[1] = rhs.columns[1];
		columns[2] = rhs.columns[2];
		columns[3] = rhs.columns[3];
		return *this;
	}

	Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs)
	{
		columns[0] += rhs.columns[0];
		columns[1] += rhs.columns[1];
		columns[2] += rhs.columns[2];
		columns[3] += rhs.columns[3];
		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs)
	{
		columns[0] -= rhs.columns[0];
		columns[1] -= rhs.columns[1];
		columns[2] -= rhs.columns[2];
		columns[3] -= rhs.columns[3];
		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=(const real& factor)
	{
		columns[0] *= factor;
		columns[1] *= factor;
		columns[2] *= factor;
		columns[3] *= factor;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator/=(const real& factor)
	{
		columns[0] /= factor;
		columns[1] /= factor;
		columns[2] /= factor;
		columns[3] /= factor;
		return *this;
	}

	Matrix4x4 Matrix4x4::operator*(const real& factor) const
	{
		return { columns[0] * factor, columns[1] * factor, columns[2] * factor, columns[3] * factor };
	}

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rhs) const
	{
		return {columns[0] + rhs.columns[0], columns[1] + rhs.columns[1], columns[2] + rhs.columns[2], columns[3] + rhs.columns[3]};
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rhs) const
	{
		return {columns[0] - rhs.columns[0], columns[1] - rhs.columns[1], columns[2] - rhs.columns[2], columns[3] - rhs.columns[3]};
	}

	bool Matrix4x4::operator==(const Matrix4x4& rhs) const
	{
		return columns[0] == rhs.columns[0] && columns[1] == rhs.columns[1] && columns[2] == rhs.columns[2] && columns[3] == rhs.columns[3];
	}

	Vector4 Matrix4x4::row1() const
	{
		return { columns[0].x, columns[1].x,
			columns[2].x, columns[3].x };
	}

	Vector4 Matrix4x4::row2() const
	{
		return {columns[0].y, columns[1].y,
			columns[2].y, columns[3].y};
	}

	Vector4 Matrix4x4::row3() const
	{
		return {columns[0].z, columns[1].z,
			columns[2].z, columns[3].z};
	}

	Vector4 Matrix4x4::row4() const
	{
		return {columns[0].w, columns[1].w,
			columns[2].w, columns[3].w};
	}

	real& Matrix4x4::operator[](const int& index)
	{
		assert(index >= 0 && index < 16);
		return data[index];
	}

	real Matrix4x4::operator[](const int& index) const
	{
		assert(index >= 0 && index < 16);
		return data[index];
	}

	Matrix4x4& Matrix4x4::set(const real& col1_x, const real& col1_y, const real& col1_z, const real& col1_w,
	                          const real& col2_x, const real& col2_y, const real& col2_z, const real& col2_w, const real& col3_x,
	                          const real& col3_y, const real& col3_z, const real& col3_w, const real& col4_x, const real& col4_y,
	                          const real& col4_z, const real& col4_w)
	{
		columns[0].set(col1_x, col1_y, col1_z, col1_w);
		columns[1].set(col2_x, col2_y, col2_z, col2_w);
		columns[2].set(col3_x, col3_y, col3_z, col3_w);
		columns[3].set(col4_x, col4_y, col4_z, col4_w);
		return *this;
	}

	Matrix4x4& Matrix4x4::set(const Vector4& col1, const Vector4& col2, const Vector4& col3, const Vector4& col4)
	{
		columns[0].set(col1);
		columns[1].set(col2);
		columns[2].set(col3);
		columns[3].set(col4);
		return *this;
	}

	Matrix4x4& Matrix4x4::set(const Matrix4x4& other)
	{
		columns[0].set(other.columns[0]);
		columns[1].set(other.columns[1]);
		columns[2].set(other.columns[2]);
		columns[3].set(other.columns[3]);
		return *this;
	}

	Matrix4x4& Matrix4x4::set(const Matrix3x3& other)
	{
		columns[0].set(other.columns[0]);
		columns[1].set(other.columns[1]);
		columns[2].set(other.columns[2]);
		columns[3].clear();
		return *this;
	}

	Matrix4x4& Matrix4x4::clear()
	{
		columns[0].clear();
		columns[1].clear();
		columns[2].clear();
		columns[3].clear();
		return *this;
	}

	Vector4 Matrix4x4::multiply(const Vector4& rhs) const
	{
		return multiply(*this, rhs);
	}

	Matrix4x4& Matrix4x4::multiply(const Matrix4x4& rhs)
	{
		*this = multiply(*this, rhs);
		return *this;
	}

	real Matrix4x4::determinant() const
	{
		return determinant(*this);
	}

	Matrix4x4& Matrix4x4::transpose()
	{
		realSwap(columns[0].y, columns[1].x);
		realSwap(columns[0].z, columns[2].x);
		realSwap(columns[0].w, columns[3].x);

		realSwap(columns[1].z, columns[2].y);
		realSwap(columns[1].w, columns[3].y);

		realSwap(columns[2].w, columns[3].z);
		return *this;
	}

	Matrix4x4& Matrix4x4::invert()
	{
		invert(*this);
		return *this;
	}

	Matrix4x4 Matrix4x4::identityMatrix()
	{
		return {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
	}

	Matrix4x4 Matrix4x4::multiply(const Matrix4x4& lhs, const Matrix4x4& rhs)
	{
		return { multiply(lhs, rhs.columns[0]),
			multiply(lhs, rhs.columns[1]),
			multiply(lhs, rhs.columns[2]),
			multiply(lhs, rhs.columns[3]) };
	}

	Vector4 Matrix4x4::multiply(const Matrix4x4& lhs, const Vector4& rhs)
	{
		return {
			lhs.columns[0].x * rhs.x + lhs.columns[1].x * rhs.y + lhs.columns[2].x * rhs.z + lhs.columns[3].x * rhs.w,
			lhs.columns[0].y * rhs.x + lhs.columns[1].y * rhs.y + lhs.columns[2].y * rhs.z + lhs.columns[3].y * rhs.w,
			lhs.columns[0].z * rhs.x + lhs.columns[1].z * rhs.y + lhs.columns[2].z * rhs.z + lhs.columns[3].z * rhs.w,
			lhs.columns[0].w * rhs.x + lhs.columns[1].w * rhs.y + lhs.columns[2].w * rhs.z + lhs.columns[3].w * rhs.w
		};
	}

	real Matrix4x4::determinant(const Matrix4x4& mat)
	{
		const real det11 = Matrix3x3::determinant({
			mat.columns[1].y, mat.columns[1].z, mat.columns[1].w,
		mat.columns[2].y, mat.columns[2].z, mat.columns[2].w ,
		mat.columns[3].y, mat.columns[3].z, mat.columns[3].w });

		const real det21 = -Matrix3x3::determinant({
			mat.columns[1].x, mat.columns[1].z, mat.columns[1].w,
		mat.columns[2].x, mat.columns[2].z, mat.columns[2].w ,
		mat.columns[3].x, mat.columns[3].z, mat.columns[3].w });

		const real det31 = Matrix3x3::determinant({
			mat.columns[1].x, mat.columns[1].y, mat.columns[1].w,
		mat.columns[2].x, mat.columns[2].y, mat.columns[2].w ,
		mat.columns[3].x, mat.columns[3].y, mat.columns[3].w });

		const real det41 = -Matrix3x3::determinant({
			mat.columns[1].x, mat.columns[1].y, mat.columns[1].z,
		mat.columns[2].x, mat.columns[2].y, mat.columns[2].z ,
		mat.columns[3].x, mat.columns[3].y, mat.columns[3].z });

		return mat.columns[0].x * det11 +
			mat.columns[0].y * det21 +
			mat.columns[0].z * det31 +
			mat.columns[0].w * det41;
	}

	bool Matrix4x4::invert(Matrix4x4& mat)
	{
		const real det = mat.determinant();
		if (realEqual(det, 0.0f))
			return false;

		const real det11 = Matrix3x3::determinant(
			{ mat.m22,mat.m23,mat.m24,
			mat.m32,mat.m33,mat.m34,
			mat.m42,mat.m43,mat.m44 });

		const real det12 = -Matrix3x3::determinant(
			{ mat.m12,mat.m13,mat.m14,
			mat.m32,mat.m33,mat.m34,
			mat.m42,mat.m43,mat.m44 });

		const real det13 = Matrix3x3::determinant(
			{ mat.m12,mat.m13,mat.m14,
			mat.m22,mat.m23,mat.m24,
			mat.m42,mat.m43,mat.m44 });

		const real det14 = -Matrix3x3::determinant(
			{ mat.m12,mat.m13,mat.m14,
			mat.m22,mat.m23,mat.m24,
			mat.m32,mat.m33,mat.m34 });


		const real det21 = -Matrix3x3::determinant(
			{ mat.m21,mat.m23,mat.m24,
			mat.m31,mat.m33,mat.m34,
			mat.m41,mat.m43,mat.m44 });

		const real det22 = Matrix3x3::determinant(
			{ mat.m11,mat.m13,mat.m14,
			mat.m31,mat.m33,mat.m34,
			mat.m41,mat.m43,mat.m44 });

		const real det23 = -Matrix3x3::determinant(
			{ mat.m11,mat.m13,mat.m24,
			mat.m21,mat.m23,mat.m24,
			mat.m41,mat.m43,mat.m44 });

		const real det24 = Matrix3x3::determinant(
			{ mat.m11,mat.m13,mat.m14,
			mat.m21,mat.m23,mat.m24,
			mat.m31,mat.m33,mat.m34 });


		const real det31 = Matrix3x3::determinant(
			{ mat.m21,mat.m22,mat.m24,
			mat.m31,mat.m32,mat.m34,
			mat.m41,mat.m42,mat.m44 });

		const real det32 = -Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m14,
			mat.m31,mat.m32,mat.m34,
			mat.m41,mat.m42,mat.m44 });

		const real det33 = Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m14,
			mat.m21,mat.m22,mat.m24,
			mat.m41,mat.m42,mat.m44 });

		const real det34 = -Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m14,
			mat.m21,mat.m22,mat.m24,
			mat.m31,mat.m32,mat.m34 });


		const real det41 = -Matrix3x3::determinant(
			{ mat.m21,mat.m22,mat.m23,
			mat.m31,mat.m32,mat.m33,
			mat.m41,mat.m42,mat.m43 });

		const real det42 = Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m13,
			mat.m31,mat.m32,mat.m33,
			mat.m41,mat.m42,mat.m43 });

		const real det43 = -Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m13,
			mat.m21,mat.m22,mat.m23,
			mat.m41,mat.m42,mat.m43 });

		const real det44 = Matrix3x3::determinant(
			{ mat.m11,mat.m12,mat.m13,
			mat.m21,mat.m22,mat.m23,
			mat.m31,mat.m32,mat.m33 });

		mat.set(det11, det12, det13, det14,
			det21, det22, det23, det24,
			det31, det32, det33, det34,
			det41, det42, det43, det44);

		mat /= det;
		return true;

	}
}
