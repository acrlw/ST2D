#include "Polygon.h"

#include "ST2D/Log.h"
#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	Polygon::Polygon()
	{
		m_type = ShapeType::Polygon;
	}

	const std::array<Vector2, Constant::MaxPolygonVertices>& Polygon::vertices() const
	{
		return m_vertices;
	}

	void Polygon::set(const std::initializer_list<Vector2>& vertices)
	{
		CORE_ASSERT(vertices.size() < Constant::MaxPolygonVertices, "The number of inserted fixed points exceeds the upper limit");
		std::ranges::copy(vertices, m_vertices.begin());
		m_count = vertices.size();
		updateVertices();
	}

	void Polygon::set(const std::array<Vector2, Constant::MaxPolygonVertices>& vertices, const uint32_t& count)
	{
		std::ranges::copy(vertices, m_vertices.begin());
		m_count = count;
		updateVertices();
	}


	void Polygon::set(Vector2* vertices, const uint32_t& count)
	{
		CORE_ASSERT(count < Constant::MaxPolygonVertices, "The number of inserted fixed points exceeds the upper limit");
		for (uint32_t i = 0; i < count; ++i)
			m_vertices[i] = vertices[i];
		m_count = count;
		updateVertices();
	}

	Vector2 Polygon::center()const
	{
		return Algorithm2D::computeCenter(m_vertices.data(), m_count);
	}

	void Polygon::scale(const real& factor)
	{
		assert(!m_vertices.empty());
		for (Vector2& vertex : m_vertices)
			vertex *= factor;
	}

	bool Polygon::contains(const Vector2& point, const real& epsilon)
	{
		for (size_t i = 0;i < m_count; ++i)
		{
			size_t nextIdx = i + 1 == m_count ? 0 : i + 1;
			size_t refIdx = nextIdx + 1 == m_count ? 0 : nextIdx + 1;
			if (!Algorithm2D::checkPointsOnSameSide(m_vertices[i], m_vertices[nextIdx], m_vertices[refIdx], point))
				return false;
		}
		return true;
	}

	size_t Polygon::count() const
	{
		return m_count;
	}

	const std::array<uint32_t, Constant::MaxSupportFieldSize>& Polygon::supportField()const
	{
		return m_supportField;
	}

	void Polygon::updateVertices()
	{
		Vector2 center = this->center();
		for (size_t i = 0; i < m_count; ++i)
			m_vertices[i] -= center;
		
		buildSupportField();
	}

	void Polygon::buildSupportField()
	{
		real minDeg = Constant::Max;
		size_t startIdx = 0;
		std::array<uint32_t, Constant::MaxPolygonVertices> degree;
		for (size_t i = 0; i < m_count; ++i)
		{
			size_t nextIdx = i + 1 == m_count ? 0 : i + 1;
			real theta = (m_vertices[i] - m_vertices[nextIdx]).ortho().theta();
			if (theta < 0)
				theta += Constant::TwoPi;
			real deg = std::floor(Math::degree(theta));
			degree[i] = static_cast<uint32_t>(deg) % 360;
			if (minDeg > deg)
			{
				minDeg = deg;
				startIdx = i;
			}
		}

		uint32_t step = 360 / Constant::MaxSupportFieldSize;

		for (uint32_t i = 0, idxCounter = 0; i < Constant::MaxSupportFieldSize; ++i)
		{
			uint32_t deg = i * step;
			if (deg > degree[startIdx] && idxCounter < m_count)
			{
				startIdx = startIdx + 1 == m_count ? 0 : startIdx + 1;
				idxCounter++;
			}
			m_supportField[i] = startIdx;
		}
	}
}
