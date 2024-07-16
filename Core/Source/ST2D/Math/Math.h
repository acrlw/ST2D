#pragma once

#include "ST2D/Core.h"
#include "immintrin.h"
#include "xmmintrin.h"

namespace ST
{
	//basic real utilities
	ST_API inline void realSwap(real& lhs, real& rhs)
	{
		const real temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	ST_API inline bool fuzzyRealEqual(const real& lhs, const real& rhs,
		const real& epsilon = Constant::GeometryEpsilon)
	{
		return std::fabs(lhs - rhs) < epsilon;
	}

	ST_API inline bool realEqual(const real& lhs, const real& rhs)
	{
		return fuzzyRealEqual(lhs, rhs, Constant::GeometryEpsilon);
	}

	class Math
	{
	public:
		ST_API static real bernstein(const real& t, const real& i, const real& n)
		{
			return combination(n, i) * std::pow(t, i) * std::pow(1.0f - t, n - i);
		}

		ST_API static real dBernstein(const real& t, const real& i, const real& n)
		{
			if (i == 0)
				return -1.0f * n * std::pow(1.0f - t, n - 1);

			if (i == n)
				return n * std::pow(t, n - 1);

			return combination(n, i) * (i * std::pow(t, i - 1) * std::pow(1.0f - t, n - i) -
				(n - i) * std::pow(t, i) * std::pow(1.0f - t, n - i - 1));
		}

		ST_API static real d2Bernstein(const real& t, const real& i, const real& n)
		{
			if (i == 0)
				return n * (n - 1) * std::pow(1.0f - t, n - 2.0f);

			if (i == 1)
				return combination(n, 1) * (-2.0f * (n - 1.0f) * std::pow(1.0f - t, n - 2.0f) +
					(n - 1.0f) * (n - 2.0f) * t * std::pow(1.0f - t, n - 3.0f));

			if (i == 2)
				return combination(n, 2) * (2.0f * std::pow(1.0f - t, n - 2.0f)
					- 4.0f * (n - 2.0f) * t * std::pow(1.0f - t, n - 3.0f) +
					(n - 2.0f) * (n - 3.0f) * t * t * std::pow(1.0f - t, n - 3.0f));

			if (i == n)
				return n * (n - 1) * std::pow(t, n - 2.0f);


			return combination(n, i) * (i * (i - 1.0f) * std::pow(t, i - 2.0f) * std::pow(1.0f - t, n - i)
				- 2.0f * i * (n - i) * std::pow(t, i - 1.0f) * std::pow(1.0f - t, n - i - 1.0f) +
				(n - i) * (n - i - 1.0f) * std::pow(t, i) * std::pow(1.0f - t, n - i - 2.0f));
		}

		ST_API static real combination(const real& n, const real& m)
		{
			real a = 1.0f, b = 1.0f, c = 1.0f;
			for (real i = n; i > 0; i -= 1.0f)
				a *= i;
			for (real i = m; i > 0; i -= 1.0f)
				b *= i;
			for (real i = n - m; i > 0; i -= 1.0f)
				c *= i;
			return a / (b * c);
		}

		//trigonometric function
		ST_API static real abs(const real& x)
		{
			return std::fabs(x);
		}

		ST_API static real sinx(const real& x)
		{
			return std::sin(x);
		}

		ST_API static real cosx(const real& x)
		{
			return std::cos(x);
		}

		ST_API static real tanx(const real& x)
		{
			return std::tan(x);
		}

		ST_API static real arcsinx(const real& x)
		{
			return std::asin(x);
		}

		ST_API static real arccosx(const real& x)
		{
			return std::acos(x);
		}

		ST_API static real arctanx(const real& y, const real& x)
		{
			return std::atan2(y, x);
		}

		ST_API static real max(const real& a, const real& b)
		{
			return std::max(a, b);
		}

		ST_API static real min(const real& a, const real& b)
		{
			return std::min(a, b);
		}

		ST_API static real tripleMin(const real& a, const real& b, const real& c)
		{
			return std::min(std::min(a, b), c);
		}

		ST_API static real tripleMax(const real& a, const real& b, const real& c)
		{
			return std::max(std::max(a, b), c);
		}

		ST_API static real absMax(const real& a, const real& b)
		{
			return std::max(std::fabs(a), std::fabs(b));
		}

		ST_API static real absMin(const real& a, const real& b)
		{
			return std::min(std::fabs(a), std::fabs(b));
		}

		ST_API static real sqrt(const real& x)
		{
			return std::sqrt(x);
		}

		ST_API static real pow(const real& x, const real& e)
		{
			return std::pow(x, e);
		}

		//other
		ST_API static bool sameSign(const real& a, const real& b)
		{
			return a >= 0 && b >= 0 || a <= 0 && b <= 0;
		}

		ST_API static bool sameSign(const real& a, const real& b, const real& c)
		{
			return a >= 0 && b >= 0 && c >= 0 || a <= 0 && b <= 0 && c <= 0;
		}

		ST_API static bool sameSignStrict(const real& a, const real& b)
		{
			return a > 0 && b > 0 || a < 0 && b < 0;
		}

		ST_API static bool sameSignStrict(const real& a, const real& b, const real& c)
		{
			return a > 0 && b > 0 && c > 0 || a < 0 && b < 0 && c < 0;
		}

		ST_API static int8_t sign(const real& num)
		{
			return num > 0 ? 1 : -1;
		}

		ST_API static bool isInRange(const real& value, const real& low, const real& high,
			const real& epsilon = Constant::GeometryEpsilon)
		{
			return value >= low - epsilon && value <= high + epsilon;
		}

		ST_API static bool fuzzyIsInRange(const real& value, const real& low, const real& high,
			const real& epsilon = Constant::GeometryEpsilon)
		{
			return value >= low - epsilon && value <= high + epsilon || value <= low + epsilon && low >= high - epsilon;
		}

		ST_API static real clamp(const real& num, const real& low, const real& high)
		{
			return std::clamp(num, low, high);
		}

		ST_API static size_t clamp(const size_t& num, const size_t& low, const size_t& high)
		{
			if (num < low)
				return low;
			if (num > high)
				return high;
			return num;
		}

		ST_API static real radians(const real& degree)
		{
			return degree * Constant::Pi * Constant::ReciprocalOf180;
		}

		ST_API static real degree(const real& radian)
		{
			return radian * 180.0f * Constant::ReciprocalOfPi;
		}

		ST_API static float fastInvSqrtDouble(double x, size_t maxIter = 4)
		{
			double y = x;
			double x2 = y * 0.5;
			std::int64_t i = *reinterpret_cast<std::int64_t*>(&y);
			i = 0x5fe6eb50c7b537a9 - (i >> 1);
			y = *reinterpret_cast<double*>(&i);
			for (size_t k = 0; k <= maxIter; k++)
				y = y * (1.5 - (x2 * y * y));
			return y;
		}
		
		ST_API static float fastInvSqrtFloat(float x, size_t maxIter = 4)
		{
			float y = x;
			float x2 = y * 0.5f;
			std::int32_t i = *reinterpret_cast<std::int32_t*>(&y);
			i = 0x5f3759df - (i >> 1);
			y = *reinterpret_cast<float*>(&i);
			for (size_t k = 0; k <= maxIter; k++)
				y = y * (1.5f - (x2 * y * y));
			return y;
		}
	};
}
