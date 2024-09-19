#include "Polygon.h"
#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	Polygon::Polygon()
	{
		m_type = ShapeType::Polygon;
		m_vertices.reserve(4);
	}

	const std::vector<Vector2>& Polygon::vertices() const
	{
		return m_vertices;
	}

	void Polygon::set(const std::initializer_list<Vector2>& vertices)
	{
		for (const Vector2& vertex : vertices)
			m_vertices.emplace_back(vertex);
		updateVertices();
	}

	void Polygon::set(std::vector<Vector2>& vertices)
	{
		m_vertices = std::move(vertices);
		updateVertices();
	}

	Vector2 Polygon::center()const
	{
		return Algorithm2D::computeCenter(this->vertices());
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

	void Polygon::updateVertices()
	{
		Vector2 center = this->center();
		for (auto& elem : m_vertices)
			elem -= center;
	}
}