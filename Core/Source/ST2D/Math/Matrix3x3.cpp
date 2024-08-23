#include "Matrix3x3.h"

namespace ST
{
	Matrix3x3::Matrix3x3() : data{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	{
	}

	Matrix3x3::Matrix3x3(const Matrix3x3& mat) : columns{ mat.columns[0], mat.columns[0], mat.columns[1] }
	{
	}


	Matrix3x3::Matrix3x3(const Vector3& col1, const Vector3& col2, const Vector3& col3)
		: columns{ col1, col2, col3 }
	{
	}

	Matrix3x3::Matrix3x3(const real& col1_x, const real& col1_y, const real& col1_z, const real& col2_x,
		const real& col2_y, const real& col2_z, const real& col3_x, const real& col3_y,
		const real& col3_z)
			: m11(col1_x), m21(col1_y), m31(col1_z), m12(col2_x), m22(col2_y), m32(col2_z), m13(col3_x), m23(col3_y), m33(col3_z)
	{
	}

	Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs)
	{
		if (&rhs == this)
			return *this;
		columns[0] = rhs.columns[0];
		columns[1] = rhs.columns[1];
		columns[2] = rhs.columns[2];
		return *this;
	}

	Matrix3x3& Matrix3x3::operator=(const Matrix2x2& rhs)
	{
		columns[0].x = rhs.columns[0].x;
		columns[0].y = rhs.columns[0].y;
		columns[0].z = 0;
		columns[1].x = rhs.columns[1].x;
		columns[1].y = rhs.columns[1].y;
		columns[1].z = 0;
		columns[2].clear();
		return *this;
	}

	Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& rhs)
	{
		columns[0] += rhs.columns[0];
		columns[1] += rhs.columns[1];
		columns[2] += rhs.columns[2];
		return *this;
	}

	Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& rhs)
	{
		columns[0] -= rhs.columns[0];
		columns[1] -= rhs.columns[1];
		columns[2] -= rhs.columns[2];
		return *this;
	}

	Matrix3x3& Matrix3x3::operator*=(const real& factor)
	{
		columns[0] *= factor;
		columns[1] *= factor;
		columns[2] *= factor;
		return *this;
	}

	Matrix3x3& Matrix3x3::operator/=(const real& factor)
	{
		assert(!realEqual(factor, 0));
		columns[0] /= factor;
		columns[1] /= factor;
		columns[2] /= factor;
		return *this;
	}

	bool Matrix3x3::operator==(const Matrix3x3& rhs) const
	{
		return columns[0] == rhs.columns[0] && columns[1] == rhs.columns[1] && columns[2] == rhs.columns[2];
	}

	Matrix3x3 Matrix3x3::operator*(const real& factor) const
	{
		return { columns[0] * factor, columns[1] * factor, columns[2] * factor };
	}

	Matrix3x3 Matrix3x3::operator+(const Matrix3x3& rhs) const
	{
		return {columns[0] + rhs.columns[0], columns[1] + rhs.columns[1], columns[2] + rhs.columns[2]};
	}

	Matrix3x3 Matrix3x3::operator-(const Matrix3x3& rhs) const
	{
		return {columns[0] - rhs.columns[0], columns[1] - rhs.columns[1], columns[2] - rhs.columns[2]};
	}

	Vector3 Matrix3x3::row1() const
	{
		return { columns[0].x, columns[1].x, columns[2].x };
	}

	Vector3 Matrix3x3::row2() const
	{
		return {columns[0].y, columns[1].y, columns[2].y};
	}

	Vector3 Matrix3x3::row3() const
	{
		return {columns[0].z, columns[1].z, columns[2].z};
	}

	real& Matrix3x3::operator[](const int& index)
	{
		assert(index >= 0 && index < 9);
		return data[index];
	}

	real Matrix3x3::operator[](const int& index) const
	{
		assert(index >= 0 && index < 9);
		return data[index];
	}

	real Matrix3x3::determinant() const
	{
		return determinant(*this);
	}

	Matrix3x3& Matrix3x3::transpose()
	{
		realSwap(columns[0].y, columns[1].x);
		realSwap(columns[0].z, columns[2].x);
		realSwap(columns[1].z, columns[2].y);
		return *this;
	}

	Matrix3x3& Matrix3x3::invert()
	{
		invert(*this);
		return *this;
	}

	Matrix3x3& Matrix3x3::clear()
	{
		columns[0].clear();
		columns[1].clear();
		columns[2].clear();
		return *this;
	}

	Matrix3x3& Matrix3x3::set(const real& col1_x, const real& col1_y, const real& col1_z, const real& col2_x,
		const real& col2_y, const real& col2_z, const real& col3_x, const real& col3_y,
		const real& col3_z)
	{
		columns[0].set(col1_x, col1_y, col1_z);
		columns[1].set(col2_x, col2_y, col2_z);
		columns[2].set(col3_x, col3_y, col3_z);
		return *this;
	}

	Matrix3x3& Matrix3x3::set(const Vector3& col1, const Vector3& col2, const Vector3& col3)
	{
		columns[0] = col1;
		columns[1] = col2;
		columns[2] = col3;
		return *this;
	}

	Matrix3x3& Matrix3x3::set(const Matrix3x3& other)
	{
		columns[0] = other.columns[0];
		columns[1] = other.columns[1];
		columns[2] = other.columns[2];
		return *this;
	}

	Vector3 Matrix3x3::multiply(const Vector3& rhs) const
	{
		return multiply(*this, rhs);
	}

	Vector2 Matrix3x3::multiply(const Vector2& rhs) const
	{
		return multiply(*this, rhs);
	}

	Matrix3x3& Matrix3x3::multiply(const Matrix3x3& rhs)
	{
		*this = multiply(*this, rhs);
		return *this;
	}

	Matrix3x3 Matrix3x3::skewSymmetricMatrix(const Vector3& v)
	{
		return {
			0, v.z, -v.y,
			-v.z, 0, v.x,
			v.y, -v.x, 0 };
	}

	Matrix3x3 Matrix3x3::identityMatrix()
	{
		return {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1 };
	}

	Matrix3x3 Matrix3x3::multiply(const Matrix3x3& lhs, const Matrix3x3& rhs)
	{
		return {
			multiply(lhs, rhs.columns[0]),
			multiply(lhs, rhs.columns[1]),
			multiply(lhs, rhs.columns[2])
		};
	}

	Vector3 Matrix3x3::multiply(const Matrix3x3& lhs, const Vector3& rhs)
	{
		return { lhs.columns[0].x * rhs.x + lhs.columns[1].x * rhs.y + lhs.columns[2].x * rhs.z,
			lhs.columns[0].y * rhs.x + lhs.columns[1].y * rhs.y + lhs.columns[2].y * rhs.z,
			lhs.columns[0].z * rhs.x + lhs.columns[1].z * rhs.y + lhs.columns[2].z * rhs.z };
	}

	Vector2 Matrix3x3::multiply(const Matrix3x3& lhs, const Vector2& rhs)
	{
		return { lhs.columns[0].x * rhs.x + lhs.columns[1].x * rhs.y,
			lhs.columns[0].y * rhs.x + lhs.columns[1].y * rhs.y };
	}

	real Matrix3x3::determinant(const Matrix3x3& mat)
	{
		return mat.columns[0].x * Vector2::crossProduct(mat.columns[1].y, mat.columns[1].z, mat.columns[2].y, mat.columns[2].z) +
			mat.columns[1].x * Vector2::crossProduct(mat.columns[2].y, mat.columns[2].z, mat.columns[0].y, mat.columns[0].z) +
			mat.columns[2].x * Vector2::crossProduct(mat.columns[0].y, mat.columns[0].z, mat.columns[1].y, mat.columns[1].z);
	}

	bool Matrix3x3::invert(Matrix3x3& mat)
	{
		const real det = mat.determinant();
		if (realEqual(det, 0.0f))
			return false;

		const real det11 = Vector2::crossProduct(mat.columns[1].y, mat.columns[1].z, mat.columns[2].y, mat.columns[2].z);
		const real det12 = Vector2::crossProduct(mat.columns[1].x, mat.columns[1].z, mat.columns[2].x, mat.columns[2].z) * -1;
		const real det13 = Vector2::crossProduct(mat.columns[1].x, mat.columns[1].y, mat.columns[2].x, mat.columns[2].y);

		const real det21 = Vector2::crossProduct(mat.columns[0].y, mat.columns[0].z, mat.columns[2].y, mat.columns[2].z) * -1;
		const real det22 = Vector2::crossProduct(mat.columns[0].x, mat.columns[0].z, mat.columns[2].x, mat.columns[2].z);
		const real det23 = Vector2::crossProduct(mat.columns[0].x, mat.columns[0].y, mat.columns[2].x, mat.columns[2].y) * -1;

		const real det31 = Vector2::crossProduct(mat.columns[0].y, mat.columns[0].z, mat.columns[1].y, mat.columns[1].z);
		const real det32 = Vector2::crossProduct(mat.columns[0].x, mat.columns[0].z, mat.columns[1].x, mat.columns[1].z) * -1;
		const real det33 = Vector2::crossProduct(mat.columns[0].x, mat.columns[0].y, mat.columns[1].x, mat.columns[1].y);

		mat.set(det11, det12, det13, det21, det22, det23, det31, det32, det33);
		mat.transpose();
		mat /= det;
		return true;
	}
}
