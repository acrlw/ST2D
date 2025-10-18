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

	struct ST_API SegmentClosestResult
	{
		Vector2 P, Q;
		real t = 0;
		real u = 0;
	};


	namespace Algorithm2D
	{
		ST_API real minIntervalSeparation(const real& tMin, const real& tMax, const real& uMin, const real& uMax);

		ST_API RaycastHit raycastCircle(const Vector2& p, const Vector2& dir, const Vector2& center, const real& radius);

		ST_API RaycastHit raycastEllipse(const Vector2& p, const Vector2& dir, const Vector2& center, const real& a, const real& b);

		ST_API RaycastHit raycastCapsule(const Vector2& p, const Vector2& dir, const Vector2& center, const real& halfWidth, const real& halfHeight);

		ST_API RaycastHit raycastSegment(const Vector2& p, const Vector2& dir, const Vector2& a, const Vector2& b);

		ST_API RaycastHit raycastPolygon(const Vector2& p, const Vector2& dir, const std::vector<Vector2>& vertices);

		ST_API Vector2 axialSymmetry(const Vector2& center, const Vector2& dir, const Vector2& point);


		ST_API bool checkCollinear(const Vector2& a, const Vector2& b, const Vector2& c);

		ST_API bool checkPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c);

		ST_API bool fuzzyCheckPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c, const real& epsilon = Constant::GeometryEpsilon);
		ST_API bool fuzzyCheckCollinear(const Vector2& a, const Vector2& b, const Vector2& c);

		ST_API bool lineSegmentIntersection(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d, Vector2& result);

		ST_API Vector2 lineIntersection(const Vector2& p1, const Vector2& p2, const Vector2& q1, const Vector2& q2);

		ST_API bool checkConvexPolygon(const std::vector<Vector2>& vertices);

		ST_API std::vector<Vector2> grahamScan(const std::vector<Vector2>& vertices);

		ST_API Vector2 shortestLengthPointOfEllipse(const real& a, const real& b, const Vector2& p, const real& epsilon = Constant::GeometryEpsilon);

		ST_API Vector2 triangleCentroid(const Vector2& a1, const Vector2& a2, const Vector2& a3);

		ST_API real triangleArea(const Vector2& a1, const Vector2& a2, const Vector2& a3);

		ST_API Vector2 computeCenter(const std::vector<Vector2>& vertices);
		ST_API Vector2 computeCenter(const std::list<Vector2>& vertices);
		ST_API Vector2 computeCenter(const Vector2* vertices, const uint32_t& count);

		ST_API PointsResult shortestLengthLineSegmentEllipse(const real& a, const real& b, const Vector2& p1, const Vector2& p2);

		ST_API bool raycast(const Vector2& p, const Vector2& dir, const Vector2& a, const Vector2& b, Vector2& result);
		ST_API bool raycastAABB(const Vector2& p, const Vector2& dir, const Vector2& topLeft, const Vector2& bottomRight, Vector2& enter, Vector2& exit);
		ST_API bool checkPointInsideAABB(const Vector2& pos, const Vector2& topLeft, const Vector2& bottomRight);
		ST_API bool checkPointOnAABB(const Vector2& p, const Vector2& topLeft, const Vector2& bottomRight);

		ST_API Vector2 rotateAround(const Vector2& p, const Vector2& center, const real& radians);


		ST_API Vector2 computeEllipseProjectionPoint(const real& a, const real& b, const Vector2& direction);
		ST_API Vector2 computeCapsuleProjectionPoint(const real& halfLength, const real& radius, const Vector2& direction);
		ST_API Vector2 computeSectorProjectionPoint(const real& startRadian, const real& spanRadian, const real& radius, const Vector2& direction);
		ST_API bool checkOriginInTriangle(const Vector2& a, const Vector2& b, const Vector2& c);
		ST_API bool checkPointsOnSameSide(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refPoint, const Vector2 targetPoint);

		ST_API Vector2 lineSegmentNormal(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refDirection);

		ST_API Vector2 pointToLineSegment(const Vector2& a, const Vector2& b, const Vector2& p);

		ST_API Vector2 pointToSegment(const Vector2& a, const Vector2& b, const Vector2& p, bool clamp = true);

		ST_API real pointToSegmentWeight(const Vector2& a, const Vector2& b, const Vector2& p, bool clamp = true);

		ST_API Vector2 rayRayIntersectionUnsafe(const Vector2& p1, const Vector2& dir1, const Vector2& p2, const Vector2& dir2);
		ST_API SegmentClosestResult segmentClosestPoint(const Vector2& A1, const Vector2& A2, const Vector2& B1, const Vector2& B2);

	};
}
