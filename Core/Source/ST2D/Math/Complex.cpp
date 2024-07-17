#include "Complex.h"

namespace ST
{
	Complex::Complex(const real& _re, const real& _im) : re(_re), im(_im)
	{

	}

	Complex::Complex(const Complex& copy): re(copy.re), im(copy.im)
	
	{

	}

	Complex::Complex(const Vector2& vec) : re(vec.x), im(vec.y)
	{

	}

	Complex::Complex(const real& radians)
	{
		re = std::cos(radians);
		im = std::sin(radians);
	}

	Complex& Complex::operator=(const Complex& copy)
	{
		if (&copy == this)
			return *this;
		re = copy.re;
		im = copy.im;
		return *this;
	}

	Complex Complex::operator+(const Complex& rhs) const
	{
		return Complex(re + rhs.re, im + rhs.im);
	}

	Complex Complex::operator-(const Complex& rhs) const
	{
		return Complex(re - rhs.re, im - rhs.im);
	}

	Complex Complex::operator-() const
	{
		return Complex(-re, -im);
	}

	Complex Complex::operator*(const int& factor) const
	{
		return Complex(re * factor, im * factor);
	}

	Complex Complex::operator*(const real& factor) const
	{
		return Complex(re * factor, im * factor);
	}

	Complex Complex::operator/(const real& factor) const
	{
		assert(!realEqual(factor, 0));
		return Complex(re / factor, im / factor);
	}

	Complex Complex::operator/(const int& factor) const
	{
		assert(factor != 0);
		return Complex(re / factor, im / factor);
	}

	Complex& Complex::operator+=(const Complex& rhs)
	{
		if (&rhs == this)
			return *this;

		re += rhs.re;
		im += rhs.im;

		return *this;
	}

	Complex& Complex::operator-=(const Complex& rhs)
	{
		if (&rhs == this)
			return *this;

		re -= rhs.re;
		im -= rhs.im;

		return *this;
	}

	Complex& Complex::operator*=(const real& factor)
	{
		re *= factor;
		im *= factor;

		return *this;
	}

	Complex& Complex::operator*=(const int& factor)
	{
		re *= factor;
		im *= factor;

		return *this;
	}

	Complex& Complex::operator/=(const real& factor)
	{
		assert(!realEqual(factor, 0));
		re /= factor;
		im /= factor;

		return *this;
	}

	Complex& Complex::operator/=(const int& factor)
	{
		assert(factor != 0);
		re /= factor;
		im /= factor;

		return *this;
	}

	bool Complex::operator==(const Complex& rhs) const
	{
		return realEqual(re, rhs.re) && realEqual(im, rhs.im);
	}

	bool Complex::operator!=(const Complex& rhs) const
	{
		return !realEqual(re, rhs.re) || !realEqual(im, rhs.im);
	}

	bool Complex::equal(const Complex& rhs) const
	{
		return realEqual(re, rhs.re) && realEqual(im, rhs.im);
	}

	bool Complex::fuzzyEqual(const Complex& rhs, const real& epsilon) const
	{
		return fuzzyRealEqual(re, rhs.re, epsilon) && fuzzyRealEqual(im, rhs.im, epsilon);
	}

	bool Complex::isOrigin(const real& epsilon) const
	{
		return fuzzyRealEqual(re, 0, epsilon) && fuzzyRealEqual(im, 0, epsilon);
	}

	bool Complex::isSameQuadrant(const Complex& rhs) const
	{
		return Math::sameSign(re, rhs.re) && Math::sameSign(im, rhs.im);
	}

	real Complex::lengthSquare() const
	{
		return re * re + im * im;
	}

	real Complex::length() const
	{
		return std::sqrt(lengthSquare());
	}

	real Complex::theta() const
	{
		return Math::arctanx(im, re);
	}

	Complex Complex::normal() const
	{
		Complex result(re, im);
		if(!realEqual(result.length(), 1.0f))
			result.normalize();
		return result;
	}

	Complex Complex::negative() const
	{
		return { -re, -im };
	}

	Complex& Complex::set(const real& _re, const real& _im)
	{
		re = _re;
		im = _im;
		return *this;
	}

	Complex& Complex::set(const Complex& copy)
	{
		re = copy.re;
		im = copy.im;
		return *this;
	}

	Complex& Complex::clear()
	{
		re = 0.0f;
		im = 0.0f;
		return *this;
	}

	Complex& Complex::negate()
	{
		re = -re;
		im = -im;
		return *this;
	}

	Complex& Complex::swap(Complex& other) noexcept
	{
		if (&other == this)
			return *this;

		re = std::exchange(other.re, re);
		im = std::exchange(other.im, im);
		return *this;
	}

	Complex& Complex::normalize()
	{
		float ls = length();
		assert(!realEqual(ls, 0));
		re /= ls;
		im /= ls;

		return *this;
	}

	Complex Complex::perpendicular() const
	{
		return Complex(-im, re);
	}

	Complex& Complex::matchSign(const Complex& rhs)
	{
		if(&rhs == this)
			return *this;

		re = static_cast<real>(Math::sign(rhs.re)) * std::abs(re);
		im = static_cast<real>(Math::sign(rhs.im)) * std::abs(im);
		return *this;
	}

	Vector2 Complex::multiply(const Vector2& vec) const
	{
		return { re * vec.x - im * vec.y, im * vec.x + re * vec.y };
	}

	Complex& Complex::dot(const Complex& rhs)
	{
		*this = dotProduct(*this, rhs);
		return *this;
	}

	Complex Complex::conjugate() const
	{
		return { re, -im };
	}

	Complex Complex::dotProduct(const Complex& lhs, const Complex& rhs)
	{
		return { lhs.re * rhs.im - lhs.im * rhs.re, lhs.im * rhs.re + lhs.re * rhs.im };
	}

	Complex Complex::slerp(const Complex& start, const Complex& end, const real& t)
	{
		real realT = Math::clamp(t, 0.0f, 1.0f);
		Complex nStart = start.normal();
		Complex nEnd = end.normal();
		float rStart = std::acos(nStart.re);
		float rEnd = std::acos(nEnd.re);
		return { rStart * (1 - realT) + rEnd * realT };
	}

	Matrix2x2 Complex::toMatrix() const
	{
		Matrix2x2 result(re, im, -im, re);
		return result;
	}
}
