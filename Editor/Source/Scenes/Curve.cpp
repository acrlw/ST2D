#include "Curve.h"
#include <gsl/gsl_poly.h>

namespace STEditor
{
	void SpiralShapeBase::setParam(real Cf, real arccosx)
	{
		if(m_Cf == Cf && m_arccosx == arccosx)
			return;

		m_Cf = Cf;
		m_arccosx = arccosx;
		m_needUpdate = true;
	}


	void SpiralShapeBase::computePoints(std::vector<Vector2>& curvePoints, std::vector<Vector2>& curvaturePoints, real L,
		size_t count)
	{
		real ds = L / static_cast<real>(count);
		real lastC = 0.0f;
		real lastS = 0.0f;
		real lastCIntegral = 0.0f;
		real lastSIntegral = 0.0f;
		for (size_t i = 0; i < count; ++i)
		{
			real s = static_cast<real>(i) * ds;
			real integral = computeCurvatureIntegral(s, L);

			real C = std::cos(integral);
			real S = std::sin(integral);

			if (i == 0)
			{
				lastC = C;
				lastS = S;
				curvePoints.emplace_back(0, 0);
				curvaturePoints.emplace_back(0, 0);

				continue;
			}
			real mid = s - 0.5f * ds;
			integral = computeCurvatureIntegral(mid, L);
			real midC = std::cos(integral);
			real midS = std::sin(integral);

			real CIntegral = 1.0f / 6.0f * ds * (lastC + 4.0f * midC + C) + lastCIntegral;
			real SIntegral = 1.0f / 6.0f * ds * (lastS + 4.0f * midS + S) + lastSIntegral;


			Vector2 p(CIntegral, SIntegral);
			curvePoints.emplace_back(p);

			Vector2 normal = Vector2(-S, C);
			real curvature = computeCurvature(s, L);

			Vector2 curvaturePoint = p - curvature * normal;

			curvaturePoints.emplace_back(curvaturePoint);

			lastCIntegral = CIntegral;
			lastSIntegral = SIntegral;

			lastC = C;
			lastS = S;
		}
	}

	bool SpiralShapeBase::needUpdate() const
	{
		return m_needUpdate;
	}

	void SpiralShapeBase::setNeedUpdate(bool needUpdate)
	{
		m_needUpdate = needUpdate;
	}

	void TwoWeightCubicBezierSpiral::setWeights(real w0, real w1)
	{
		if(m_w0 == w0 && m_w1 == w1)
			return;

		m_w0 = w0;
		m_w1 = w1;
		m_needUpdate = true;
	}

	real TwoWeightCubicBezierSpiral::weight0() const
	{
		return m_w0;
	}

	real TwoWeightCubicBezierSpiral::weight1() const
	{
		return m_w1;
	}

	real TwoWeightCubicBezierSpiral::computeArcLength()
	{
		return 10.0f * m_arccosx / (m_Cf * (5.0f + 3.0f * (m_w1 - m_w0)));
	}

	real TwoWeightCubicBezierSpiral::computeCurvature(real s, real L)
	{
		real x_1 = m_w0 * L;
		real x_2 = (1.0 - m_w1) * L;
		real x_3 = L;

		real y_2 = m_Cf;
		real y_3 = m_Cf;

		real A = 3.0 * x_1 - 3.0 * x_2 + x_3;
		real B = -6.0 * x_1 + 3.0 * x_2;
		real C = 3.0 * x_1;
		real D = -s;
		double coeffs[] = { D, C, B, A };

		//Ax^3 + Bx^2 + Cx + D = 0, find root

		gsl_complex roots[3];

		gsl_poly_complex_workspace* w = gsl_poly_complex_workspace_alloc(4);
		gsl_poly_complex_solve(coeffs, 4, w, &roots[0].dat[0]);
		gsl_poly_complex_workspace_free(w);
		real t = 0.0f;
		for (int i = 0; i < 3; i++) {
			double realPart = GSL_REAL(roots[i]);
			double imagPart = GSL_IMAG(roots[i]);
			if (imagPart == 0.0)
				t = realPart;
		}

		// \left( 1-t \right) ^3y_0+3t\left( 1-t \right) ^2y_1+3t^2\left( 1-t \right) y_2+t^3y_3
		real y = 3.0f * t * t * (1.0f - t) * y_2 + t * t * t * y_3;

		return y;
	}

