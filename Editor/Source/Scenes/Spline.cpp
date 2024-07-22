#include "Spline.h"

namespace STEditor
{
	void BSpline::addControlPoints(const std::vector<Vector2>& points)
	{
		for (const auto& point : points)
			m_controlPoints.push_back(point);
		
	}

	void BSpline::addKnots(const std::vector<real>& knots)
	{
		for (const auto& knot : knots)
			m_knots.push_back(knot);
	}

	void BSpline::addControlPoint(const Vector2& point)
	{
		m_controlPoints.push_back(point);
	}

	void BSpline::addKnot(real knot)
	{
		m_knots.push_back(knot);
	}

	float BSpline::curvatureAt(float t)
	{
		return 0;
	}

	Vector2 BSpline::sample(float t)
	{
		return {};
	}

	void BSpline::sampleCurvePoints()
	{
	}

	void BSpline::sampleCurvaturePoints()
	{
	}
}
