#pragma once

#include "ST2DCore.h"

namespace STEditor
{
	using namespace ST;
	class AbstractCurve
	{
	public:

		virtual ~AbstractCurve() = default;

		virtual real curvatureAt(real t) = 0;

		virtual Vector2 sample(real t) = 0;

		void setCount(size_t count)
		{
			if(m_count != count)
			{
				m_needUpdateCurvePoints = true;
				m_needUpdateCurvaturePoints = true;
			}
			m_count = count;
		}

		virtual std::vector<Vector2> curvePoints()
		{
			if(m_needUpdateCurvePoints)
			{
				sampleCurvePoints();
				m_needUpdateCurvePoints = false;
			}
			return m_curvePoints;
		}

		virtual std::vector<Vector2> curvaturePoints()
		{
			if(m_needUpdateCurvaturePoints)
			{
				sampleCurvaturePoints();
				m_needUpdateCurvaturePoints = false;
			}
			return m_curvaturePoints;
		
		}

	protected:

		virtual void sampleCurvePoints() = 0;
		virtual void sampleCurvaturePoints() = 0;

		std::vector<Vector2> m_curvePoints;
		std::vector<Vector2> m_curvaturePoints;
		bool m_needUpdateCurvePoints = true;
		bool m_needUpdateCurvaturePoints = true;
		size_t m_count = 100;
		float m_threshold = 1e-3;
	};
}