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

		/**
		 * \brief Check if point a,b,c are collinear using triangle area method
		 * \param a point a
		 * \param b point b
		 * \param c point c
		 * \return
		 */
		static bool checkCollinear(const Vector2& a, const Vector2& b, const Vector2& c);
		/**
		 * \brief Check if point c is on line segment ab using line projection and set-union method
		 * \param a end of segment a
		 * \param b end of segment b
		 * \param c point c
		 * \return
		 */
		static bool checkPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c);
		/**
		 * \brief Check if point c is on line segment ab, given a,b,c is already collinear by calculating cross product
		 * \param a
		 * \param b
		 * \param c
		 * \param epsilon
		 * \return
		 */
		static bool fuzzyCheckPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c, const real& epsilon = Constant::GeometryEpsilon);
		static bool fuzzyCheckCollinear(const Vector2& a, const Vector2& b, const Vector2& c);
		/**
		 * \brief Calculate intersected point between line ab and line cd.\n
		 * Notices: overlapping is NOT considered as a kind of intersection situation in this function
		 * \param a
		 * \param b
		 * \param c
		 * \param d
		 * \return if there is a actual intersected point.
		 */
		static bool lineSegmentIntersection(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d, Vector2& result);
		/**
		 * \brief line intersection
		 * \param p1
		 * \param p2
		 * \param q1
		 * \param q2
		 * \return
		 */
		static Vector2 lineIntersection(const Vector2& p1, const Vector2& p2, const Vector2& q1, const Vector2& q2);
		/**
		 * \brief Check if a polygon is convex
		 * \param vertices
		 * \return
		 */
		static bool checkConvexPolygon(const std::vector<Vector2>& vertices);
		/**
		 * \brief Convex hull algorithm: Graham Scan. Given a series of points, find the convex polygon that can contains all of these points.
		 * \param vertices
		 * \return
		 */
		static std::vector<Vector2> grahamScan(const std::vector<Vector2>& vertices);
		/**
		 * \brief Calculate point on ellipse that is the shortest length to point p(aka projection point).
		 * \param a
		 * \param b
		 * \param p
		 * \param epsilon
		 * \return
		 */
		static Vector2 shortestLengthPointOfEllipse(const real& a, const real& b, const Vector2& p, const real& epsilon = Constant::GeometryEpsilon);
		/**
		 * \brief Calculate the centroid of triangle.
		 * \param a1
		 * \param a2
		 * \param a3
		 * \return
		 */
		static Vector2 triangleCentroid(const Vector2& a1, const Vector2& a2, const Vector2& a3);
		/**
		 * \brief Calculate the area of triangle use cross product.
		 * \param a1
		 * \param a2
		 * \param a3
		 * \return
		 */
		static real triangleArea(const Vector2& a1, const Vector2& a2, const Vector2& a3);
		/**
		 * \brief Calculate mass center of 'convex' polygon
		 * \param vertices
		 * \return
		 */
		static Vector2 computeCenter(const std::vector<Vector2>& vertices);
		static Vector2 computeCenter(const std::list<Vector2>& vertices);
		static Vector2 computeCenter(Vector2* vertices, const uint32_t& count);
		/**
		 * \brief Calculate two points on line segment and ellipse respectively. The length of two points is the shortest distance of line segment and ellipse
		 * \param a major axis a
		 * \param b minor axis b
		 * \param p1 line segment point 1
		 * \param p2 line segment point 2
		 * \return
		 */
		static PointsResult shortestLengthLineSegmentEllipse(const real& a, const real& b, const Vector2& p1, const Vector2& p2);
		/**
		 * \brief Calculate point on line segment ab, if point 'p' can cast ray in 'dir' direction on line segment ab. \n
		 * Algorithm from wikipedia Line-line intersection.
		 * \param p ray start point
		 * \param dir ray direction
		 * \param a line segment point a
		 * \param b line segment point b
		 * \return
		 */
		static bool raycast(const Vector2& p, const Vector2& dir, const Vector2& a, const Vector2& b, Vector2& result);
		static bool raycastAABB(const Vector2& p, const Vector2& dir, const Vector2& topLeft, const Vector2& bottomRight, Vector2& enter, Vector2& exit);
		inline static bool checkPointInsideAABB(const Vector2& pos, const Vector2& topLeft, const Vector2& bottomRight);
		static bool checkPointOnAABB(const Vector2& p, const Vector2& topLeft, const Vector2& bottomRight);
		/**
		 * \brief Rotate point 'p' around point 'center' by 'radians' radians
		 * \param p
		 * \param center
		 * \param angle
		 * \return
		 */
		static Vector2 rotateAround(const Vector2& p, const Vector2& center, const real& radians);

		/**
		 * \brief Calculate the projection axis of ellipse in user-define direction.
		 * \param a
		 * \param b
		 * \param direction
		 * \return the maximum point in ellipse
		 */
		static Vector2 computeEllipseProjectionPoint(const real& a, const real& b, const Vector2& direction);
		static Vector2 computeCapsuleProjectionPoint(const real& halfWidth, const real& halfHeight, const Vector2& direction);
		static Vector2 computeSectorProjectionPoint(const real& startRadian, const real& spanRadian, const real& radius, const Vector2& direction);
		static bool checkOriginInTriangle(const Vector2& a, const Vector2& b, const Vector2& c);
		static bool checkPointsOnSameSide(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refPoint, const Vector2 targetPoint);
		/**
		 * \brief calculate normal of line segment with direction of refDirection
		 * \param edgePoint1
		 * \param edgePoint2
		 * \param refDirection
		 * \return
		 */
		static Vector2 lineSegmentNormal(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refDirection);
		/**
		 * \brief calculate point on line segment ab that is the shortest length to point p
		 * \param a point a
		 * \param b point b
		 * \param p target point
		 * \return
		 */
		static Vector2 pointToLineSegment(const Vector2& a, const Vector2& b, const Vector2& p);
		/**
		 * \brief ray-ray intersection with no exception check, be sure two rays must intersect.
		 * \param p1
		 * \param dir1
		 * \param p2
		 * \param dir2
		 * \return
		 */
		static Vector2 rayRayIntersectionUnsafe(const Vector2& p1, const Vector2& dir1, const Vector2& p2, const Vector2& dir2);
	};
}
