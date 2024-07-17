#pragma once

#include "Complex.h"
#include "Matrix2x2.h"

namespace ST
{
	struct ST_API Complex
	{
		Complex(const real& _re, const real& _im);
		Complex(const Complex& copy);
		Complex(const Vector2& vec);
		Complex(const real& radians);
		Complex& operator=(const Complex& copy);
		Complex(Complex&& other) = default;


		Complex operator+(const Complex& rhs) const;
		Complex operator-(const Complex& rhs) const;
		Complex operator-() const;
		Complex operator*(const int& factor) const;
		Complex operator*(const real& factor) const;
		Complex operator/(const real& factor) const;
		Complex operator/(const int& factor) const;

		Complex& operator+=(const Complex& rhs);
		Complex& operator-=(const Complex& rhs);
		Complex& operator*=(const real& factor);
		Complex& operator*=(const int& factor);
		Complex& operator/=(const real& factor);
		Complex& operator/=(const int& factor);

		bool operator==(const Complex& rhs) const;
		bool operator!=(const Complex& rhs) const;

		bool equal(const Complex& rhs) const;
		bool fuzzyEqual(const Complex& rhs, const real& epsilon = Constant::GeometryEpsilon) const;
		bool isOrigin(const real& epsilon = Constant::GeometryEpsilon) const;
		bool isSameQuadrant(const Complex& rhs) const;

		real lengthSquare() const;
		real length() const;
		real theta() const;
		Complex normal() const;
		Complex negative() const;

		Complex& set(const real& _re, const real& _im);
		Complex& set(const Complex& copy);
		Complex& clear();
		Complex& negate();
		Complex& swap(Complex& other) noexcept;

		Complex& normalize();
		Complex perpendicular() const;
		Complex& matchSign(const Complex& rhs);

		Vector2 multiply(const Vector2& vec) const;

		Complex& dot(const Complex& rhs);
		Complex conjugate() const;

		static Vector2 dotProduct(const Complex& lhs, const Vector2& rhs);
		static Complex dotProduct(const Complex& lhs, const Complex& rhs);
		static Complex slerp(const Complex& start, const Complex& end, const real& t);

		Matrix2x2 toMatrix() const;

		real re;
		real im;
	};
}