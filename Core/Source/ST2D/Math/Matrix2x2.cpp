#include "Matrix2x2.h"

namespace ST
{
	Matrix2x2::Matrix2x2(): data{0, 0, 0, 0}
	{
	}

	Matrix2x2::Matrix2x2(const real& radian)
	{
		set(radian);
	}


	Matrix2x2::Matrix2x2(const Vector2& col1, const Vector2& col2) : columns{ col1, col2 }
	{
	}

	Matrix2x2::Matrix2x2(const real& col1_x, const real& col1_y, const real& col2_x, const real& col2_y)
		: m11(col1_x), m21(col1_y), m12(col2_x), m22(col2_y)
	{
	}

	Matrix2x2::Matrix2x2(const Matrix2x2& mat) : columns{ mat.columns[0], mat.columns[1] }
	{
	}

	Matrix2x2& Matrix2x2::operator=(const Matrix2x2& rhs)
	{
		if (&rhs == this)
			return *this;
		columns[0] = rhs.columns[0];
		columns[1] = rhs.columns[1];
		return *this;
	}

	Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& rhs)
	{
		columns[0] += rhs.columns[0];
		columns[1] += rhs.columns[1];
		return *this;
	}

	Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& rhs)
	{
		columns[0] -= rhs.columns[0];
		columns[1] -= rhs.columns[1];
		return *this;
	}

	Matrix2x2& Matrix2x2::operator*=(const real& factor)
	{
		columns[0] *= factor;
		columns[1] *= factor;
		return *this;
	}

	Matrix2x2& Matrix2x2::operator/=(const real& factor)
	{
		assert(!realEqual(factor, 0));
		columns[0] /= factor;
		columns[1] /= factor;
		return *this;
	}

	bool Matrix2x2::operator==(const Matrix2x2& rhs) const
	{
		return columns[0] == rhs.columns[0] && columns[1] == rhs.columns[1];
	}

	real& Matrix2x2::operator[](const int& index)
	{
		assert(index >= 0 && index < 4);
		return data[index];
	}

	real Matrix2x2::operator[](const int& index) const
	{
		assert(index >= 0 && index < 4);
		return data[index];
	}

	Matrix2x2 Matrix2x2::operator+(const Matrix2x2& rhs) const
	{
		return { columns[0] + rhs.columns[0], columns[1] + rhs.columns[1] };
	}

	Matrix2x2 Matrix2x2::operator-(const Matrix2x2& rhs) const
	{
		return { columns[0] - rhs.columns[0], columns[1] - rhs.columns[1] };
	}

	Matrix2x2 Matrix2x2::operator*(const real& factor) const
	{
		Matrix2x2 result = *this;
		result *= factor;
		return result;
	}

	Vector2 Matrix2x2::row1() const
	{
		return { columns[0].x, columns[1].x };
	}

	Vector2 Matrix2x2::row2() const
	{
		return { columns[0].y, columns[1].y };
	}

	real Matrix2x2::determinant() const
	{
		return determinant(*this);
	}

	Matrix2x2& Matrix2x2::transpose()
	{
		realSwap(columns[0].y, columns[1].x);
		return *this;
	}

	Matrix2x2& Matrix2x2::invert()
	{
		invert(*this);
		return *this;
	}

	Matrix2x2& Matrix2x2::multiply(const Matrix2x2& rhs)
	{
		*this = multiply(*this, rhs);
		return *this;
	}

	Vector2 Matrix2x2::multiply(const Vector2& rhs) const
	{
		return multiply(*this, rhs);
	}

	Matrix2x2& Matrix2x2::clear()
	{
		columns[0].clear();
		columns[1].clear();
		return *this;
	}

	Matrix2x2& Matrix2x2::set(const real& col1_x, const real& col1_y, const real& col2_x, const real& col2_y)
	{
		columns[0].set(col1_x, col1_y);
		columns[1].set(col2_x, col2_y);
		return *this;
	}

	Matrix2x2& Matrix2x2::set(const Vector2& col1, const Vector2& col2)
	{
		columns[0] = col1;
		columns[1] = col2;
		return *this;
	}

	Matrix2x2& Matrix2x2::set(const Matrix2x2& other)
	{
		columns[0] = other.columns[0];
		columns[1] = other.columns[1];
		return *this;
	}

	Matrix2x2& Matrix2x2::set(const real& radian)
	{
		const real c = Math::cosx(radian);
		const real s = Math::sinx(radian);
		columns[0].set(c, s);
		columns[1].set(-s, c);
		return *this;
	}

	Matrix2x2 Matrix2x2::fromRadian(const real& radian)
	{
		return { radian };
	}

	Matrix2x2 Matrix2x2::skewSymmetricMatrix(const Vector2& r)
	{
		return { 0, -r.y, r.x, 0 };
	}

	Matrix2x2 Matrix2x2::identityMatrix()
	{
		return { 1, 0, 0, 1 };
	}

	Vector2 Matrix2x2::multiply(const Matrix2x2& lhs, const Vector2& rhs)
	{
		return { lhs.columns[0].x * rhs.x + lhs.columns[1].x * rhs.y, lhs.columns[0].y * rhs.x + lhs.columns[1].y * rhs.y };
	}

	Matrix2x2 Matrix2x2::multiply(const Matrix2x2& lhs, const Matrix2x2& rhs)
	{
		return { lhs.columns[0].x * rhs.columns[0].x + lhs.columns[1].x * rhs.columns[0].y,
			lhs.columns[0].y * rhs.columns[0].x + lhs.columns[1].y * rhs.columns[0].y,
			lhs.columns[0].x * rhs.columns[1].x + lhs.columns[1].x * rhs.columns[1].y,
			lhs.columns[0].y * rhs.columns[1].x + lhs.columns[1].y * rhs.columns[1].y };
	}

	real Matrix2x2::determinant(const Matrix2x2& mat)
	{
		return mat.columns[0].x * mat.columns[1].y - mat.columns[1].x * mat.columns[0].y;
	}

	bool Matrix2x2::invert(Matrix2x2& mat)
	{
		const real det = mat.determinant();

		if (realEqual(det, 0))
			return false;

		realSwap(mat.columns[0].x, mat.columns[1].y);
		mat.columns[0].y *= -1;
		mat.columns[1].x *= -1;
		mat /= det;
		return true;
	}
}
