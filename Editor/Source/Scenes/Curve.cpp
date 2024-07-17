#include "Curve.h"


namespace STEditor
{
	var CubicBezierAD::bezier3Func(var t, const Params& param)
	{
		return (1.0 - t) * (1.0 - t) * (1.0 - t) * param.p0 +
			3.0 * t * (1.0 - t) * (1.0 - t) * param.p1 +
			3.0 * t * t * (1.0 - t) * param.p2 +
			t * t * t * param.p3;
	}
	CubicBezierAD::CubicBezierAD(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;
	   
		m_t = 0.0;

		Params param1, param2;
		param1.p0 = m_points[0].x;
		param1.p1 = m_points[1].x;
		param1.p2 = m_points[2].x;
		param1.p3 = m_points[3].x;

		param2.p0 = m_points[0].y;
		param2.p1 = m_points[1].y;
		param2.p2 = m_points[2].y;
		param2.p3 = m_points[3].y;


		m_px = bezier3Func(m_t, param1);
		m_py = bezier3Func(m_t, param2);
	}

	void CubicBezierAD::setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		if (p0.equal(m_points[0]) && p1.equal(m_points[1]) && p2.equal(m_points[2]) && p3.equal(m_points[3]))
			return;

		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;

		m_needUpdateCurvePoints = true;
		m_needUpdateCurvaturePoints = true;
	}


	Vector2 CubicBezierAD::sample(float t)
	{
		Vector2 result;
		m_t = t;
		Params param1, param2;
		param1.p0 = m_points[0].x;
		param1.p1 = m_points[1].x;
		param1.p2 = m_points[2].x;
		param1.p3 = m_points[3].x;

		param2.p0 = m_points[0].y;
		param2.p1 = m_points[1].y;
		param2.p2 = m_points[2].y;
		param2.p3 = m_points[3].y;


		m_px = bezier3Func(m_t, param1);
		m_py = bezier3Func(m_t, param2);

		double x = m_px.expr->val;
		double y = m_py.expr->val;

		return Vector2(x, y);
	}

	std::array<Vector2, 4> CubicBezierAD::points() const
	{
		return m_points;
	}

	Vector2 CubicBezierAD::pointAt(size_t index) const
	{
		return m_points[index];
	}

	Vector2 CubicBezierAD::operator[](size_t index) const
	{
		return m_points[index];
	}

	float CubicBezierAD::curvatureAt(float t)
	{
		m_t.update(t);

		if (m_needUpdateCurvePoints)
		{
			Params param1, param2;
			param1.p0 = m_points[0].x;
			param1.p1 = m_points[1].x;
			param1.p2 = m_points[2].x;
			param1.p3 = m_points[3].x;

			param2.p0 = m_points[0].y;
			param2.p1 = m_points[1].y;
			param2.p2 = m_points[2].y;
			param2.p3 = m_points[3].y;

			m_px = bezier3Func(m_t, param1);
			m_py = bezier3Func(m_t, param2);
		}
		else
		{
			m_px.update();
			m_py.update();
		}

		auto [Vdxdt] = reverse::detail::derivativesx(m_px, wrt(m_t));
		auto [Vdydt] = reverse::detail::derivativesx(m_py, wrt(m_t));

		auto [Vd2xdt2] = reverse::detail::derivativesx(Vdxdt, wrt(m_t));
		auto [Vd2ydt2] = reverse::detail::derivativesx(Vdydt, wrt(m_t));

		double dxdt = Vdxdt.expr->val;
		double dydt = Vdydt.expr->val;

		double d2xdt2 = Vd2xdt2.expr->val;
		double d2ydt2 = Vd2ydt2.expr->val;

		double k = std::abs(dxdt * d2ydt2 - dydt * d2xdt2) / std::pow(dxdt * dxdt + dydt * dydt, 3.0);

		if (std::isnan(k))
			__debugbreak();

		return k;

		{
			//APP_INFO("----------")
			//Params param1, param2;
			//param1.p0 = m_points[0].x;
			//param1.p1 = m_points[1].x;
			//param1.p2 = m_points[2].x;
			//param1.p3 = m_points[3].x;

			//param2.p0 = m_points[0].y;
			//param2.p1 = m_points[1].y;
			//param2.p2 = m_points[2].y;
			//param2.p3 = m_points[3].y;

			//var t = 0.1;
			//var ux = f(t, param1);
			//var uy = f(t, param2);

			//auto [Vdxdt] = reverse::detail::derivativesx(ux, wrt(t));
			//auto [Vdydt] = reverse::detail::derivativesx(uy, wrt(t));

			//auto [Vd2xdt2] = reverse::detail::derivativesx(Vdxdt, wrt(t));
			//auto [Vd2ydt2] = reverse::detail::derivativesx(Vdydt, wrt(t));

			//double dxdt = Vdxdt.expr->val;
			//double dydt = Vdydt.expr->val;

			//double d2xdt2 = Vd2xdt2.expr->val;
			//double d2ydt2 = Vd2ydt2.expr->val;



			//APP_INFO("AD Curvature at t:{} is {}", t.expr->val, k);

			//Vector2 dp = Math::dBernstein(t.expr->val, 0, 3) * m_points[0] +
			//	Math::dBernstein(t.expr->val, 1, 3) * m_points[1] +
			//	Math::dBernstein(t.expr->val, 2, 3) * m_points[2] +
			//	Math::dBernstein(t.expr->val, 3, 3) * m_points[3];

			//Vector2 dp2 = -3.0f * (1.0f - t.expr->val) * (1.0f - t.expr->val) * m_points[0] +
			//	(9.0f * t.expr->val * t.expr->val - 12.0f * t.expr->val + 3.0f) * m_points[1] +
			//	(6.0f * t.expr->val - 9.0f * t.expr->val * t.expr->val) * m_points[2] +
			//	3.0f * t.expr->val * t.expr->val * m_points[3];

			//APP_INFO("AD: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dxdt, dydt);
			//APP_INFO("Bernstein: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dp.x, dp.y);
			//APP_INFO("Manual: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dp2.x, dp2.y);

			//t.update(0.5);
			//ux.update();
			//uy.update();

			//auto [V2dxdt] = reverse::detail::derivativesx(ux, wrt(t));
			//auto [V2dydt] = reverse::detail::derivativesx(uy, wrt(t));

			//auto [V2d2xdt2] = reverse::detail::derivativesx(Vdxdt, wrt(t));
			//auto [V2d2ydt2] = reverse::detail::derivativesx(Vdydt, wrt(t));

			//dxdt = V2dxdt.expr->val;
			//dydt = V2dydt.expr->val;

			//d2xdt2 = V2d2xdt2.expr->val;
			//d2ydt2 = V2d2ydt2.expr->val;


			//k = std::abs(dxdt * d2ydt2 - dydt * d2xdt2) / std::pow(dxdt * dxdt + dydt * dydt, 3.0);
			//APP_INFO("---")
			//APP_INFO("AD Curvature at t:{} is {}", t.expr->val, k);

			//dp = Math::dBernstein(t.expr->val, 0, 3) * m_points[0] +
			//	Math::dBernstein(t.expr->val, 1, 3) * m_points[1] +
			//	Math::dBernstein(t.expr->val, 2, 3) * m_points[2] +
			//	Math::dBernstein(t.expr->val, 3, 3) * m_points[3];

			//dp2 = -3.0f * (1.0f - t.expr->val) * (1.0f - t.expr->val) * m_points[0] +
			//	(9.0f * t.expr->val * t.expr->val - 12.0f * t.expr->val + 3.0f) * m_points[1] +
			//	(6.0f * t.expr->val - 9.0f * t.expr->val * t.expr->val) * m_points[2] +
			//	3.0f * t.expr->val * t.expr->val * m_points[3];

			//APP_INFO("AD: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dxdt, dydt);
			//APP_INFO("Bernstein: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dp.x, dp.y);
			//APP_INFO("Manual: t:{}, dx/dt:{}, dy/dt:{}", t.expr->val, dp2.x, dp2.y);
			//APP_INFO("----------")
		}
	}


	void CubicBezierAD::sampleCurvePoints()
	{
		m_curvePoints.clear();
		m_curvePoints.reserve(m_count);
		float step = 1.0f / (m_count - 1);
		for (size_t i = 0; i < m_count; i++)
		{
			m_curvePoints.push_back(sample(i * step));
		}
	}

	void CubicBezierAD::sampleCurvaturePoints()
	{
		m_curvaturePoints.clear();
		m_curvaturePoints.reserve(m_count);

		if (m_needUpdateCurvePoints)
		{
			Params param1, param2;
			param1.p0 = m_points[0].x;
			param1.p1 = m_points[1].x;
			param1.p2 = m_points[2].x;
			param1.p3 = m_points[3].x;

			param2.p0 = m_points[0].y;
			param2.p1 = m_points[1].y;
			param2.p2 = m_points[2].y;
			param2.p3 = m_points[3].y;

			m_px = bezier3Func(m_t, param1);
			m_py = bezier3Func(m_t, param2);
		}

		float step = 1.0f / (m_count - 1);
		for (size_t i = 0; i < m_count; i++)
		{
			float t = i * step;

			Vector2 p = sample(t);
			m_t.update(t);
			m_px.update();
			m_py.update();

			auto [Vdxdt] = reverse::detail::derivativesx(m_px, wrt(m_t));
			auto [Vdydt] = reverse::detail::derivativesx(m_py, wrt(m_t));

			auto [Vd2xdt2] = reverse::detail::derivativesx(Vdxdt, wrt(m_t));
			auto [Vd2ydt2] = reverse::detail::derivativesx(Vdydt, wrt(m_t));

			double dxdt = Vdxdt.expr->val;
			double dydt = Vdydt.expr->val;

			double d2xdt2 = Vd2xdt2.expr->val;
			double d2ydt2 = Vd2ydt2.expr->val;

			float k = std::abs(dxdt * d2ydt2 - dydt * d2xdt2) / std::pow(dxdt * dxdt + dydt * dydt, 1.5);


			Vector2 dir(dxdt, dydt);
			Vector2 normal = dir.perpendicular().normal();
			m_curvaturePoints.push_back(p + normal * k);
		}
	}

	Vector2 CubicBezierAD::tangent(float t)
	{
		return {};
	}

	void RationalCubicBezier::setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		if(p0.equal(m_points[0]) && 
			p1.equal(m_points[1]) && 
			p2.equal(m_points[2]) && 
			p3.equal(m_points[3]))
			return;

		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;
		
		m_needUpdateCurvaturePoints = true;
		m_needUpdateCurvePoints = true;
	}

	void RationalCubicBezier::setWeights(float w0, float w1, float w2, float w3)
	{
		if( realEqual(w0, m_weights[0]) && realEqual(w1, m_weights[1]) &&
			realEqual(w2, m_weights[2]) && realEqual(w3, m_weights[3]))
			return;

		m_weights[0] = w0;
		m_weights[1] = w1;
		m_weights[2] = w2;
		m_weights[3] = w3;

		m_needUpdateCurvaturePoints = true;
		m_needUpdateCurvePoints = true;
	}

	std::array<Vector2, 4> RationalCubicBezier::points() const
	{
		return m_points;
	}

	std::array<float, 4> RationalCubicBezier::weights() const
	{
		return m_weights;
	}

	Vector2 RationalCubicBezier::pointAt(size_t index) const
	{
		return m_points[index];
	}

	float RationalCubicBezier::weightAt(size_t index) const
	{
		return m_weights[index];
	}

	Vector2 RationalCubicBezier::sample(float t)
	{
		float u0 = m_weights[0] * Math::bernstein(t, 0.0f, 3.0f);
		float u1 = m_weights[1] * Math::bernstein(t, 1.0f, 3.0f);
		float u2 = m_weights[2] * Math::bernstein(t, 2.0f, 3.0f);
		float u3 = m_weights[3] * Math::bernstein(t, 3.0f, 3.0f);

		Vector2 result = (m_points[0] * u0 + m_points[1] * u1 + m_points[2] * u2 + m_points[3] * u3) / (u0 + u1 + u2 + u3);

		return result;
	}


	float RationalCubicBezier::curvatureAt(float t)
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

	void RationalCubicBezier::sampleCurvePoints()
	{
		m_curvePoints.clear();
		float step = 1.0f / static_cast<float>(m_count);

		for (float t = 0.0f; t <= 1.0f; t += step)
			m_curvePoints.push_back(sample(t));
	}

	void RationalCubicBezier::sampleCurvaturePoints()
	{
		m_curvaturePoints.clear();

		float step = 1.0f / static_cast<float>(m_count);
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
			Vector2 v = normal * k;

			Vector2 curvaturePoint = v + p1;
			m_curvaturePoints.push_back(curvaturePoint);

		}
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

	Vector2 CubicBezier::sample(float t)
	{
		return Math::bernstein(t, 0, 3) * m_points[0] +
			Math::bernstein(t, 1, 3) * m_points[1] +
			Math::bernstein(t, 2, 3) * m_points[2] +
			Math::bernstein(t, 3, 3) * m_points[3];
	}

	float CubicBezier::curvatureAt(float t)
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

	std::array<Vector2, 4> CubicBezier::points() const
	{
		return m_points;
	}

	Vector2 CubicBezier::pointAt(size_t index) const
	{
		return m_points[index];
	}

	Vector2 CubicBezier::operator[](size_t index) const
	{
		return m_points[index];
	}

	void CubicBezier::setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		if (p0.equal(m_points[0]) && p1.equal(m_points[1]) && p2.equal(m_points[2]) && p3.equal(m_points[3]))
			return;

		m_points[0] = p0;
		m_points[1] = p1;
		m_points[2] = p2;
		m_points[3] = p3;

		m_needUpdateCurvaturePoints = true;
		m_needUpdateCurvePoints = true;
	}


	void CubicBezier::sampleCurvePoints()
	{
		m_curvePoints.clear();
		float step = 1.0f / static_cast<float>(m_count);

		for (float t = 0.0f; t <= 1.0f; t += step)
			m_curvePoints.push_back(sample(t));
	}

	void CubicBezier::sampleCurvaturePoints()
	{
		m_curvaturePoints.clear();

		float step = 1.0f / static_cast<float>(m_count);
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
			Vector2 v = normal;

			Vector2 curvaturePoint = v * k + p1 ;
			m_curvaturePoints.push_back(curvaturePoint);
		}
	}
}