	real TwoWeightCubicBezierSpiral::computeCurvatureIntegral(real s, real L)
	{
		real x_1 = m_w0 * L;
		real x_2 = (1.0 - m_w1) * L;
		real x_3 = L;

		real A = 3.0 * x_1 - 3.0 * x_2 + x_3;
		real B = -6.0 * x_1 + 3.0 * x_2;
		real C = 3.0 * x_1;
		real D = -s;
		double coeffs[] = { D, C, B, A };

		//Ax^3 + Bx^2 + Cx + D = 0, find root

		gsl_complex roots[3];

		gsl_poly_complex_workspace* w = gsl_poly_complex_workspace_alloc(4);
		gsl_poly_complex_solve(coeffs, 4, w, &roots[0].dat[0]);
		gsl_poly_complex_workspace_free(w);
		real t = 0.0f;
		for (int i = 0; i < 3; i++) {
			double realPart = GSL_REAL(roots[i]);
			double imagPart = GSL_IMAG(roots[i]);
			if (imagPart == 0.0)
				t = realPart;
		}
		// -\frac{1}{10}LT^6\left( 30w_1+30w_2-20 \right) C_f-\frac{1}{10}LT^5\left( -102w_1-78w_2+60 \right) C_f-\frac{1}{10}LT^4\left( 105w_1+45w_2-45 \right) C_f+3LT^3w_1C_f

		real integral = -1.0 / 10.0 * L * std::pow(t, 6) * (30.0 * m_w0 + 30.0 * m_w1 - 20.0) * m_Cf -
			1.0 / 10.0 * L * std::pow(t, 5) * (-102.0 * m_w0 - 78.0 * m_w1 + 60.0) * m_Cf -
			1.0 / 10.0 * L * std::pow(t, 4) * (105.0 * m_w0 + 45.0 * m_w1 - 45.0) * m_Cf +
			3.0 * L * std::pow(t, 3) * m_w0 * m_Cf;

		return integral;
	}

	void OneWeightCubicBezierSpiral::setWeight(real w)
	{
		if (m_w == w)
			return;

		m_w = w;
		m_needUpdate = true;
	}

	real OneWeightCubicBezierSpiral::weight() const
	{
		return m_w;
	}

	real OneWeightCubicBezierSpiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real OneWeightCubicBezierSpiral::computeCurvature(real s, real L)
	{
		real x_1 = m_w * L;
		real x_2 = (1.0 - m_w) * L;
		real x_3 = L;

		real y_2 = m_Cf;
		real y_3 = m_Cf;

		real A = 3.0 * x_1 - 3.0 * x_2 + x_3;
		real B = -6.0 * x_1 + 3.0 * x_2;
		real C = 3.0 * x_1;
		real D = -s;
		double coeffs[] = { D, C, B, A };

		//Ax^3 + Bx^2 + Cx + D = 0, find root

		gsl_complex roots[3];

		gsl_poly_complex_workspace* w = gsl_poly_complex_workspace_alloc(4);
		gsl_poly_complex_solve(coeffs, 4, w, &roots[0].dat[0]);
		gsl_poly_complex_workspace_free(w);
		real t = 0.0f;
		for (int i = 0; i < 3; i++) {
			double realPart = GSL_REAL(roots[i]);
			double imagPart = GSL_IMAG(roots[i]);
			if (imagPart == 0.0)
				t = realPart;
		}

		// \left( 1-t \right) ^3y_0+3t\left( 1-t \right) ^2y_1+3t^2\left( 1-t \right) y_2+t^3y_3
		real y = 3.0f * t * t * (1.0f - t) * y_2 + t * t * t * y_3;
		return y;
	}

	real OneWeightCubicBezierSpiral::computeCurvatureIntegral(real s, real L)
	{
		real x_1 = m_w * L;
		real x_2 = (1.0 - m_w) * L;
		real x_3 = L;

		real A = 3.0 * x_1 - 3.0 * x_2 + x_3;
		real B = -6.0 * x_1 + 3.0 * x_2;
		real C = 3.0 * x_1;
		real D = -s;
		double coeffs[] = { D, C, B, A };

		//Ax^3 + Bx^2 + Cx + D = 0, find root

		gsl_complex roots[3];

		gsl_poly_complex_workspace* work = gsl_poly_complex_workspace_alloc(4);
		gsl_poly_complex_solve(coeffs, 4, work, &roots[0].dat[0]);
		gsl_poly_complex_workspace_free(work);
		real t = 0.0f;
		for (int i = 0; i < 3; i++) {
			double realPart = GSL_REAL(roots[i]);
			double imagPart = GSL_IMAG(roots[i]);
			if (imagPart == 0.0)
				t = realPart;
		}

		//-2L(3w-1)C_fT^6+6L(3w-1)C_fT^5-\frac{1}{2}L(30w-9)C_fT^4+3LwC_fT^3

		real integral = -2.0 * L * (3.0 * m_w - 1.0) * m_Cf * std::pow(t, 6) +
			6.0 * L * (3.0 * m_w - 1.0) * m_Cf * std::pow(t, 5) -
			0.5 * L * (30.0 * m_w - 9.0) * m_Cf * std::pow(t, 4) +
			3.0 * L * m_w * m_Cf * std::pow(t, 3);

		return integral;
	}

