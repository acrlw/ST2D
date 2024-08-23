#include "Vector2.h"

namespace ST
{
	Vector2::Vector2(const real& _x, const real& _y) : x(_x), y(_y)
	{
		assert(!std::isnan(x));
		assert(!std::isnan(y));
	}

	Vector2::Vector2(const Vector2& copy) : x(copy.x), y(copy.y)
	{
		assert(!std::isnan(x));
		assert(!std::isnan(y));
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		return { x + rhs.x, y + rhs.y };
	}

	Vector2 Vector2::operator-(const Vector2& rhs) const
	{
		return {x - rhs.x, y - rhs.y};
	}
	Vector2 Vector2::operator-()const
	{
		return {-x, -y};
	}
	Vector2 Vector2::operator*(const int& factor) const
	{
		return {x * factor, y * factor};
	}

	Vector2 Vector2::operator/(const real& factor) const
	{
		assert(!realEqual(factor, 0));
		return { x / factor, y / factor };
	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector2& Vector2::operator*=(const real& factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	Vector2& Vector2::operator/=(const real& factor)
	{
		assert(!realEqual(factor, 0));
		x /= factor;
		y /= factor;
		return *this;
	}

	bool Vector2::operator==(const Vector2& rhs) const
	{
		return realEqual(x, rhs.x) && realEqual(y, rhs.y);
	}

	bool Vector2::operator!=(const Vector2& rhs) const
	{
		return !realEqual(x, rhs.x) || !realEqual(y, rhs.y);
	}

	real Vector2::lengthSquare() const
	{
		return x * x + y * y;
	}

	real Vector2::length() const
	{
		return std::sqrt(lengthSquare());
	}

	real Vector2::theta() const
	{
		return Math::arctanx(y, x);
	}

	Vector2& Vector2::set(const real& _x, const real& _y)
	{
		x = _x;
		y = _y;
		return *this;
	}

	Vector2& Vector2::set(const Vector2& copy)
	{
		x = copy.x;
		y = copy.y;
		return *this;
	}

	Vector2& Vector2::clear()
	{
		x = 0.0f;
		y = 0.0f;
		return *this;
	}

	Vector2& Vector2::negate()
	{
		x *= -1.0f;
		y *= -1.0f;
		return *this;
	}

	Vector2& Vector2::swap(Vector2& other) noexcept
	{
		realSwap(x, other.x);
		realSwap(y, other.y);
		return *this;
	}

	Vector2& Vector2::normalize()
	{
		const real length_inv = 1.0f / std::sqrt(lengthSquare());
		assert(!std::isinf(length_inv));

		//const real length_inv = Math::fastInverseSqrt<real>(lengthSquare());
		x *= length_inv;
		y *= length_inv;

		return *this;
	}

	Vector2 Vector2::normal() const
	{
		return Vector2(*this).normalize();
	}

	Vector2 Vector2::negative() const
	{
		return Vector2(-x, -y);
	}

	real Vector2::distance(const Vector2& rhs) const
	{
		return (*this - rhs).length();
	}

	bool Vector2::equal(const Vector2& rhs) const
	{
		return realEqual(x, rhs.x) && realEqual(y, rhs.y);
	}

	bool Vector2::fuzzyEqual(const Vector2& rhs, const real& epsilon)const
	{
		return fuzzyRealEqual(x, rhs.x, epsilon) && fuzzyRealEqual(y, rhs.y, epsilon);
	}

	bool Vector2::isOrigin(const real& epsilon) const
	{
		return fuzzyEqual({ 0, 0 }, epsilon);
	}

	bool Vector2::isSameQuadrant(const Vector2& rhs) const
	{
		return Math::sameSign(x, rhs.x) && Math::sameSign(y, rhs.y);
	}

	real Vector2::dot(const Vector2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	real Vector2::cross(const Vector2& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}

	real Vector2::distance(const Vector2& lhs, const Vector2& rhs)
	{
		return (lhs - rhs).length();
	}

	Vector2& Vector2::matchSign(const Vector2& rhs)
	{
		x = std::abs(x);
		y = std::abs(y);
		if (rhs.x < 0)
			x = -x;
		if (rhs.y < 0)
			y = -y;
		return *this;
	}

	real& Vector2::operator[](const int& index)
	{
		assert(index >= 0 && index < 2);
		return data[index];
	}

	real Vector2::operator[](const int& index) const
	{
		assert(index >= 0 && index < 2);
		return data[index];
	}

	Vector2 Vector2::perpendicular() const
	{
		return { -y, x };
	}

	real Vector2::dotProduct(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	real Vector2::crossProduct(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	real Vector2::crossProduct(const real& x1, const real& y1, const real& x2, const real& y2)
	{
		return x1 * y2 - x2 * y1;
	}

	Vector2 Vector2::crossProduct(const real& lhs, const Vector2& rhs)
	{
		return { -rhs.y * lhs, rhs.x * lhs };
	}

	Vector2 Vector2::crossProduct(const Vector2& lhs, const real& rhs)
	{
		return {lhs.y * rhs, -lhs.x * rhs };
	}

	Vector2 Vector2::lerp(const Vector2& lhs, const Vector2& rhs, const real& t)
	{
		return lhs + (rhs - lhs) * t;
	}

	Vector2& Vector2::operator/=(const int& factor)
	{
		assert(!realEqual(factor, 0));
		x /= factor;
		y /= factor;
		return *this;
	}

	Vector2& Vector2::operator*=(const int& factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	Vector2 Vector2::operator/(const int& factor) const
	{
		assert(!realEqual(factor, 0));
		return {x / factor, y / factor};
	}

	Vector2 Vector2::operator*(const real& factor) const
	{
		return {x * factor, y * factor};
	}

	Vector2& Vector2::operator=(const Vector2& copy)
	{
		if (&copy == this)
			return *this;
		x = copy.x;
		y = copy.y;
		return *this;
	}
}
