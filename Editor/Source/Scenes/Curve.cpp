#include "Curve.h"

namespace STEditor
{
	void RationalCubicBezier::set(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float w0,
		float w1, float w2, float w3)
	{
		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;
		m_weights[0] = w0;
		m_weights[1] = w1;
		m_weights[2] = w2;
		m_weights[3] = w3;
	}

	Vector2& RationalCubicBezier::pointAt(size_t index)
	{
		return m_points[index];
	}

	float& RationalCubicBezier::weightAt(size_t index)
	{
		return m_weights[index];
	}

	Vector2 RationalCubicBezier::sample(float t) const
	{
		float u0 = m_weights[0] * Math::bernstein(t, 0.0f, 3.0f);
		float u1 = m_weights[1] * Math::bernstein(t, 1.0f, 3.0f);
		float u2 = m_weights[2] * Math::bernstein(t, 2.0f, 3.0f);
		float u3 = m_weights[3] * Math::bernstein(t, 3.0f, 3.0f);

		Vector2 result = (m_points[0] * u0 + m_points[1] * u1 + m_points[2] * u2 + m_points[3] * u3) / (u0 + u1 + u2 + u3);

		return result;
	}

	std::vector<Vector2> RationalCubicBezier::samplePoints(size_t count) const
	{
		std::vector<Vector2> result;
		result.reserve(count);
		float step = 1.0f / static_cast<float>(count);

		for (float t = 0.0f; t <= 1.0f; t += step)
			result.push_back(sample(t));

		return result;
	}

	float RationalCubicBezier::curvatureAD(float t) const
	{


		return 0.0f;
	}

