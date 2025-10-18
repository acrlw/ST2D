#include "ShapeBase.h"
#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	ShapeBase::ShapeBase(ShapeBaseType type): type(type)
	{
		switch (type)
		{
		case ShapeBaseType::Capsule:
			capsule = CapsuleShape{};
			break;
		case ShapeBaseType::Circle:
			circle = CircleShape{};
			break;
		case ShapeBaseType::Ellipse:
			ellipse = EllipseShape{};
			break;
		case ShapeBaseType::Polygon:
			polygon = PolygonShape{};
			break;
		case ShapeBaseType::OneSidedSegment:
			oneSidedSegment = OneSidedSegment{};
			break;
		case ShapeBaseType::TwoSidedSegment:
			twoSidedSegment = TwoSidedSegment{};
			break;
		}
	}

	ShapeBase::ShapeBase(const ShapeBase& other): type(other.type)
	{
		switch (type)
		{
		case ShapeBaseType::Capsule: capsule = other.capsule; break;
		case ShapeBaseType::Circle: circle = other.circle; break;
		case ShapeBaseType::Ellipse: ellipse = other.ellipse; break;
		case ShapeBaseType::Polygon: polygon = other.polygon; break;
		case ShapeBaseType::OneSidedSegment: oneSidedSegment = other.oneSidedSegment; break;
		case ShapeBaseType::TwoSidedSegment: twoSidedSegment = other.twoSidedSegment; break;
		}
	}

	ShapeBase& ShapeBase::operator=(const ShapeBase& rhs)
	{
		if (this == &rhs) {
			return *this;
		}
		type = rhs.type;
		switch (type)
		{
		case ShapeBaseType::Capsule: capsule = rhs.capsule; break;
		case ShapeBaseType::Circle: circle = rhs.circle; break;
		case ShapeBaseType::Ellipse: ellipse = rhs.ellipse; break;
		case ShapeBaseType::Polygon: polygon = rhs.polygon; break;
		case ShapeBaseType::OneSidedSegment: oneSidedSegment = rhs.oneSidedSegment; break;
		case ShapeBaseType::TwoSidedSegment: twoSidedSegment = rhs.twoSidedSegment; break;
		}
		return *this;
	}

	bool ShapeBase::contains(const Vector2& localPoint, const real& epsilon) const
	{
		bool result = false;
		switch (type)
		{
		case ShapeBaseType::Capsule:
			{

				const Vector2 anchor1(0, capsule.halfLength);
				const Vector2 anchor2(0, -capsule.halfLength);
				const Vector2 p = Algorithm2D::pointToSegment(anchor1, anchor2, localPoint);
				result = (localPoint - p).norm() < capsule.radius;
				break;
			}
		case ShapeBaseType::Circle:
			{

				result = (circle.radius * circle.radius - localPoint.square()) > epsilon;
				break;
			}
		case ShapeBaseType::Ellipse:
			{

				const real a = ellipse.halfWidth > ellipse.halfHeight ? ellipse.halfWidth : ellipse.halfHeight;
				const real b = ellipse.halfWidth < ellipse.halfHeight ? ellipse.halfWidth : ellipse.halfHeight;
				CORE_ASSERT(!realEqual(a, 0) && !realEqual(b, 0), "Invalid Ellipse")
				result = (localPoint.x / a) * (localPoint.x / a) + (localPoint.y / b) * (localPoint.y / b) <= 1.0f;
				break;
			}
		case ShapeBaseType::Polygon:
			{

				for (uint32_t i = 0; i < polygon.count; ++i)
				{
					uint32_t next1 = i + 1 == polygon.count ? 0 : i + 1;
					uint32_t next2 = next1 + 1 == polygon.count ? 0 : next1 + 1;
					result = Algorithm2D::checkPointsOnSameSide(polygon.vertices[i], polygon.vertices[next1],
					                                            polygon.vertices[next2], localPoint);
					if (!result)
						break;
				}
				break;
			}
		case ShapeBaseType::OneSidedSegment:
			{

				const Vector2 pp1 = Algorithm2D::pointToSegment(oneSidedSegment.p[0], oneSidedSegment.p[1], localPoint);
				result &= (localPoint - pp1).dot(oneSidedSegment.normal) > 0;
				result &= (localPoint - pp1).square() < epsilon * epsilon;
				break;
			}
		case ShapeBaseType::TwoSidedSegment:
			{
				const Vector2 pp2 = Algorithm2D::pointToSegment(twoSidedSegment.p[0], twoSidedSegment.p[1], localPoint);
				result = (localPoint - pp2).square() < epsilon * epsilon;
				break;
			}
		}
		return result;
	}

	void ShapeBase::scale(const real& factor)
	{
		switch (type)
		{
		case ShapeBaseType::Capsule:
			capsule.radius *= factor;
			capsule.halfLength *= factor;
			break;
		case ShapeBaseType::Circle:
			circle.radius *= factor;
			break;
		case ShapeBaseType::Ellipse:
			ellipse.halfHeight *= factor;
			ellipse.halfWidth *= factor;
			break;
		case ShapeBaseType::Polygon:
			for (uint32_t i = 0; i < polygon.count; ++i)
				polygon.vertices[i] *= factor;
			break;
		case ShapeBaseType::OneSidedSegment:
			break;
		case ShapeBaseType::TwoSidedSegment:
			break;
		}
	}

	ShapeBase ShapeBase::makePolygon(Vector2& vertices, const uint32_t& count)
	{
		ShapeBase shape(ShapeBaseType::Polygon);
		for (uint32_t i = 0; i < count; ++i)
		{
			uint32_t nextIdx = i + 1 == count ? 0 : i + 1;
			Vector2 edge = vertices[nextIdx] - vertices[i];
			shape.polygon.normals[i] = edge.ortho().negate().normal();
			shape.polygon.vertices[i] = vertices[i];
		}
		shape.polygon.count = count;
		Vector2 center = Algorithm2D::computeCenter(shape.polygon.vertices.data(), shape.polygon.count);
		for (auto& elem : shape.polygon.vertices)
			elem -= center;
		return shape;
	}

	ShapeBase ShapeBase::makePolygon(const std::initializer_list<Vector2>& vertices)
	{
		ShapeBase shape(ShapeBaseType::Polygon);
		uint32_t i = 0;
		for (auto it = vertices.begin(); it != vertices.end(); ++it, ++i)
		{
			auto nextIt = it + 1 == vertices.end() ? vertices.begin() : it + 1;

			Vector2 edge = *nextIt - *it;
			const real square = edge.square();
			CORE_ASSERT(square > 1e-8f, "Invalid Polygon")
			shape.polygon.normals[i] = edge.ortho().negate().normal();
			shape.polygon.vertices[i] = *it;
		}
		shape.polygon.count = vertices.size();
		Vector2 center = Algorithm2D::computeCenter(shape.polygon.vertices.data(), shape.polygon.count);
		for (auto& elem : shape.polygon.vertices)
			elem -= center;
		return shape;
	}

	ShapeBase ShapeBase::makePolygon(const std::array<Vector2, Constant::MaxPolygonVertices>& vertices,
		const uint32_t& count)
	{
		ShapeBase shape(ShapeBaseType::Polygon);
		std::ranges::copy(vertices, shape.polygon.vertices.begin());
		shape.polygon.count = count;
		Vector2 center = Algorithm2D::computeCenter(shape.polygon.vertices.data(), shape.polygon.count);
		for (auto& elem : shape.polygon.vertices)
			elem -= center;
		return shape;
	}

	ShapeBase ShapeBase::makeBox(const real& halfWidth, const real& halfHeight)
	{
		ShapeBase shape(ShapeBaseType::Polygon);
		shape.polygon.count = 4;
		shape.polygon.vertices[0] = Vector2(-halfWidth, halfHeight);
		shape.polygon.vertices[1] = Vector2(-halfWidth, -halfHeight);
		shape.polygon.vertices[2] = Vector2(halfWidth, -halfHeight);
		shape.polygon.vertices[3] = Vector2(halfWidth, halfHeight);
		shape.polygon.normals[0].set(-1, 0);
		shape.polygon.normals[1].set(0, -1);
		shape.polygon.normals[2].set(1, 0);
		shape.polygon.normals[3].set(0, 1);
		return shape;
	}

	ShapeBase ShapeBase::makeCircle(const real& radius)
	{
		ShapeBase shape(ShapeBaseType::Circle);
		CORE_ASSERT(!realEqual(radius, 0), "Invalid Circle")
		shape.circle.radius = radius;
		return shape;
	}

	ShapeBase ShapeBase::makeEllipse(const real& halfWidth, const real& halfHeight)
	{
		ShapeBase shape(ShapeBaseType::Ellipse);
		CORE_ASSERT(!realEqual(halfWidth, 0) && !realEqual(halfHeight, 0), "Invalid Ellipse")
		shape.ellipse.halfWidth = halfWidth;
		shape.ellipse.halfHeight = halfHeight;
		return shape;
	}

	ShapeBase ShapeBase::makeCapsule(const real& halfLength, const real& radius)
	{
		ShapeBase shape(ShapeBaseType::Capsule);
		CORE_ASSERT(!realEqual(halfLength, 0) && !realEqual(radius, 0), "Invalid Capsule")
		shape.capsule.radius = radius;
		shape.capsule.halfLength = halfLength;
		return shape;
	}

	ShapeBase ShapeBase::makeOneSidedSegment(const Vector2& p1, const Vector2& p2, const Vector2& normal)
	{
		ShapeBase shape(ShapeBaseType::OneSidedSegment);
		const real square = (p2 - p1).square();
		const real sqNorm = normal.square();
		CORE_ASSERT(!realEqual(square, 0) && !realEqual(sqNorm, 0), "Invalid One-Sided Segment")
		shape.oneSidedSegment.p[0] = p1;
		shape.oneSidedSegment.p[1] = p2;
		shape.oneSidedSegment.normal = normal;
		return shape;
	}

	ShapeBase ShapeBase::makeTwoSidedSegment(const Vector2& p1, const Vector2& p2)
	{
		ShapeBase shape(ShapeBaseType::TwoSidedSegment);
		const real square = (p2 - p1).square();
		CORE_ASSERT(!realEqual(square, 0), "Invalid Two-Sided Segment")
		shape.twoSidedSegment.p[0] = p1;
		shape.twoSidedSegment.p[1] = p2;
		return shape;
	}
}
