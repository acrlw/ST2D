#include "Capsule.h"

#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{

	Capsule::Capsule(real halfLength, real radius) : m_halfLength(halfLength), m_radius(radius)
	{
		m_type = ShapeType::Capsule;
	}
	bool Capsule::contains(const Vector2& point, const real& epsilon)
	{
		Vector2 anchor1(0, m_halfLength);
		Vector2 anchor2(0, -m_halfLength);
		Vector2 p = Algorithm2D::pointToSegment(anchor1, anchor2, point);
		bool result = (point - p).norm() < m_radius;
		return result;
	}

	void Capsule::scale(const real& factor)
	{
		m_halfLength *= factor;
		m_radius *= factor;
	}

	Vector2 Capsule::center() const
	{
		return {};
	}

	void Capsule::set(real halfLength, real radius)
	{
		m_halfLength = halfLength;
		m_radius = radius;
	}

	void Capsule::setRadius(real radius)
	{
		m_radius = radius;
	}

	void Capsule::setHalfLength(real halfLength)
	{
		m_halfLength = halfLength;
	}

	real Capsule::halfLength() const
	{
		return m_halfLength;
	}

	real Capsule::radius() const
	{
		return m_radius;
	}

	Vector2 Capsule::topLeft() const
	{
		return {-m_radius, m_halfLength + m_radius};
	}
	Vector2 Capsule::bottomLeft() const
	{
		return -topRight();
	}

	Vector2 Capsule::topRight() const
	{
		return { m_radius, m_halfLength + m_radius };
	}

	Vector2 Capsule::bottomRight() const
	{
		return -topLeft();
	}

	Vector2 Capsule::anchorTop() const
	{
		return { 0, m_halfLength };
	}

	Vector2 Capsule::anchorBottom() const
	{
		return { 0, -m_halfLength };
	}

	std::array<Vector2, 4> Capsule::boxVertices() const
	{
		std::array<Vector2, 4> vertices;
		vertices[0] = this->topLeft();
		vertices[1] = this->bottomLeft();
		vertices[2] = this->bottomRight();
		vertices[3] = this->topRight();
		return vertices;
	}
}
