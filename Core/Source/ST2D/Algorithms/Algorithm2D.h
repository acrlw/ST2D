#pragma once

#include "ST2D/Math/Vector2.h"

namespace ST
{
	class ST_API Clipper
	{
	public:
		/**
		 * \brief Sutherland Hodgman Polygon Clipping. All points is stored in counter clock winding.\n
		 * By convention:\n
		 *		p0 -> p1 -> p2 -> p0 constructs a triangle
		 * \param polygon
		 * \param clipRegion
		 * \return
		 */
		static std::vector<Vector2> sutherlandHodgmentPolygonClipping(const std::vector<Vector2>& polygon, const std::vector<Vector2>& clipRegion);
	};

	struct ST_API RaycastHit
	{
		Vector2 point;
		Vector2 normal;
		real distance = 0.0f;
		bool hit = false;
	};

	struct ST_API PointsResult
	{
		PointsResult(const Vector2& p1, const Vector2& p2) : points{ p1, p2 } {}
		std::array<Vector2, 2> points;
	};



	class ST_API Algorithm2D
	{
	public:

		static RaycastHit raycastCircle(const Vector2& p, const Vector2& dir, const Vector2& center, const real& radius);

		static RaycastHit raycastEllipse(const Vector2& p, const Vector2& dir, const Vector2& center, const real& a, const real& b);

		static RaycastHit raycastCapsule(const Vector2& p, const Vector2& dir, const Vector2& center, const real& halfWidth, const real& halfHeight);

		static RaycastHit raycastSegment(const Vector2& p, const Vector2& dir, const Vector2& a, const Vector2& b);

		static RaycastHit raycastPolygon(const Vector2& p, const Vector2& dir, const std::vector<Vector2>& vertices);

		static Vector2 axialSymmetry(const Vector2& center, const Vector2& dir, const Vector2& point);


		static bool checkCollinear(const Vector2& a, const Vector2& b, const Vector2& c);

		static bool checkPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c);

		static bool fuzzyCheckPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c, const real& epsilon = Constant::GeometryEpsilon);
		static bool fuzzyCheckCollinear(const Vector2& a, const Vector2& b, const Vector2& c);

		static bool lineSegmentIntersection(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d, Vector2& result);

		static Vector2 lineIntersection(const Vector2& p1, const Vector2& p2, const Vector2& q1, const Vector2& q2);

		static bool checkConvexPolygon(const std::vector<Vector2>& vertices);

		static std::vector<Vector2> grahamScan(const std::vector<Vector2>& vertices);

		static Vector2 shortestLengthPointOfEllipse(const real& a, const real& b, const Vector2& p, const real& epsilon = Constant::GeometryEpsilon);

		static Vector2 triangleCentroid(const Vector2& a1, const Vector2& a2, const Vector2& a3);

		static real triangleArea(const Vector2& a1, const Vector2& a2, const Vector2& a3);

		static Vector2 computeCenter(const std::vector<Vector2>& vertices);
		static Vector2 computeCenter(const std::list<Vector2>& vertices);
		static Vector2 computeCenter(const Vector2* vertices, const uint32_t& count);

		static PointsResult shortestLengthLineSegmentEllipse(const real& a, const real& b, const Vector2& p1, const Vector2& p2);

		static bool raycast(const Vector2& p, const Vector2& dir, const Vector2& a, const Vector2& b, Vector2& result);
		static bool raycastAABB(const Vector2& p, const Vector2& dir, const Vector2& topLeft, const Vector2& bottomRight, Vector2& enter, Vector2& exit);
		inline static bool checkPointInsideAABB(const Vector2& pos, const Vector2& topLeft, const Vector2& bottomRight);
		static bool checkPointOnAABB(const Vector2& p, const Vector2& topLeft, const Vector2& bottomRight);

		static Vector2 rotateAround(const Vector2& p, const Vector2& center, const real& radians);


		static Vector2 computeEllipseProjectionPoint(const real& a, const real& b, const Vector2& direction);
		static Vector2 computeCapsuleProjectionPoint(const real& halfLength, const real& radius, const Vector2& direction);
		static Vector2 computeSectorProjectionPoint(const real& startRadian, const real& spanRadian, const real& radius, const Vector2& direction);
		static bool checkOriginInTriangle(const Vector2& a, const Vector2& b, const Vector2& c);
		static bool checkPointsOnSameSide(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refPoint, const Vector2 targetPoint);

		static Vector2 lineSegmentNormal(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refDirection);

		static Vector2 pointToLineSegment(const Vector2& a, const Vector2& b, const Vector2& p);

		static Vector2 pointToSegment(const Vector2& a, const Vector2& b, const Vector2& p, bool clamp = true);

		static real pointToSegmentWeight(const Vector2& a, const Vector2& b, const Vector2& p, bool clamp = true);

		static Vector2 rayRayIntersectionUnsafe(const Vector2& p1, const Vector2& dir1, const Vector2& p2, const Vector2& dir2);
	};
}