	float RationalCubicBezier::curvature(float t) const
	{
		// \left( (t-1)^3w_0-t\left( 3(t-1)^2w_1+t\left( tw_3-3(t-1)w_2 \right) \right) \right) ^2
		float det = std::pow((std::pow(t - 1.0f, 3.0f) * m_weights[0] -
			                     t * (3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2]))), 2.0f);

		// -3(t-1)^2w_0\left( (t-1)^2w_1+t\left( tw_3-2(t-1)w_2 \right) \right)

		float dp0 = -3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[0] *
			(std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 2.0f * (t - 1.0f) * m_weights[2]));

		// +3(t-1)w_1\left( t^2\left( 2tw_3-3(t-1)w_2 \right) +(t-1)^3w_0 \right)

		float dp1 = 3.0f * (t - 1.0f) * m_weights[1] *
			(t * t * (2.0f * t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2]) + std::pow(t - 1.0f, 3.0f) * m_weights[0]);

		// -3tw_2\left( t\left( t^2w_3-3(t-1)^2w_1 \right) +2(t-1)^3w_0 \right)

		float dp2 = -3.0f * t * m_weights[2] *
			(t * (t * t * m_weights[3] - 3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1]) + 2.0f * std::pow(t - 1.0f, 3.0f) * m_weights[0]);

		// +3t^2w_3\left( (t-1)^2w_0+t\left( tw_2-2(t-1)w_1 \right) \right)

		float dp3 = 3.0f * t * t * m_weights[3] *
			(std::pow(t - 1.0f, 2.0f) * m_weights[0] + t * (t * m_weights[2] - 2.0f * (t - 1.0f) * m_weights[1]));

		Vector2 dp = dp0 * m_points[0] + dp1 * m_points[1] + dp2 * m_points[2] + dp3 * m_points[3];
		dp /= det;

		// 6(t-1)w_0\left( (t-1)^3w_0\left( \left( -2t^2+t+1 \right) w_2+(t-1)^2w_1+t(t+1)w_3 \right) +t^2\left( t\left( 5t^2-13t+8 \right) w_3w_2-(t-2)t^2w_{3}^{2}-3(t-1)^2(2t-3)w_{2}^{2} \right) -3(t-1)^5w_{1}^{2}+t(t-1)^2w_1\left( 9(t-1)^2w_2+t(3-4t)w_3 \right) \right)

		float ddp0 = 6.0f * (t - 1.0f) * m_weights[0] *
			(std::pow(t - 1.0f, 3.0f) * m_weights[0] * ((-2.0f * t * t + t + 1.0f) * m_weights[2] + std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t + 1.0f) * m_weights[3]) + t * t * (t * (5.0f * t * t - 13.0f * t + 8.0f) * m_weights[3] * m_weights[2] - (t - 2.0f) * t * t * m_weights[3] * m_weights[3] - 3.0f * std::pow(t - 1.0f, 2.0f) * (2.0f * t - 3.0f) * m_weights[2] * m_weights[2]) - 3.0f * std::pow(t - 1.0f, 5.0f) * m_weights[1] * m_weights[1] + t * std::pow(t - 1.0f, 2.0f) * m_weights[1] * (9.0f * std::pow(t - 1.0f, 2.0f) * m_weights[2] + t * (3.0f - 4.0f * t) * m_weights[3]));

		// -6w_1\left( t^3\left( t^2(3-2t)w_{3}^{2}-9(t-1)^3w_{2}^{2}+9t(t-1)^2w_2w_3+3(t-1)^2w_1\left( 3(t-1)w_2-(2t+1)w_3 \right) \right) +(t-1)^6w_{0}^{2}+(t-1)^3w_0\left( t\left( t(t+6)w_3-9(t-1)w_2 \right) -3(t-1)^3w_1 \right) \right) 

		float ddp1 = -6.0f * m_weights[1] *
			(t * t * t * (t * t * (3.0f - 2.0f * t) * m_weights[3] * m_weights[3] - 9.0f * std::pow(t - 1.0f, 3.0f) * m_weights[2] * m_weights[2] + 9.0f * t * std::pow(t - 1.0f, 2.0f) * m_weights[2] * m_weights[3] + 3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] * (3.0f * (t - 1.0f) * m_weights[2] - (2.0f * t + 1.0f) * m_weights[3])) + std::pow(t - 1.0f, 6.0f) * m_weights[0] * m_weights[0] + std::pow(t - 1.0f, 3.0f) * m_weights[0] * (t * (t * (t + 6.0f) * m_weights[3] - 9.0f * (t - 1.0f) * m_weights[2]) - 3.0f * std::pow(t - 1.0f, 3.0f) * m_weights[1]));

		// 6w_2\left( t^3\left( t^3\left( 3w_2-w_3 \right) w_3+9(t-1)^3w_{1}^{2}-9(t-1)w_1\left( (t-1)^2w_2+tw_3 \right) \right) -t^2(t-1)^2w_0\left( -3\left( 2t^2-5t+3 \right) w_2+9(t-1)^2w_1+(t-7)tw_3 \right) +(2t+1)(t-1)^5w_{0}^{2} \right) 

		float ddp2 = 6.0f * m_weights[2] *
			(t * t * t * (t * t * t * (3.0f * m_weights[2] - m_weights[3]) * m_weights[3] + 9.0f * std::pow(t - 1.0f, 3.0f) * m_weights[1] * m_weights[1] - 9.0f * (t - 1.0f) * m_weights[1] * (std::pow(t - 1.0f, 2.0f) * m_weights[2] + t * m_weights[3])) - t * t * (t - 1.0f) * (t - 1.0f) * m_weights[0] * (-3.0f * (2.0f * t * t - 5.0f * t + 3.0f) * m_weights[2] + 9.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + (t - 7.0f) * t * m_weights[3]) + (2.0f * t + 1.0f) * std::pow(t - 1.0f, 5.0f) * m_weights[0] * m_weights[0]);

		// -6tw_3\left( t^5w_2\left( 3w_2-w_3 \right) +t^4w_1\left( (2t-3)w_3-9(t-1)w_2 \right) +3(t-1)^2(2t+1)t^2w_{1}^{2}-(t-1)tw_0\left( t\left( \left( -4t^2+5t-1 \right) w_2+(t-2)tw_3 \right) +(5t+3)(t-1)^2w_1 \right) +(t-1)^4(t+1)w_{0}^{2} \right) 

		float ddp3 = -6.0f * t * m_weights[3] *
			(std::pow(t, 5.0f) * m_weights[2] * (3.0f * m_weights[2] - m_weights[3]) + std::pow(t, 4.0f) * m_weights[1] * ((2.0f * t - 3.0f) * m_weights[3] - 9.0f * (t - 1.0f) * m_weights[2]) + 3.0f * std::pow(t - 1.0f, 2.0f) * (2.0f * t + 1.0f) * t * t * m_weights[1] * m_weights[1] - (t - 1.0f) * t * m_weights[0] * (t * ((-4.0f * t * t + 5.0f * t - 1.0f) * m_weights[2] + (t - 2.0f) * t * m_weights[3]) + (5.0f * t + 3.0f) * std::pow(t - 1.0f, 2.0f) * m_weights[1]) + std::pow(t - 1.0f, 4.0f) * (t + 1.0f) * m_weights[0] * m_weights[0]);

		Vector2 ddp = ddp0 * m_points[0] + ddp1 * m_points[1] + ddp2 * m_points[2] + ddp3 * m_points[3];

		ddp /= det;

		float k = std::abs(dp.x * ddp.y - dp.y * ddp.x) / std::pow(dp.length(), 3.0f);

		return k;
	}

	std::vector<Vector2> RationalCubicBezier::curvaturePoints(size_t count, float scale, bool flip) const
	{
		std::vector<Vector2> result;
		result.reserve(count);

		float step = 1.0f / static_cast<float>(count);
		for (float t = 0.0f; t <= 1.0f; t += step)
		{
			Vector2 p1 = sample(t);


			// \left( (t-1)^3w_0-t\left( 3(t-1)^2w_1+t\left( tw_3-3(t-1)w_2 \right) \right) \right) ^2
			float det = std::pow((std::pow(t - 1.0f, 3.0f) * m_weights[0] -
				                     t * (3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2]))), 2.0f);

			// -3(t-1)^2w_0\left( (t-1)^2w_1+t\left( tw_3-2(t-1)w_2 \right) \right)

			float dp0 = -3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[0] *
				(std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 2.0f * (t - 1.0f) * m_weights[2]));

			// +3(t-1)w_1\left( t^2\left( 2tw_3-3(t-1)w_2 \right) +(t-1)^3w_0 \right)

			float dp1 = 3.0f * (t - 1.0f) * m_weights[1] *
				(t * t * (2.0f * t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2]) + std::pow(t - 1.0f, 3.0f) * m_weights[0]);

			// -3tw_2\left( t\left( t^2w_3-3(t-1)^2w_1 \right) +2(t-1)^3w_0 \right)

			float dp2 = -3.0f * t * m_weights[2] *
				(t * (t * t * m_weights[3] - 3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1]) + 2.0f * std::pow(t - 1.0f, 3.0f) * m_weights[0]);

			// +3t^2w_3\left( (t-1)^2w_0+t\left( tw_2-2(t-1)w_1 \right) \right)

			float dp3 = 3.0f * t * t * m_weights[3] *
				(std::pow(t - 1.0f, 2.0f) * m_weights[0] + t * (t * m_weights[2] - 2.0f * (t - 1.0f) * m_weights[1]));

			Vector2 dp = dp0 * m_points[0] + dp1 * m_points[1] + dp2 * m_points[2] + dp3 * m_points[3];
			dp /= det;

			// 6(t-1)w_0\left( (t-1)^3w_0\left( \left( -2t^2+t+1 \right) w_2+(t-1)^2w_1+t(t+1)w_3 \right) +t^2\left( t\left( 5t^2-13t+8 \right) w_3w_2-(t-2)t^2w_{3}^{2}-3(t-1)^2(2t-3)w_{2}^{2} \right) -3(t-1)^5w_{1}^{2}+t(t-1)^2w_1\left( 9(t-1)^2w_2+t(3-4t)w_3 \right) \right)

			float ddp0 = 6.0f * (t - 1.0f) * m_weights[0] *
				(std::pow(t - 1.0f, 3.0f) * m_weights[0] * ((-2.0f * t * t + t + 1.0f) * m_weights[2] + std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t + 1.0f) * m_weights[3]) + t * t * (t * (5.0f * t * t - 13.0f * t + 8.0f) * m_weights[3] * m_weights[2] - (t - 2.0f) * t * t * m_weights[3] * m_weights[3] - 3.0f * std::pow(t - 1.0f, 2.0f) * (2.0f * t - 3.0f) * m_weights[2] * m_weights[2]) - 3.0f * std::pow(t - 1.0f, 5.0f) * m_weights[1] * m_weights[1] + t * std::pow(t - 1.0f, 2.0f) * m_weights[1] * (9.0f * std::pow(t - 1.0f, 2.0f) * m_weights[2] + t * (3.0f - 4.0f * t) * m_weights[3]));

			// -6w_1\left( t^3\left( t^2(3-2t)w_{3}^{2}-9(t-1)^3w_{2}^{2}+9t(t-1)^2w_2w_3+3(t-1)^2w_1\left( 3(t-1)w_2-(2t+1)w_3 \right) \right) +(t-1)^6w_{0}^{2}+(t-1)^3w_0\left( t\left( t(t+6)w_3-9(t-1)w_2 \right) -3(t-1)^3w_1 \right) \right) 

			float ddp1 = -6.0f * m_weights[1] *
				(t * t * t * (t * t * (3.0f - 2.0f * t) * m_weights[3] * m_weights[3] - 9.0f * std::pow(t - 1.0f, 3.0f) * m_weights[2] * m_weights[2] + 9.0f * t * std::pow(t - 1.0f, 2.0f) * m_weights[2] * m_weights[3] + 3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] * (3.0f * (t - 1.0f) * m_weights[2] - (2.0f * t + 1.0f) * m_weights[3])) + std::pow(t - 1.0f, 6.0f) * m_weights[0] * m_weights[0] + std::pow(t - 1.0f, 3.0f) * m_weights[0] * (t * (t * (t + 6.0f) * m_weights[3] - 9.0f * (t - 1.0f) * m_weights[2]) - 3.0f * std::pow(t - 1.0f, 3.0f) * m_weights[1]));

			// 6w_2\left( t^3\left( t^3\left( 3w_2-w_3 \right) w_3+9(t-1)^3w_{1}^{2}-9(t-1)w_1\left( (t-1)^2w_2+tw_3 \right) \right) -t^2(t-1)^2w_0\left( -3\left( 2t^2-5t+3 \right) w_2+9(t-1)^2w_1+(t-7)tw_3 \right) +(2t+1)(t-1)^5w_{0}^{2} \right) 

			float ddp2 = 6.0f * m_weights[2] *
				(t * t * t * (t * t * t * (3.0f * m_weights[2] - m_weights[3]) * m_weights[3] + 9.0f * std::pow(t - 1.0f, 3.0f) * m_weights[1] * m_weights[1] - 9.0f * (t - 1.0f) * m_weights[1] * (std::pow(t - 1.0f, 2.0f) * m_weights[2] + t * m_weights[3])) - t * t * (t - 1.0f) * (t - 1.0f) * m_weights[0] * (-3.0f * (2.0f * t * t - 5.0f * t + 3.0f) * m_weights[2] + 9.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + (t - 7.0f) * t * m_weights[3]) + (2.0f * t + 1.0f) * std::pow(t - 1.0f, 5.0f) * m_weights[0] * m_weights[0]);

			// -6tw_3\left( t^5w_2\left( 3w_2-w_3 \right) +t^4w_1\left( (2t-3)w_3-9(t-1)w_2 \right) +3(t-1)^2(2t+1)t^2w_{1}^{2}-(t-1)tw_0\left( t\left( \left( -4t^2+5t-1 \right) w_2+(t-2)tw_3 \right) +(5t+3)(t-1)^2w_1 \right) +(t-1)^4(t+1)w_{0}^{2} \right) 

			float ddp3 = -6.0f * t * m_weights[3] *
				(std::pow(t, 5.0f) * m_weights[2] * (3.0f * m_weights[2] - m_weights[3]) + std::pow(t, 4.0f) * m_weights[1] * ((2.0f * t - 3.0f) * m_weights[3] - 9.0f * (t - 1.0f) * m_weights[2]) + 3.0f * std::pow(t - 1.0f, 2.0f) * (2.0f * t + 1.0f) * t * t * m_weights[1] * m_weights[1] - (t - 1.0f) * t * m_weights[0] * (t * ((-4.0f * t * t + 5.0f * t - 1.0f) * m_weights[2] + (t - 2.0f) * t * m_weights[3]) + (5.0f * t + 3.0f) * std::pow(t - 1.0f, 2.0f) * m_weights[1]) + std::pow(t - 1.0f, 4.0f) * (t + 1.0f) * m_weights[0] * m_weights[0]);

			Vector2 ddp = ddp0 * m_points[0] + ddp1 * m_points[1] + ddp2 * m_points[2] + ddp3 * m_points[3];

			ddp /= det;

			float k = std::abs(dp.x * ddp.y - dp.y * ddp.x) / std::pow(dp.length(), 3.0f);

			Vector2 tangent = dp.normal();
			Vector2 normal = tangent.perpendicular();
			Vector2 v = normal * k * scale;

			if (flip)
				v.negate();

			Vector2 curvaturePoint = v + p1;
			result.push_back(curvaturePoint);

		}

		return result;
	}

	void RationalQuadraticBezier::set(const Vector2& p0, const Vector2& p1, const Vector2& p2, float w0, float w1,
		float w2)
	{
		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_weights[0] = w0;
		m_weights[1] = w1;
		m_weights[2] = w2;
	}

	Vector2& RationalQuadraticBezier::pointAt(size_t index)
	{
		return m_points[index];
	}

	float& RationalQuadraticBezier::weightAt(size_t index)
	{
		return m_weights[index];
	}

	Vector2 RationalQuadraticBezier::sample(float t) const
	{
		float u0 = m_weights[0] * Math::bernstein(t, 0.0f, 3.0f);
		float u1 = m_weights[1] * Math::bernstein(t, 1.0f, 3.0f);
		float u2 = m_weights[2] * Math::bernstein(t, 2.0f, 3.0f);

		Vector2 result = (m_points[0] * u0 + m_points[1] * u1 + m_points[2] * u2) / (u0 + u1 + u2);

		return result;
	}

	CubicBezier CubicBezier::fromControlPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2,
		const Vector2& p3)
	{
		CubicBezier bezier;
		bezier.setPoints(p0, p1, p2, p3);
		return bezier;
	}

	CubicBezier CubicBezier::fromHermite(const Vector2& p0, const Vector2& dir0, const Vector2& p1, const Vector2& dir1)
	{
		CubicBezier bezier;
		bezier.setPoints(p0, p0 + dir0 / 3.0f, p1 - dir1 / 3.0f, p1);
		return bezier;
	}

	float CubicBezier::torsion(float t) const
	{
		Vector2 dp = -3.0f * (1.0f - t) * (1.0f - t) * m_points[0] +
			(9.0f * t * t - 12.0f * t + 3.0f) * m_points[1] +
			(6.0f * t - 9.0f * t * t) * m_points[2] +
			3.0f * t * t * m_points[3];

		Vector2 ddp = 6.0f * (1.0f - t) * m_points[0] +
			(18.0f * t - 12.0f) * m_points[1] +
			(6.0f - 18.0f * t) * m_points[2] +
			6.0f * t * m_points[3];

		Vector2 dddp = -6.0f * m_points[0] +
			18.0f * m_points[1] -
			18.0f * m_points[2] +
			6.0f * m_points[3];

		return 0;
	}

	Vector2 CubicBezier::sample(float t) const
	{
		return Math::bernstein(t, 0, 3) * m_points[0] +
			Math::bernstein(t, 1, 3) * m_points[1] +
			Math::bernstein(t, 2, 3) * m_points[2] +
			Math::bernstein(t, 3, 3) * m_points[3];
	}

	float CubicBezier::curvature(float t) const
	{
		//Vector2 dp = Math::dBernstein(t, 0, 3) * m_points[0] +
		//	Math::dBernstein(t, 1, 3) * m_points[1] +
		//	Math::dBernstein(t, 2, 3) * m_points[2] +
		//	Math::dBernstein(t, 3, 3) * m_points[3];

		Vector2 dp = -3.0f * (1.0f - t) * (1.0f - t) * m_points[0] +
			(9.0f * t * t - 12.0f * t + 3.0f) * m_points[1] +
			(6.0f * t - 9.0f * t * t) * m_points[2] +
			3.0f * t * t * m_points[3];

		Vector2 ddp = 6.0f * (1.0f - t) * m_points[0] +
			(18.0f * t - 12.0f) * m_points[1] +
			(6.0f - 18.0f * t) * m_points[2] +
			6.0f * t * m_points[3];

		float k = std::abs(dp.x * ddp.y - dp.y * ddp.x) / std::pow(dp.length(), 3.0f);

		return k;
	}

	std::vector<Vector2> CubicBezier::curvaturePoints(size_t count, float scale, bool flip) const
	{
		std::vector<Vector2> result;
		result.reserve(count);

		float step = 1.0f / static_cast<float>(count);
		for (float t = 0.0f; t <= 1.0f; t += step)
		{

			Vector2 p1 = sample(t);

			//Vector2 dp = -3.0f * (1.0f - t) * (1.0f - t) * m_points[0] +
			//	(9.0f * t * t - 12.0f * t + 3.0f) * m_points[1] +
			//	(6.0f * t - 9.0f * t * t) * m_points[2] +
			//	3.0f * t * t * m_points[3];

			Vector2 dp = Math::dBernstein(t, 0, 3) * m_points[0] +
				Math::dBernstein(t, 1, 3) * m_points[1] +
				Math::dBernstein(t, 2, 3) * m_points[2] +
				Math::dBernstein(t, 3, 3) * m_points[3];

			Vector2 ddp = Math::d2Bernstein(t, 0, 3) * m_points[0] +
				Math::d2Bernstein(t, 1, 3) * m_points[1] +
				Math::d2Bernstein(t, 2, 3) * m_points[2] +
				Math::d2Bernstein(t, 3, 3) * m_points[3];

			//Vector2 ddp = 6.0f * (1.0f - t) * m_points[0] +
			//	(18.0f * t - 12.0f) * m_points[1] +
			//	(6.0f - 18.0f * t) * m_points[2] +
			//	6.0f * t * m_points[3];

			float k = std::abs(dp.x * ddp.y - dp.y * ddp.x) / std::pow(dp.length(), 3.0f);

			Vector2 tangent = dp.normal();
			Vector2 normal = tangent.perpendicular();
			Vector2 v = normal * k * scale;

			if (flip)
				v.negate();

			Vector2 curvaturePoint = v + p1;
			result.push_back(curvaturePoint);
		}
		return result;

	}

	Vector2& CubicBezier::operator[](size_t index)
	{
		return m_points[index];
	}

	Vector2& CubicBezier::at(size_t index)
	{
		return m_points[index];
	}

	void CubicBezier::setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;
	}

	std::vector<Vector2> CubicBezier::samplePoints(size_t count)
	{
		std::vector<Vector2> result;
		result.reserve(count);
		float step = 1.0f / static_cast<float>(count);

		for (float t = 0.0f; t <= 1.0f; t += step)
			result.push_back(sample(t));

		return result;
	}
}
