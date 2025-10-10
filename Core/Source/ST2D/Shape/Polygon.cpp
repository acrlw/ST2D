#include "Polygon.h"

#include "ST2D/Log.h"
#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	Polygon::Polygon()
	{
		m_type = ShapeType::Polygon;
	}

	const std::array<Vector2, MaxPolygonVertices>& Polygon::vertices() const
	{
		return m_vertices;
	}

	void Polygon::set(const std::initializer_list<Vector2>& vertices)
	{
		CORE_ASSERT(vertices.size() < MaxPolygonVertices, "The number of inserted fixed points exceeds the upper limit");
		std::ranges::copy(vertices, m_vertices.begin());
		m_count = vertices.size();
		updateVertices();
	}

	void Polygon::set(const std::array<Vector2, MaxPolygonVertices>& vertices, const uint32_t& count)
	{
		std::ranges::copy(vertices, m_vertices.begin());
		m_count = count;
		updateVertices();
	}


	void Polygon::set(Vector2* vertices, const uint32_t& count)
	{
		CORE_ASSERT(count < MaxPolygonVertices, "The number of inserted fixed points exceeds the upper limit");
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
		for (auto iter = m_vertices.begin(); iter != m_vertices.end(); ++iter)
		{
			auto next = iter + 1;
			if (next == m_vertices.end())
				next = m_vertices.begin();
			auto ref = next + 1;
			if (ref == m_vertices.end())
				ref = m_vertices.begin();
			if (!Algorithm2D::checkPointsOnSameSide(*iter, *next, *ref, point))
				return false;

		}
		return true;
	}

	size_t Polygon::count() const
	{
		return m_count;
	}

	void Polygon::updateVertices()
	{
		Vector2 center = this->center();
		for (auto& elem : m_vertices)
			elem -= center;
	}
}