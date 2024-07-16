#pragma once

#include "ST2DCore.h"

namespace STEditor
{
	using namespace ST;
	class RationalCubicBezier
	{
	public:

		void set(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float w0 = 1.0f, float w1 = 1.0f, float w2 = 1.0f, float w3 = 1.0f);

		Vector2& pointAt(size_t index);

		float& weightAt(size_t index);

		Vector2 sample(float t)const;

		std::vector<Vector2> samplePoints(size_t count)const;

		float curvatureAD(float t)const;

		float curvature(float t)const;

		std::vector<Vector2> curvaturePoints(size_t count, float scale = 1.0, bool flip = false) const;

	private:
		std::array<Vector2, 4> m_points;
		//std::array<float, 4> m_weights = {1.0006114f, 0.9667583f, 0.8220000f, 0.8360001f };
		//std::array<float, 4> m_weights = { 0.5235227f, 0.9434174f, 0.8017000f, 0.8254001f };
		std::array<float, 4> m_weights = { 0.8029640f, 0.9434249f, 0.8017000f, 0.8254001f };
	};

	class RationalQuadraticBezier
	{

	public:

		void set(const Vector2& p0, const Vector2& p1, const Vector2& p2, float w0 = 1.0f, float w1 = 1.0f, float w2 = 1.0f);

		Vector2& pointAt(size_t index);

		float& weightAt(size_t index);

		Vector2 sample(float t)const;

	private:
		std::array<Vector2, 3> m_points;
		std::array<float, 3> m_weights = { 1.0f, 1.0f, 1.0f };
	};

	class CubicBezier
	{
	public:

		static CubicBezier fromControlPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		static CubicBezier fromHermite(const Vector2& p0, const Vector2& dir0, const Vector2& p1, const Vector2& dir1);


		float torsion(float t)const;

		Vector2 sample(float t)const;

		float curvature(float t)const;

		std::vector<Vector2> curvaturePoints(size_t count, float scale = 1.0, bool flip = false) const;

		Vector2& operator[](size_t index);

		Vector2& at(size_t index);

		void setPoints(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);

		std::vector<Vector2> samplePoints(size_t count);

	private:
		std::array<Vector2, 4> m_points;
	};

	class AbstractSpline
	{
	public:
		virtual ~AbstractSpline() = default;
		virtual std::vector<Vector2> sample(size_t count) = 0;

		void addPoints(const std::vector<Vector2>& points)
		{
			m_points.insert(m_points.end(), points.begin(), points.end());
		}

		Vector2& at(size_t index)
		{
			return m_points[index];
		}

	protected:
		std::vector<Vector2> m_points;
	};

	class CatmullRomSpline : public AbstractSpline
	{

	public:
		std::vector<Vector2> sample(size_t count) override
		{
			std::vector<Vector2> result;

			return result;
		}

	};

	class BSpline : public AbstractSpline
	{
	public:
		std::vector<Vector2> sample(size_t count) override
		{
			std::vector<Vector2> result;

			return result;

		}

	private:
	};
}