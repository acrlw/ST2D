#pragma once

#include <format>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace ST
{
	using Index = uint32_t;
	using real = float;

	union ST_API ObjectPair
	{
		struct
		{
			int32_t objectIdA;
			int32_t objectIdB;
		};
		int64_t key = 0;

		ObjectPair() : key(0) {}
		ObjectPair(int32_t a, int32_t b)
		{
			objectIdA = std::min(a, b);
			objectIdB = std::max(a, b);
		}

		bool operator==(const ObjectPair& other) const
		{
			return key == other.key;
		}

	};

	struct ObjectPairHash
	{
		std::size_t operator()(const ObjectPair& p) const
		{
			return static_cast<std::size_t>(p.key);
		}
	};

	namespace Constant
	{
		constexpr unsigned int SimplexMax = 8;
		constexpr unsigned int CCDMaxIterations = 20;
		constexpr real Epsilon = FLT_EPSILON;
		constexpr real Max = FLT_MAX;
		constexpr real PositiveMin = FLT_MIN;
		constexpr real NegativeMin = -Max;
		constexpr real Pi = 3.1415926535f;
		constexpr real HalfPi = Pi / 2.0f;
		constexpr real DoublePi = Pi * 2.0f;
		constexpr real ReciprocalOf180 = 1.0f / 180.0f;
		constexpr real ReciprocalOfPi = 0.3183098861f;
		constexpr real GeometryEpsilon = 1e-5f;
		constexpr real TrignometryEpsilon = 1e-3f;
		constexpr real CCDMinVelocity = 100.0f;
		constexpr real MaxVelocity = 1000.0f;
		constexpr real MaxAngularVelocity = 1000.0f;
		constexpr real AABBExpansionFactor = 0.0f;
		constexpr real MinLinearVelocity = 1e-4f;
		constexpr real MinAngularVelocity = 1e-4f;
		constexpr real MinEnergy = 9e-10f;
		constexpr size_t SleepCountdown = 32;
		constexpr int GJKRetryTimes = 8;
	}
}