	real G2Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G2Spiral::computeCurvature(real s, real L)
	{
		return m_Cf / L * s;
	}

	real G2Spiral::computeCurvatureIntegral(real s, real L)
	{
		return 0.5f * m_Cf / L * s * s;
	}

	real G3Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G3Spiral::computeCurvature(real s, real L)
	{
		return -2.0f * m_Cf / std::pow(L, 3) * std::pow(s, 3) +
			3.0f * m_Cf / std::pow(L, 2) * std::pow(s, 2);
	}

	real G3Spiral::computeCurvatureIntegral(real s, real L)
	{
		return -0.5 * m_Cf / std::pow(L, 3) * std::pow(s, 4) +
			m_Cf / std::pow(L, 2) * std::pow(s, 3);
	}

	real G4Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G4Spiral::computeCurvature(real s, real L)
	{
		return 6.0f * m_Cf / std::pow(L, 5) * std::pow(s, 5) -
			15.0f * m_Cf / std::pow(L, 4) * std::pow(s, 4) +
			10.0f * m_Cf / std::pow(L, 3) * std::pow(s, 3);
	}

	real G4Spiral::computeCurvatureIntegral(real s, real L)
	{
		return m_Cf / std::pow(L, 5) * std::pow(s, 6) -
			3.0f * m_Cf / std::pow(L, 4) * std::pow(s, 5) +
			5.0f * m_Cf / (2.0f * std::pow(L, 3)) * std::pow(s, 4);
	}

	real G5Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G5Spiral::computeCurvature(real s, real L)
	{
		//\kappa \left( s \right) =-\frac{20C_f}{L^7}s^7+\frac{70C_f}{L^6}s^6-\frac{84C_f}{L^5}s^5+\frac{35C_f}{L^4}s^4
		return -20.0f * m_Cf / std::pow(L, 7) * std::pow(s, 7) +
			70.0f * m_Cf / std::pow(L, 6) * std::pow(s, 6) -
			84.0f * m_Cf / std::pow(L, 5) * std::pow(s, 5) +
			35.0f * m_Cf / std::pow(L, 4) * std::pow(s, 4);
	}

	real G5Spiral::computeCurvatureIntegral(real s, real L)
	{
		//\theta \left( s \right) =-\frac{5C_f}{2L^7}s^8+\frac{10C_f}{L^6}s^7-\frac{42C_f}{3L^5}s^6+\frac{7C_f}{L^4}s^5
		return -5.0f * m_Cf / (2.0f * std::pow(L, 7)) * std::pow(s, 8) +
			10.0f * m_Cf / std::pow(L, 6) * std::pow(s, 7) -
			42.0f * m_Cf / (3.0f * std::pow(L, 5)) * std::pow(s, 6) +
			7.0f * m_Cf / std::pow(L, 4) * std::pow(s, 5);
	}

	real G6Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G6Spiral::computeCurvature(real s, real L)
	{
		// \kappa \left( s \right) =\frac{70\!\:C_f}{L^9}s^9-\frac{315\!\:C_f}{L^8}s^8+\frac{540\!\:C_f}{L^7}s^7-\frac{420\!\:C_f}{L^6}s^6+\frac{126\!\:C_f}{L^5}s^5

		return 70.0f * m_Cf / std::pow(L, 9) * std::pow(s, 9) -
			315.0f * m_Cf / std::pow(L, 8) * std::pow(s, 8) +
			540.0f * m_Cf / std::pow(L, 7) * std::pow(s, 7) -
			420.0f * m_Cf / std::pow(L, 6) * std::pow(s, 6) +
			126.0f * m_Cf / std::pow(L, 5) * std::pow(s, 5);
	}

