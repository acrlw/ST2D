#include "Segment.h"
#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	Segment::Segment()
	{
		m_type = ShapeType::Segment;
	}

	void Segment::set(const Vector2& start, const Vector2& end)
	{
		m_point[0] = start;
		m_point[1] = end;
		m_normal = (m_point[1] - m_point[0]).ortho().normal().negate();
	}

	void Segment::setStartPoint(const Vector2& start)
	{
		m_point[0] = start;
	}

	void Segment::setEndPoint(const Vector2& end)
	{
		m_point[1] = end;
	}

	Vector2 Segment::startPoint() const
	{
		return m_point[0];
	}

	Vector2 Segment::endPoint() const
	{
		return m_point[1];
	}

	void Segment::scale(const real& factor)
	{
		m_point[0] *= factor;
		m_point[1] *= factor;
	}

	bool Segment::contains(const Vector2& point, const real& epsilon)
	{
		return Algorithm2D::checkPointOnSegment(m_point[0], m_point[1], point);
	}

	Vector2 Segment::center()const
	{
		return (m_point[0] + m_point[1]) / 2.0f;
	}

	Vector2 Segment::normal() const
	{
		return m_normal;
	}

	void Segment::setNormal(const Vector2& normal)
	{
		m_normal = normal;
	}
}