#include "Rectangle.h"
namespace ST
{
	Rectangle::Rectangle(const real& width, const real& height)
	{
		m_type = ShapeType::Polygon;
		this->set(width, height);
	}

	void Rectangle::set(const real& width, const real& height)
	{
		m_width = width;
		m_height = height;
		calcVertices();
	}

	real Rectangle::width() const
	{
		return m_width;
	}

	real Rectangle::height() const
	{
		return m_height;
	}

	void Rectangle::setWidth(const real& width)
	{
		m_width = width;
		calcVertices();
	}

	void Rectangle::setHeight(const real& height)
	{
		m_height = height;
		calcVertices();
	}
	void Rectangle::scale(const real& factor)
	{
		m_width *= factor;
		m_height *= factor;
		calcVertices();
	}
	bool Rectangle::contains(const Vector2& point, const real& epsilon)
	{
		return (point.x < m_width / 2.0 && point.x > -m_width / 2.0) &&
			point.y < m_height / 2.0 && point.y > -m_height / 2.0;
	}
	void Rectangle::calcVertices()
	{
		m_count = 4;
		m_vertices[0] = Vector2(-m_width * (0.5f), m_height * (0.5f));
		m_vertices[1] = Vector2(-m_width * (0.5f), -m_height * (0.5f));
		m_vertices[2] = Vector2(m_width * (0.5f), -m_height * (0.5f));
		m_vertices[3] = Vector2(m_width * (0.5f), m_height * (0.5f));
	}
}