	real G6Spiral::computeCurvatureIntegral(real s, real L)
	{
		// \theta \left( s \right) =\frac{70\!\:C_f}{10L^9}s^{10}-\frac{315\!\:C_f}{9L^8}s^9+\frac{540\!\:C_f}{8L^7}s^8-\frac{420\!\:C_f}{7L^6}s^7+\frac{126\!\:C_f}{6L^5}s^6

		return 70.0f * m_Cf / (10.0f * std::pow(L, 9)) * std::pow(s, 10) -
			315.0f * m_Cf / (9.0f * std::pow(L, 8)) * std::pow(s, 9) +
			540.0f * m_Cf / (8.0f * std::pow(L, 7)) * std::pow(s, 8) -
			420.0f * m_Cf / (7.0f * std::pow(L, 6)) * std::pow(s, 7) +
			126.0f * m_Cf / (6.0f * std::pow(L, 5)) * std::pow(s, 6);
	}

	real G7Spiral::computeArcLength()
	{
		return 2.0f * m_arccosx / m_Cf;
	}

	real G7Spiral::computeCurvature(real s, real L)
	{
		//\kappa \left( s \right) =-\frac{252\!\:C_f}{L^{11}}s^{11}+\frac{1386\!\:C_f}{L^{10}}s^{10}-\frac{3080\!\:C_f}{L^9}s^9+\frac{3465\!\:C_f}{L^8}s^8-\frac{1980\!\:C_f}{L^7}s^7+\frac{462\!\:C_f}{L^6}s^6

		return -252.0f * m_Cf / std::pow(L, 11) * std::pow(s, 11) +
			1386.0f * m_Cf / std::pow(L, 10) * std::pow(s, 10) -
			3080.0f * m_Cf / std::pow(L, 9) * std::pow(s, 9) +
			3465.0f * m_Cf / std::pow(L, 8) * std::pow(s, 8) -
			1980.0f * m_Cf / std::pow(L, 7) * std::pow(s, 7) +
			462.0f * m_Cf / std::pow(L, 6) * std::pow(s, 6);
	}

	real G7Spiral::computeCurvatureIntegral(real s, real L)
	{
		//\theta \left( s \right) =-\frac{252\!\:C_f}{12L^{11}}s^{12}+\frac{1386\!\:C_f}{11L^{10}}s^{11}-\frac{3080\!\:C_f}{10L^9}s^{10}+\frac{3465\!\:C_f}{9L^8}s^9-\frac{1980\!\:C_f}{8L^7}s^8+\frac{462\!\:C_f}{7L^6}s^7

		return -252.0f * m_Cf / (12.0f * std::pow(L, 11)) * std::pow(s, 12) +
			1386.0f * m_Cf / (11.0f * std::pow(L, 10)) * std::pow(s, 11) -
			3080.0f * m_Cf / (10.0f * std::pow(L, 9)) * std::pow(s, 10) +
			3465.0f * m_Cf / (9.0f * std::pow(L, 8)) * std::pow(s, 9) -
			1980.0f * m_Cf / (8.0f * std::pow(L, 7)) * std::pow(s, 8) +
			462.0f * m_Cf / (7.0f * std::pow(L, 6)) * std::pow(s, 7);
	}

	real Spiral::curvatureAt(real s)
	{
		real L = m_spiral->computeArcLength();
		return m_spiral->computeCurvature(s, L);
	}

	Vector2 Spiral::sample(real s)
	{
		return {};
	}

	void Spiral::setSpiralShape(SpiralShapeBase* spiral)
	{
		if (m_spiral == spiral)
			return;

		m_needUpdateCurvePoints = true;
		m_spiral = spiral;
	}

	SpiralShapeBase* Spiral::spiralShape() const
	{
		return m_spiral;
	}

	void Spiral::sampleCurvePoints()
	{
		if (m_spiral == nullptr)
			return;

		m_curvePoints.clear();
		m_curvaturePoints.clear();

		real L = m_spiral->computeArcLength();

		m_spiral->computePoints(m_curvePoints, m_curvaturePoints, L, m_count);
	}

	void Spiral::sampleCurvaturePoints()
	{

	}

	std::vector<Vector2> Spiral::curvePoints()
	{
		if (m_spiral->needUpdate() || m_needUpdateCurvePoints)
		{
			sampleCurvePoints();
			m_spiral->setNeedUpdate(false);
			m_needUpdateCurvePoints = false;
		}
		return m_curvePoints;
	}

