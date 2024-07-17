#pragma once

#include "ST2DCore.h"

// autodiff include
#include <autodiff/reverse/var.hpp>
#include <Eigen/Dense>
#include "AbstractCurve.h"

namespace STEditor
{
	using namespace ST;
	using namespace autodiff;

	// A type defining parameters for a function of interest

	struct Params
	{
		var p0, p1, p2, p3;
	};

	var bezier3Func(var t, const Params& param);


	class CubicBezierAD
	{
	public:

		CubicBezierAD(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		Vector2 sample(float t)const;

		std::vector<Vector2> curvePoints();

		std::vector<Vector2> curvaturePoints();

		float curvatureAt(float t);

	private:


		void samplePoints();

		void sampleCurvaturePoints();

		Params m_params1;
		Params m_params2;
		std::vector<Vector2> m_curvePoints;
		std::vector<Vector2> m_curvaturePoints;
		bool m_needUpdateCurvePoints = true;
		bool m_needUpdateCurvaturePoints = true;

		size_t m_count = 50;
	};

	class RationalCubicBezier : public AbstractCurve
	{
	public:

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);
		void setWeights(float w0, float w1, float w2, float w3);

		std::array<Vector2, 4> points() const;
		std::array<float, 4> weights() const;

		Vector2 pointAt(size_t index) const;

		float weightAt(size_t index) const;

		Vector2 sample(float t)const override;

		float curvatureAt(float t)const override;

	protected:

		void sampleCurvePoints()override;
		void sampleCurvaturePoints()override;

	private:
		std::array<Vector2, 4> m_points;
		//std::array<float, 4> m_weights = {1.0006114f, 0.9667583f, 0.8220000f, 0.8360001f };
		//std::array<float, 4> m_weights = { 0.5235227f, 0.9434174f, 0.8017000f, 0.8254001f };
		std::array<float, 4> m_weights = { 0.8029640f, 0.9434249f, 0.8017000f, 0.8254001f };
	};


	class CubicBezier : public AbstractCurve
	{
	public:

		static CubicBezier fromControlPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		static CubicBezier fromHermite(const Vector2& p0, const Vector2& dir0, const Vector2& p1, const Vector2& dir1);



		Vector2 sample(float t)const override;

		float curvatureAt(float t)const override;

		std::array<Vector2, 4> points() const;
		Vector2 pointAt(size_t index) const;
		Vector2 operator[](size_t index) const;

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);


	protected:
		void sampleCurvePoints() override;
		void sampleCurvaturePoints() override;


	private:

		std::array<Vector2, 4> m_points;
		
	};
}