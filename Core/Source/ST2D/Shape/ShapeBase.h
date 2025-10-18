#pragma once

#include "ST2D/Log.h"
#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Math/Complex.h"
#include "ST2D/Math/Matrix2x2.h"

namespace ST
{
	struct ST_API CapsuleShape
	{
		real halfLength = 0;
		real radius = 0;
	};

	struct ST_API CircleShape
	{
		real radius = 0;
	};

	struct ST_API EllipseShape
	{
		real halfWidth = 0;
		real halfHeight = 0;
	};

	struct ST_API PolygonShape
	{
		std::array<Vector2, Constant::MaxPolygonVertices> vertices;
		std::array<Vector2, Constant::MaxPolygonVertices> normals;
		uint32_t count = 0;
	};

	struct ST_API OneSidedSegment
	{
		//0: start
		//1: end
		Vector2 p[2];
		Vector2 normal;
	};

	struct ST_API TwoSidedSegment
	{
		//0: start
		//1: end
		Vector2 p[2];
	};

	enum class ShapeBaseType
	{
		Capsule,
		Circle,
		Ellipse,
		Polygon,
		OneSidedSegment,
		TwoSidedSegment
	};

	struct ST_API ShapeBase
	{
		ShapeBaseType type;
		union
		{
			CapsuleShape capsule;
			CircleShape circle;
			EllipseShape ellipse;
			PolygonShape polygon;
			OneSidedSegment oneSidedSegment;
			TwoSidedSegment twoSidedSegment;
		};

		ShapeBase(ShapeBaseType type);
		~ShapeBase() = default;
		ShapeBase(const ShapeBase& other);

		ShapeBase& operator=(const ShapeBase& rhs);


		bool contains(const Vector2& localPoint, const real& epsilon = Constant::GeometryEpsilon)const;

		void scale(const real& factor);

		static ShapeBase makePolygon(Vector2& vertices, const uint32_t& count);

		static ShapeBase makePolygon(const std::initializer_list<Vector2>& vertices);

		static ShapeBase makePolygon(const std::array<Vector2, Constant::MaxPolygonVertices>& vertices, const uint32_t& count);

		static ShapeBase makeBox(const real& halfWidth, const real& halfHeight);

		static ShapeBase makeCircle(const real& radius);

		static ShapeBase makeEllipse(const real& halfWidth, const real& halfHeight);

		static ShapeBase makeCapsule(const real& halfLength, const real& radius);

		static ShapeBase makeOneSidedSegment(const Vector2& p1, const Vector2& p2, const Vector2& normal);

		static ShapeBase makeTwoSidedSegment(const Vector2& p1, const Vector2& p2);
	};
}