	std::vector<Vector2> Spiral::curvaturePoints()
	{
		return m_curvaturePoints;
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

	void RationalCubicBezier::setWeights(real w0, real w1, real w2, real w3)
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

	std::array<real, 4> RationalCubicBezier::weights() const
	{
		return m_weights;
	}

	Vector2 RationalCubicBezier::pointAt(size_t index) const
	{
		return m_points[index];
	}

	real RationalCubicBezier::weightAt(size_t index) const
	{
		return m_weights[index];
	}

	Vector2 RationalCubicBezier::sample(real t)
	{
		real u0 = m_weights[0] * Math::bernstein(t, 0.0f, 3.0f);
		real u1 = m_weights[1] * Math::bernstein(t, 1.0f, 3.0f);
		real u2 = m_weights[2] * Math::bernstein(t, 2.0f, 3.0f);
		real u3 = m_weights[3] * Math::bernstein(t, 3.0f, 3.0f);

		Vector2 result = (m_points[0] * u0 + m_points[1] * u1 + m_points[2] * u2 + m_points[3] * u3) / (u0 + u1 + u2 + u3);

		return result;
	}

	Vector2 RationalCubicBezier::tangent(real t) const
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

		return dp;
	}


	real RationalCubicBezier::curvatureAt(real t)
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

		det = std::pow(
			std::pow(t - 1.0f, 3.0f) * m_weights[0] -
			t * (3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2])), 3.0f);


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

		Vector2 lastPoint;
		for (size_t i = 0; i <= m_count; ++i)
		{
			float t = static_cast<float>(i) / static_cast<float>(m_count);
			Vector2 p = sample(t);
			if (i == 0)
			{
				lastPoint = p;
				m_curvePoints.push_back(p);
				continue;
			}

			if ((lastPoint - p).length() > m_threshold && i != m_count)
			{
				m_curvePoints.push_back(p);
				lastPoint = p;
			}
			if (i == m_count)
				m_curvePoints.push_back(p);
		}

	}

	void RationalCubicBezier::sampleCurvaturePoints()
	{
		m_curvaturePoints.clear();
		Vector2 lastPoint;

		for (size_t i = 0; i <= m_count; ++i)
		{
			float t = static_cast<float>(i) / static_cast<float>(m_count);

			Vector2 p1 = sample(t);

			if (!lastPoint.isOrigin() && (lastPoint - p1).length() <= m_threshold && i != m_count)
				continue;


			lastPoint = p1;

			// \left( (t-1)^3w_0-t\left( 3(t-1)^2w_1+t\left( tw_3-3(t-1)w_2 \right) \right) \right) ^2
			float det = std::pow(
				std::pow(t - 1.0f, 3.0f) * m_weights[0] -
				t * (3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2])), 2.0f);

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

			// \left( (t-1)^3w_0-t\left( 3(t-1)^2w_1+t\left( tw_3-3(t-1)w_2 \right) \right) \right) ^3

			det = std::pow(
				std::pow(t - 1.0f, 3.0f) * m_weights[0] -
				t * (3.0f * std::pow(t - 1.0f, 2.0f) * m_weights[1] + t * (t * m_weights[3] - 3.0f * (t - 1.0f) * m_weights[2])), 3.0f);

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

	Vector2 CubicBezier::sample(real t)
	{
		return Math::bernstein(t, 0, 3) * m_points[0] +
			Math::bernstein(t, 1, 3) * m_points[1] +
			Math::bernstein(t, 2, 3) * m_points[2] +
			Math::bernstein(t, 3, 3) * m_points[3];
	}

	real CubicBezier::curvatureAt(real t)
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

		Vector2 lastPoint;
		for(size_t i = 0;i <= m_count; ++i)
		{
			float t = static_cast<float>(i) / static_cast<float>(m_count);
			Vector2 p = sample(t);
			if (i == 0)
			{
				lastPoint = p;
				m_curvePoints.push_back(p);
				continue;
			}

			if ((lastPoint - p).length() > m_threshold && i != m_count)
			{
				m_curvePoints.push_back(p);
				lastPoint = p;
			}
			if (i == m_count)
				m_curvePoints.push_back(p);
		}
	}

	void CubicBezier::sampleCurvaturePoints()
	{
		m_curvaturePoints.clear();

		Vector2 lastPoint;

		for (size_t i = 0; i <= m_count; ++i)
		{
			float t = static_cast<float>(i) / static_cast<float>(m_count);

			Vector2 p1 = sample(t);

			if (!lastPoint.isOrigin() && (lastPoint - p1).length() <= m_threshold && i != m_count)
				continue;


			lastPoint = p1;

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
		if (m_curvaturePoints.size() != m_curvePoints.size())
			__debugbreak();
	}

}
