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

	class RationalCubicBezierAD : public AbstractCurve
	{
	public:

		RationalCubicBezierAD(const Vector2& p0 = {}, const Vector2& p1 = {}, const Vector2& p2 = {}, const Vector2& p3 = {}
		, real w0 = 0.8029640f, real w1 = 0.9434249f, real w2 = 0.8017000f, real w3 = 0.8254001f);

		struct Params
		{
			var p0, p1, p2, p3, w0, w1, w2, w3;
		};

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);
		void setWeights(float w0, float w1, float w2, float w3);

		static var bezierFunc(var t, const Params& param);

		real curvatureAt(real t) override;
		Vector2 sample(real t) override;

		Vector2 tangent(real t);

		std::array<Vector2, 4> points() const;
		std::array<real, 4> weights() const;

		Vector2 pointAt(size_t index) const;

		float weightAt(size_t index) const;
	protected:
		void sampleCurvePoints() override;
		void sampleCurvaturePoints() override;

	private:

		std::array<Vector2, 4> m_points;
		std::array<real, 4> m_weights = { 0.8029640f, 0.9434249f, 0.8017000f, 0.8254001f };

	};


	class CubicBezierAD : public AbstractCurve
	{
	public:
		struct Params
		{
			var p0, p1, p2, p3;
		};

		static var bezierFunc(var t, const Params& param);


		CubicBezierAD(const Vector2& p0 = {}, const Vector2& p1 = {}, const Vector2& p2 = {}, const Vector2& p3 = {});

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		real curvatureAt(real t) override;

		Vector2 sample(real t) override;

		std::array<Vector2, 4> points() const;
		Vector2 pointAt(size_t index) const;
		Vector2 operator[](size_t index) const;

	protected:
		void sampleCurvePoints() override;
		void sampleCurvaturePoints() override;

	private:
		Vector2 tangent(float t);
		std::array<Vector2, 4> m_points;

	};

	class SpiralShapeBase
	{
	public:
		virtual ~SpiralShapeBase() = default;
		void setParam(real Cf, real arccosx);

		virtual real computeArcLength() = 0;
		virtual real computeCurvature(real s, real L) = 0;
		virtual real computeCurvatureIntegral(real s, real L) = 0;

		void computePoints(std::vector<Vector2>& curvePoints, std::vector<Vector2>& curvaturePoints, real L, size_t count);

		bool needUpdate() const;
		void setNeedUpdate(bool needUpdate);
	protected:
		real m_Cf = 1.0f;
		real m_arccosx = 1.0f;
		bool m_needUpdate = true;
	};

	class TwoWeightCubicBezierSpiral : public SpiralShapeBase
	{

	public:
		void setWeights(real w0, real w1);
		real weight0() const;
		real weight1() const;
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;


	private:
		real m_w0 = 0.5f;
		real m_w1 = 0.5f;
	};

	class OneWeightCubicBezierSpiral : public SpiralShapeBase
	{
	public:
		void setWeight(real w);
		real weight() const;
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;


	private:
		real m_w = 0.5;
	};

	class G2Spiral : public SpiralShapeBase
	{
	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;
	};

	class G3Spiral : public SpiralShapeBase
	{
	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;

	private:
	};

	class G4Spiral : public SpiralShapeBase
	{
	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;

	private:
	};

	class G5Spiral: public SpiralShapeBase
	{
	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;
	};

	class G6Spiral : public SpiralShapeBase
	{

	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;
	};

	class G7Spiral : public SpiralShapeBase
	{

	public:
		real computeArcLength() override;
		real computeCurvature(real s, real L) override;
		real computeCurvatureIntegral(real s, real L) override;
	};

	

	class Spiral : public AbstractCurve
	{
	public:

		real curvatureAt(real s) override;
		Vector2 sample(real s) override;
		void setSpiralShape(SpiralShapeBase* spiral);
		SpiralShapeBase* spiralShape() const;
	protected:
		void sampleCurvePoints() override;
		void sampleCurvaturePoints() override;

	public:
		std::vector<Vector2> curvePoints() override;
		std::vector<Vector2> curvaturePoints() override;

	private:
		SpiralShapeBase * m_spiral = nullptr; 
	};

	class RationalCubicBezier : public AbstractCurve
	{
	public:

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);
		void setWeights(real w0, real w1, real w2, real w3);

		std::array<Vector2, 4> points() const;
		std::array<real, 4> weights() const;

		Vector2 pointAt(size_t index) const;

		real weightAt(size_t index) const;

		Vector2 sample(real t) override;

		Vector2 tangent(real t) const;

		real curvatureAt(real t) override;

	protected:

		void sampleCurvePoints()override;
		void sampleCurvaturePoints()override;

	private:
		std::array<Vector2, 4> m_points;
		//std::array<float, 4> m_weights = {1.0006114f, 0.9667583f, 0.8220000f, 0.8360001f };
		//std::array<float, 4> m_weights = { 0.5235227f, 0.9434174f, 0.8017000f, 0.8254001f };
		std::array<real, 4> m_weights = { 0.085, 0.24000002, 0.2098289430, 0.183451116 };
	};


	class CubicBezier : public AbstractCurve
	{
	public:

		static CubicBezier fromControlPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		static CubicBezier fromHermite(const Vector2& p0, const Vector2& dir0, const Vector2& p1, const Vector2& dir1);

		Vector2 sample(real t) override;

		real curvatureAt(real t) override;

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