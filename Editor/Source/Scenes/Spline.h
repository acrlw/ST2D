#pragma once

#include "ST2DCore.h"

#include "AbstractCurve.h"
namespace STEditor
{
	using namespace ST;

	class BSpline : public  AbstractCurve
	{
	public:

		void addControlPoints(const std::vector<Vector2>& points);
		void addKnots(const std::vector<real>& knots);
		void addControlPoint(const Vector2& point);
		void addKnot(real knot);

		float curvatureAt(float t) override;
		Vector2 sample(float t) override;

	protected:
		void sampleCurvePoints() override;
		void sampleCurvaturePoints() override;

	private:

		std::vector<Vector2> m_controlPoints;
		std::vector<real> m_knots;
	};
	

}