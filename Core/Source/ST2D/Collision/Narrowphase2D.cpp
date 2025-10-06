#include "Narrowphase2D.h"

#include <utility>

#include "Simplex.h"
#include "ST2D/Log.h"
#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Shape/Capsule.h"
#include "ST2D/Shape/Ellipse.h"
#include "ST2D/Shape/Polygon.h"
#include "ST2D/Shape/Circle.h"
#include "ST2D/Shape/Segment.h"
namespace ST
{
	Simplex2D Narrowphase2D::gjk(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
		const Shape* shapeB, const uint32_t& iteration, const Vector2& initialDirection)
	{
		CORE_ASSERT(shapeA != nullptr && shapeB != nullptr, "Shape is nullptr.");

		Simplex2D simplex;
		Vector2 direction = transformB.position - transformA.position;

		if (direction.fuzzyEqual({ 0, 0 }))
			direction.set(1, 1);

		if (!initialDirection.fuzzyEqual({0, 0}))
			direction = initialDirection;

		simplex.m[0] = support(transformA, shapeA, transformB, shapeB, direction);
		simplex.m[1] = support(transformA, shapeA, transformB, shapeB, direction.negative());
		simplex.count = 2;

		//check 1d simplex(line segment) across origin
		//if it is, just return isCollide=true
		if (Algorithm2D::checkPointOnSegment(simplex.m[0].p, simplex.m[1].p, { 0,0 }))
		{
			simplex.isContainOrigin = true;
			return simplex;
		}

		for (uint32_t i = 0; i < iteration; ++i)
		{
			direction = getDirection(simplex.m[0].p, simplex.m[1].p, true);
			simplex.m[2] = support(transformA, shapeA, transformB, shapeB, direction);
			simplex.count = 3;

			// check repeated vertex
			if (simplex.m[2].p == simplex.m[0].p || simplex.m[2].p == simplex.m[1].p)
			{
				simplex.count = 2;
				break;
			}

			// check if new vertex passes origin
			if (Vector2::dot(simplex.m[2].p, direction) <= 0)
				break;

			// check simplex contains origin by using voronoi region with new vertex
			// if contains, then return isCollide=true
			// if not, check which two vertices to keep

			simplex = solveSimplex(simplex);
			if (simplex.isContainOrigin)
				break;

		}
		return simplex;
	}

	Epa2DResult Narrowphase2D::epa(const Simplex2D& simplex, const Transform& transformA, const Shape* shapeA,
		const Transform& transformB, const Shape* shapeB, const uint32_t& iteration, const real& epsilon)
	{
		Epa2DResult result;
		result.simplex.isContainOrigin = simplex.isContainOrigin;
		Polytope polytope = initializePolytope(simplex);
		result.polytope = polytope; //[debug]
		if (polytope.nFace == 0)
			return result;

		result.state = EpaState::MaxIteration;

		// get the struct data size of polytope [debug]
		// auto size = sizeof(Polytope);


		//3. EPA loop

		for (uint32_t i = 0; i < iteration; ++i)
		{
			// 3.1 find face closest to origin
			const auto& faceIndex = polytope.faces[polytope.closestFaceIdx];
			const auto& m0 = polytope.vertices[faceIndex[0]];
			const auto& m1 = polytope.vertices[faceIndex[1]];
			const Vector2 direction = getDirection(m0.p, m1.p, false);
			const MinkowskiDiff m2 = support(transformA, shapeA, transformB, shapeB, direction);

			// check if new vertex m2 is already in polytope
			bool duplicated = false;
			for (uint32_t j = 0; j < polytope.nVertex; ++j)
			{
				if (m2.p.fuzzyEqual(polytope.vertices[j].p, epsilon))
				{
					duplicated = true;
					break;
				}
			}
			if (duplicated)
			{
				result.state = EpaState::Valid;
				break;
			}

			// check if m2 and m0 are close enough in the direction
			const real dotm0dir = Vector2::dot(m0.p, direction);
			const real dotm2dir = Vector2::dot(m2.p, direction);
			const real gain = dotm2dir - dotm0dir;
			if (gain * gain < epsilon * epsilon * direction.square())
			{
				result.state = EpaState::Valid;
				break;
			}

			// check if nVertex exceeds max
			if (polytope.nFace >= MaxPolytopeFaces - 2)
			{
				result.state = EpaState::MaxFaces;
				break;
			}

			// add new vertex m2 to polytope vertices
			const uint32_t m2VertexIdx = polytope.nVertex;
			polytope.vertices[polytope.nVertex] = m2;
			polytope.nVertex++;

			// calculate distance from origin to face m0-m2, m1-m2

			const real distm0m2 = Algorithm2D::pointToSegment(m0.p, m2.p, { 0, 0 }).square();
			const real distm1m2 = Algorithm2D::pointToSegment(m1.p, m2.p, { 0, 0 }).square();

			// construct m0-m2
			// 1. add new face m0-m2
			// 2. activate face m0-m2
			// 3. calculate distance from origin to face m0-m2
			// 4. add distance to polytope distances
			polytope.faces[polytope.nFace] = { faceIndex[0], m2VertexIdx };
			polytope.activeFaces[polytope.nFace] = 1;
			polytope.distances[polytope.nFace] = distm0m2;
			polytope.nFace++;

			// construct m1-m2
			polytope.faces[polytope.nFace] = { m2VertexIdx, faceIndex[1] };
			polytope.activeFaces[polytope.nFace] = 1;
			polytope.distances[polytope.nFace] = distm1m2;
			polytope.nFace++;

			// deactivate face m0-m1
			polytope.activeFaces[polytope.closestFaceIdx] = 0;

			// 3.2 find face closest to origin for next iteration
			real minDist = Constant::Max;
			uint32_t minDistIdx = 0;
			for (uint32_t j = 0; j < polytope.nFace; ++j)
			{
				if (polytope.activeFaces[j] == 1 && minDist > polytope.distances[j])
				{
					minDist = polytope.distances[j];
					minDistIdx = j;
				}
			}
			// 3.3 update closest face index
			polytope.closestFaceIdx = minDistIdx;
		}

		const auto& closestFace = polytope.faces[polytope.closestFaceIdx];
		result.simplex.m[0] = polytope.vertices[closestFace[0]];
		result.simplex.m[1] = polytope.vertices[closestFace[1]];
		result.simplex.count = 2;
		const real t = Math::clamp(originToSegmentWeight(result.simplex.m[0].p, result.simplex.m[1].p), 0, 1);

		result.simplex.m[2].p = t * result.simplex.m[0].p + (1 - t) * result.simplex.m[1].p;
		// get witness points on both shapes, save on simplex.m[2]
		result.simplex.m[2].v[0].v = t * result.simplex.m[0].v[0].v + (1 - t) * result.simplex.m[1].v[0].v;
		result.simplex.m[2].v[1].v = t * result.simplex.m[0].v[1].v + (1 - t) * result.simplex.m[1].v[1].v;

		result.penetration = result.simplex.m[2].p.norm();
		result.normal = result.simplex.m[2].p / result.penetration;

		//ensure normal always point from A to B
		if (Vector2::dot(result.normal, transformB.position - transformA.position) < 0)
		{
			result.normal = result.normal.negative();
			result.penetration = -result.penetration;
		}


		result.polytope = polytope; //[debug]
		return result;
	}

	Distance2DResult Narrowphase2D::distance(const Transform& transformA, const Shape* shapeA,
		const Transform& transformB, const Shape* shapeB, const uint32_t& iteration, const real& epsilon)
	{
		Distance2DResult result;
		Vector2 direction = transformB.position - transformA.position;
		if (direction.fuzzyEqual({ 0, 0 }))
			direction.set(1, 1);

		// get minkowski difference from direction to build initial simplex1(point)
		result.simplex.m[0] = support(transformA, shapeA, transformB, shapeB, direction);
		result.simplex.count++;
		direction = result.simplex.m[0].p.negative();
		uint32_t i = 0;

		for (;i < iteration; ++i)
		{
			MinkowskiDiff newM = support(transformA, shapeA, transformB, shapeB, direction);
			bool isExists1 = result.simplex.count == 1 && result.simplex.m[0].p == newM.p;
			bool isExists2 = result.simplex.count == 2 &&
				result.simplex.m[0].p == newM.p ||
				result.simplex.m[1].p == newM.p;

			// check if repeated
			if (isExists1 || isExists2)
				break;

			const real dotm0dir = Vector2::dot(result.simplex.m[0].p, direction);
			const real dotm2dir = Vector2::dot(newM.p, direction);
			const real gain = dotm2dir - dotm0dir;
			// check if close enough
			if (gain * gain < epsilon * epsilon * direction.square())
				break;

			// add newM to simplex
			uint32_t count = result.simplex.count;
			result.simplex.m[count] = newM;
			result.simplex.count++;

			SolveSimplexResult solveResult;
			switch (result.simplex.count)
			{
			case 1:
				solveResult = solveSimplex1(result.simplex);
				break;
			case 2:
				solveResult = solveSimplex2(result.simplex);
				break;
			case 3:
				solveResult = solveSimplex3(result.simplex);
				break;
			default:
				assert(false && "Invalid simplex for GJK distance check");
				return result;
			}
			// the simplex of solution is always a segment(count=2)

			result.simplex = solveResult.simplex;

			if (result.simplex.isContainOrigin)
				return result;

			direction = solveResult.direction;
			

		}

		if (result.simplex.count == 1)
		{
			result.closestPoints[0] = result.simplex.m[0].v[0].v;
			result.closestPoints[1] = result.simplex.m[0].v[1].v;
			result.distance = (result.closestPoints[0] - result.closestPoints[1]).norm();
		}
		else if (result.simplex.count == 2)
		{
			const real t = Math::clamp(originToSegmentWeight(result.simplex.m[0].p, result.simplex.m[1].p), 0, 1);
			
			result.closestPoints[0] = t * result.simplex.m[0].v[0].v + (1 - t) * result.simplex.m[1].v[0].v;
			result.closestPoints[1] = t * result.simplex.m[0].v[1].v + (1 - t) * result.simplex.m[1].v[1].v;
			result.distance = (result.closestPoints[0] - result.closestPoints[1]).norm();
		}

		return result;

	}

	Distance2DResult Narrowphase2D::distanceRound(const Transform& transformA, const Shape* shapeA,
	                                              const Transform& transformB, const Shape* shapeB, const real& radius1, const real& radius2,
	                                              const uint32_t& iteration, const real& epsilon)
	{
		Distance2DResult result = distance(transformA, shapeA, transformB, shapeB, iteration, epsilon);
		const Vector2 normal = (result.closestPoints[1] - result.closestPoints[0]).normal();
		result.closestPoints[0] += radius1 * normal;
		result.closestPoints[1] -= radius2 * normal;

		return result;
	}

	Polytope Narrowphase2D::initializePolytope(const Simplex2D& simplex)
	{
		Polytope polytope;
		polytope.nVertex = simplex.count;
		//0. load vertices from simplex
		for (uint32_t i = 0; i < simplex.count; ++i)
			polytope.vertices[i] = simplex.m[i];

		//1. build faces from simplex
		if (simplex.count == 2)
		{
			// two points build a line segment
			polytope.faces[0] = { 0, 1 };
			polytope.nFace = 1;
		}
		else if (simplex.count == 3)
		{
			// three points build a triangle
			polytope.faces[0] = { 0, 1 };
			polytope.faces[1] = { 1, 2 };
			polytope.faces[2] = { 2, 0 };
			polytope.nFace = 3;
		}
		else
		{
			assert(false && "Invalid simplex for EPA.");
			polytope.nFace = 0;
			return polytope;
		}

		//2. active all faces and calculate distances from origin to each face

		real minDist = Constant::Max;
		uint32_t minDistIdx = 0;
		for (uint32_t i = 0; i < polytope.nFace; ++i)
		{
			polytope.activeFaces[i] = 1;
			const auto& face = polytope.faces[i];
			const auto& m0 = polytope.vertices[face[0]];
			const auto& m1 = polytope.vertices[face[1]];
			polytope.distances[i] = Algorithm2D::pointToSegment(m0.p, m1.p, { 0, 0 }).square();
			if (minDist > polytope.distances[i])
			{
				minDist = polytope.distances[i];
				minDistIdx = i;
			}
		}
		polytope.closestFaceIdx = minDistIdx;

		return polytope;
	}

	MinkowskiDiff Narrowphase2D::support(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
		const Shape* shapeB, const Vector2& direction)
	{
		MinkowskiDiff result;
		result.v[0] = findFurthestVertex(transformA, shapeA, direction);
		result.v[1] = findFurthestVertex(transformB, shapeB, direction.negative());
		result.p = result.v[0].v - result.v[1].v;
		return result;
	}


	FurthestVertex Narrowphase2D::findFurthestVertex(const Transform& transform, const Shape* shape, const Vector2& direction)
	{
		FurthestVertex result;
		Complex rot(-transform.rotation);
		Vector2 rot_dir = rot.multiply(direction);
		switch (shape->type())
		{
		case ShapeType::Polygon:
		{
			auto polygon = static_cast<const Polygon*>(shape);
			result = findFurthestVertexHillClimbing(polygon->vertices(), rot_dir);
			break;
		}
		case ShapeType::Circle:
		{
			auto circle = static_cast<const Circle*>(shape);
			result.v = direction.normal() * circle->radius() + transform.position;
			result.i = INT32_MAX;
			return result;
		}
		case ShapeType::Ellipse:
		{
			auto ellipse = static_cast<const Ellipse*>(shape);
			result.v = Algorithm2D::computeEllipseProjectionPoint(ellipse->A(), ellipse->B(), rot_dir);
			result.i = INT32_MAX;
			break;
		}
		case ShapeType::Segment:
		{
			auto edge = static_cast<const Segment*>(shape);
			const real dot1 = Vector2::dot(edge->startPoint(), rot_dir);
			const real dot2 = Vector2::dot(edge->endPoint(), rot_dir);
			result.v = dot1 > dot2 ? edge->startPoint() : edge->endPoint();
			result.i = dot1 > dot2 ? 0 : 1;
			break;
		}
		case ShapeType::Capsule:
		{
			auto capsule = static_cast<const Capsule*>(shape);
			result.v = Algorithm2D::computeCapsuleProjectionPoint(
				capsule->halfWidth(), capsule->halfHeight(), rot_dir);
			if (result.v.x > 0 && result.v.y > 0)
				result.i = 0;
			else if (result.v.x < 0 && result.v.y > 0)
				result.i = 1;
			else if (result.v.x < 0 && result.v.y < 0)
				result.i = 2;
			else if (result.v.x > 0 && result.v.y < 0)
				result.i = 3;
			break;
		}
		}
		rot.conjugate();
		result.v = rot.multiply(result.v);
		result.v += transform.position;
		return result;
	}

	FurthestVertex Narrowphase2D::findFurthestVertex(const std::vector<Vector2>& vertices,
		const Vector2& direction)
	{
		real max = Constant::NegativeMin;
		Vector2 target;
		int32_t index = INT_MAX;
		for (int i = 0; i < vertices.size(); i++)
		{
			real result = Vector2::dot(vertices[i], direction);
			if (max < result)
			{
				max = result;
				target = vertices[i];
				index = i;
			}
		}
		return FurthestVertex{ target, index };
	}

	FurthestVertex Narrowphase2D::findFurthestVertexHillClimbing(const std::vector<Vector2>& vertices,
		const Vector2& direction, int32_t startIndex)
	{
		int count = vertices.size();
		if (count == 0)
			return {};

		int32_t currentIndex = startIndex >= 0 && startIndex < count ? startIndex : 0;
		for (int i = 0; i < vertices.size(); i++)
		{
			real currentDot = Vector2::dot(vertices[currentIndex], direction);
			const int32_t prevIndex = (currentIndex - 1 + count) % count;
			const int32_t nextIndex = (currentIndex + 1) % count;
			const real dotPrev = Vector2::dot(vertices[prevIndex], direction);
			const real dotNext = Vector2::dot(vertices[nextIndex], direction);
			if (currentDot < dotPrev)
				currentIndex = prevIndex;
			else if (currentDot < dotNext)
				currentIndex = nextIndex;
			else
				break;
		}
		return FurthestVertex{ vertices[currentIndex], currentIndex };
	}

	Vector2 Narrowphase2D::getDirection(const Vector2 p1, const Vector2 p2, bool pointToOrigin)
	{
		const Vector2 ao = p1.negative();
		const Vector2 ab = p2 - p1;
		Vector2 orthoAB = ab.ortho();
		real dot = Vector2::dot(ao, orthoAB);
		
		//if (dot < 0 && pointToOrigin) || (
		//	dot > 0 && !pointToOrigin))
		//	orthoAB.negate();

		const float c = (static_cast<float>(pointToOrigin) * 2.0f) - 1.0f;
		const float scale = std::copysign(1.0f, dot * c);

		return orthoAB * scale;
	}

	ContactResult Narrowphase2D::generateContacts(const Epa2DResult& epaResult, const Transform& transformA,
		const Shape* shapeA, const Transform& transformB, const Shape* shapeB)
	{
		ContactResult result;
		if (shapeA->type() == ShapeType::Polygon && shapeB->type() == ShapeType::Polygon)
		{
			result = clipPolygonPolygon(epaResult, transformA, shapeA, transformB, shapeB);
		}
		else if (shapeA->type() == ShapeType::Capsule && shapeB->type() == ShapeType::Polygon)
		{
			result = clipCapsulePolygon(epaResult, transformA, shapeA, transformB, shapeB);
		}
		else if (shapeA->type() == ShapeType::Polygon && shapeB->type() == ShapeType::Capsule)
		{
			result = clipCapsulePolygon(epaResult, transformB, shapeB, transformA, shapeA, true);
		}
		else if (shapeA->type() == ShapeType::Polygon && shapeB->type() == ShapeType::Segment)
		{
			result = clipPolygonSegment(epaResult, transformA, shapeA, transformB, shapeB);
		}
		else if (shapeA->type() == ShapeType::Segment && shapeB->type() == ShapeType::Polygon)
		{
			result = clipPolygonSegment(epaResult, transformB, shapeB, transformA, shapeA, true);
		}
		else if (shapeA->type() == ShapeType::Capsule && shapeB->type() == ShapeType::Segment)
		{
			result = clipCapsuleSegment(epaResult, transformA, shapeA, transformB, shapeB);
		}
		else if (shapeA->type() == ShapeType::Segment && shapeB->type() == ShapeType::Capsule)
		{
			result = clipCapsuleSegment(epaResult, transformB, shapeB, transformA, shapeA, true);
		}
		else if (shapeA->type() == ShapeType::Capsule && shapeB->type() == ShapeType::Capsule)
		{
			result = clipCapsuleCapsule(epaResult, transformA, shapeA, transformB, shapeB);
		}
		else
		{
			//just use minkowski difference 2
			result.count = 1;
			result.normal = epaResult.normal;
			result.pA[0] = epaResult.simplex.m[2].v[0].v;
			result.pB[0] = epaResult.simplex.m[2].v[1].v;
			result.penetration[0] = epaResult.penetration;
		}



		return result;
	}

	ClipEdge Narrowphase2D::clipEdges(const std::array<Vector2, 4>& edge, const Vector2& clipNormal)
	{
		ClipEdge result;
		// A, B : refEdge
		// C, D : incEdge
		// n: collision normal
		// v: incEdge normal
		// PA = C + u * v = A + t * n
		// PB = C + w * v = B + s * n
		// t * n - u * v = C - A
		// s * n - w * v = C - B
		// solve for t, u, s, w and check sign
		const Vector2& A = edge[0];
		const Vector2& B = edge[1];
		const Vector2& C = edge[2];
		const Vector2& D = edge[3];
		const real normCD = (D - C).norm();
		Vector2 v = (edge[3] - edge[2]).normal();
		Vector2 n = clipNormal;
		real det = v.x * n.y - n.x * v.y;
		CORE_ASSERT(!realEqual(det, 0), "Invalid edge");

		real det1 = v.x * (C.y - A.y) - v.y * (C.x - A.x);
		real det2 = n.x * (C.y - A.y) - n.y * (C.x - A.x);
		real det3 = v.x * (C.y - B.y) - v.y * (C.x - B.x);
		real det4 = n.x * (C.y - B.y) - n.y * (C.x - B.x);

		real t = det1 / det;
		real u = det2 / det;
		real s = det3 / det;
		real w = det4 / det;

		Vector2 pA = A + t * n; // pA = A + t * n = C + t * v
		Vector2 pB = B + s * n; // pB = B + s * n = C + w * v

		//pA = h * C + (1 - h) * D
		//pB = r * C + (1 - r) * D
		real h = 1 - (u / normCD);
		real r = 1 - (w / normCD);

		real tC = Algorithm2D::pointToSegmentWeight(A, B, C, false);
		real tD = Algorithm2D::pointToSegmentWeight(A, B, D, false);

		Vector2 pC = tC * A + (1 - tC) * B;
		Vector2 pD = tD * A + (1 - tD) * B;


		Vector2 AC = C - A;
		Vector2 AD = D - A;

		// clip incident edge first, then get reference edge projection point

		//check pC in clip region and located in AB
		if (Vector2::dot(AC, n) >= 0 && tC >= 0 && tC <= 1 && result.count < 2)
		{
			result.refEdge[result.count] = pC;
			result.incEdge[result.count] = C;
			result.count++;
		}
		//check pD in clip region and located in AB
		if (Vector2::dot(AD, n) >= 0 && tD >= 0 && tD <= 1 && result.count < 2)
		{
			result.refEdge[result.count] = pD;
			result.incEdge[result.count] = D;
			result.count++;
		}
		//check pA in clip region and located in CD
		if (t >= 0 && h >= 0 && h <= 1 && result.count < 2)
		{
			result.refEdge[result.count] = A;
			result.incEdge[result.count] = pA;
			result.count++;
		}
		//check pB in clip region and located in CD
		if (s >= 0 && r >= 0 && r <= 1 && result.count < 2)
		{
			result.refEdge[result.count] = B;
			result.incEdge[result.count] = pB;
			result.count++;
		}
		return result;
	}

	ContactResult Narrowphase2D::clipPolygonPolygon(const Epa2DResult& epaResult, const Transform& transformA,
	                                                const Shape* shapeA, const Transform& transformB, const Shape* shapeB)
	{
		ContactResult result;
		bool sameA = epaResult.simplex.m[0].v[0].i == epaResult.simplex.m[1].v[0].i;
		bool sameB = epaResult.simplex.m[0].v[1].i == epaResult.simplex.m[1].v[1].i;
		bool swap = sameA; // default: refEdge from A, incEdge from B

		Vector2 clipNormal = epaResult.normal; // default normal is: A -> B, means B + normal * penetration can be separated from A
		if (!sameA)
		{
			//refEdge from A, clipNormal need negate
			clipNormal.negate();
		}

		std::array<Vector2, 4> edge;
		auto polygonA = static_cast<const Polygon*>(shapeA);
		auto polygonB = static_cast<const Polygon*>(shapeB);
		if (sameA && !sameB)
		{
			edge[0] = epaResult.simplex.m[0].v[1].v;
			edge[1] = epaResult.simplex.m[1].v[1].v;
			// get incident edge
			int32_t idx = epaResult.simplex.m[0].v[0].i;
			Vector2 currVertex = epaResult.simplex.m[0].v[0].v;
			int32_t prevIdx = (idx - 1 + polygonA->vertices().size()) % polygonA->vertices().size();
			int32_t nextIdx = (idx + 1) % polygonA->vertices().size();
			Vector2 prevVertex = transformA.translatePoint(polygonA->vertices()[prevIdx]);
			Vector2 nextVertex = transformA.translatePoint(polygonA->vertices()[nextIdx]);
			Vector2 vCurrToPrev = prevVertex - currVertex;
			Vector2 vCurrToNext = nextVertex - currVertex;
			real dot1 = Math::abs(Vector2::dot(vCurrToPrev, epaResult.normal));
			real dot2 = Math::abs(Vector2::dot(vCurrToNext, epaResult.normal));
			edge[2] = currVertex;
			edge[3] = dot1 < dot2 ? prevVertex : nextVertex;
		}
		else if (!sameA && sameB)
		{
			edge[0] = epaResult.simplex.m[0].v[0].v;
			edge[1] = epaResult.simplex.m[1].v[0].v;
			// get incident edge
			int32_t idx = epaResult.simplex.m[0].v[1].i;
			Vector2 currVertex = epaResult.simplex.m[0].v[1].v;
			int32_t prevIdx = (idx - 1 + polygonB->vertices().size()) % polygonB->vertices().size();
			int32_t nextIdx = (idx + 1) % polygonB->vertices().size();
			Vector2 prevVertex = transformB.translatePoint(polygonB->vertices()[prevIdx]);
			Vector2 nextVertex = transformB.translatePoint(polygonB->vertices()[nextIdx]);
			Vector2 vCurrToPrev = prevVertex - currVertex;
			Vector2 vCurrToNext = nextVertex - currVertex;
			real dot1 = Math::abs(Vector2::dot(vCurrToPrev, epaResult.normal));
			real dot2 = Math::abs(Vector2::dot(vCurrToNext, epaResult.normal));
			edge[2] = currVertex;
			edge[3] = dot1 < dot2 ? prevVertex : nextVertex;
		}
		else
		{
			edge[0] = epaResult.simplex.m[0].v[0].v;
			edge[1] = epaResult.simplex.m[1].v[0].v;
			edge[2] = epaResult.simplex.m[0].v[1].v;
			edge[3] = epaResult.simplex.m[1].v[1].v;
		}

		ClipEdge clipResult = clipEdges(edge, clipNormal);

		result.count = clipResult.count;
		result.normal = epaResult.normal;
		if (!swap)
		{
			for (int i = 0; i < clipResult.count; ++i)
			{
				result.pA[i] = clipResult.refEdge[i];
				result.pB[i] = clipResult.incEdge[i];
				result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
			}
		}
		else
		{
			for (int i = 0; i < clipResult.count; ++i)
			{
				result.pA[i] = clipResult.incEdge[i];
				result.pB[i] = clipResult.refEdge[i];
				result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
			}
		}
		return result;
	}

	ContactResult Narrowphase2D::clipPolygonSegment(const Epa2DResult& epaResult, const Transform& transformA,
		const Shape* shapeA, const Transform& transformB, const Shape* shapeB, bool swap)
	{
		ContactResult result;
		return result;
	}

	ContactResult Narrowphase2D::clipCapsulePolygon(const Epa2DResult& epaResult, const Transform& transformA,
		const Shape* shapeA, const Transform& transformB, const Shape* shapeB, bool swap)
	{
		ContactResult result;
		// suppose shapeA: capsule, shapeB: polygon
		Vector2 clipNormal = -epaResult.normal;
		std::array<Vector2, 4> edge;
		int idxA = 0;
		int idxB = 1;
		if (swap)
		{
			clipNormal.negate();
			idxA = 1;
			idxB = 0;
		}

		bool sameCapsule = epaResult.simplex.m[0].v[idxA].i == epaResult.simplex.m[1].v[idxA].i;
		const Polygon* polygon = static_cast<const Polygon*>(shapeB);

		if (!sameCapsule)
		{
			edge[0] = epaResult.simplex.m[0].v[idxA].v;
			edge[1] = epaResult.simplex.m[1].v[idxA].v;

			int idx = epaResult.simplex.m[0].v[idxB].i;
			Vector2 currVertex = epaResult.simplex.m[0].v[idxB].v;
			int32_t prevIdx = (idx - 1 + polygon->vertices().size()) % polygon->vertices().size();
			int32_t nextIdx = (idx + 1) % polygon->vertices().size();
			Vector2 prevVertex = transformB.translatePoint(polygon->vertices()[prevIdx]);
			Vector2 nextVertex = transformB.translatePoint(polygon->vertices()[nextIdx]);
			Vector2 vCurrToPrev = prevVertex - currVertex;
			Vector2 vCurrToNext = nextVertex - currVertex;
			real dot1 = Math::abs(Vector2::dot(vCurrToPrev, epaResult.normal));
			real dot2 = Math::abs(Vector2::dot(vCurrToNext, epaResult.normal));

			edge[2] = currVertex;
			edge[3] = dot1 < dot2 ? prevVertex : nextVertex;
			ClipEdge clipResult = clipEdges(edge, clipNormal);

			result.count = clipResult.count;
			result.normal = epaResult.normal;
			if (!swap)
			{
				for (int i = 0; i < clipResult.count; ++i)
				{
					result.pA[i] = clipResult.refEdge[i];
					result.pB[i] = clipResult.incEdge[i];
					result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
				}
			}
			else
			{
				for (int i = 0; i < clipResult.count; ++i)
				{
					result.pA[i] = clipResult.incEdge[i];
					result.pB[i] = clipResult.refEdge[i];
					result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
				}
			}
		}
		else
		{
			// use minkowski difference 2
			result.count = 1;
			result.normal = epaResult.normal;
			result.pA[0] = epaResult.simplex.m[2].v[0].v;
			result.pB[0] = epaResult.simplex.m[2].v[1].v;
			result.penetration[0] = epaResult.penetration;
		}
		return result;
	}

	ContactResult Narrowphase2D::clipCapsuleCapsule(const Epa2DResult& epaResult, const Transform& transformA,
		const Shape* shapeA, const Transform& transformB, const Shape* shapeB)
	{
		ContactResult result;
		std::array<Vector2, 4> edge;
		Vector2 clipNormal = epaResult.normal;
		bool sameA = epaResult.simplex.m[0].v[0].i == epaResult.simplex.m[1].v[0].i;
		bool sameB = epaResult.simplex.m[0].v[1].i == epaResult.simplex.m[1].v[1].i;
		if (sameA && sameB)
		{
			result.count = 1;
			result.normal = epaResult.normal;
			result.pA[0] = epaResult.simplex.m[2].v[0].v;
			result.pB[0] = epaResult.simplex.m[2].v[1].v;
			result.penetration[0] = epaResult.penetration;
			return result;
		}

		if (!sameA && !sameB)
		{
			edge[0] = epaResult.simplex.m[0].v[0].v;
			edge[1] = epaResult.simplex.m[1].v[0].v;
			edge[2] = epaResult.simplex.m[0].v[1].v;
			edge[3] = epaResult.simplex.m[1].v[1].v;
			clipNormal = -epaResult.normal;
			auto clipResult = clipEdges(edge, clipNormal);
			result.count = clipResult.count;
			result.normal = epaResult.normal;
			for (int i = 0; i < clipResult.count; ++i)
			{
				result.pA[i] = clipResult.refEdge[i];
				result.pB[i] = clipResult.incEdge[i];
				result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
			}
			return result;
		}

		Vector2 localP;
		real halfWidth = 0;
		real halfHeight = 0;
		bool horizontal = false;
		Vector2 P1, P2;
		Transform transform;
		if (sameA && !sameB)
		{
			edge[0] = epaResult.simplex.m[0].v[1].v;
			edge[1] = epaResult.simplex.m[1].v[1].v;
			const Capsule* capsuleA = static_cast<const Capsule*>(shapeA);
			localP = transformA.inverseTranslatePoint(epaResult.simplex.m[0].v[0].v);
			halfWidth = capsuleA->halfWidth();
			halfHeight = capsuleA->halfHeight();
			horizontal = halfWidth > halfHeight;
			transform = transformA;
		}
		else if (!sameA && sameB)
		{
			edge[0] = epaResult.simplex.m[0].v[0].v;
			edge[1] = epaResult.simplex.m[1].v[0].v;
			clipNormal = -epaResult.normal;
			const Capsule* capsuleB = static_cast<const Capsule*>(shapeB);
			localP = transformB.inverseTranslatePoint(epaResult.simplex.m[0].v[1].v);
			halfWidth = capsuleB->halfWidth();
			halfHeight = capsuleB->halfHeight();
			horizontal = halfWidth > halfHeight;
			transform = transformB;
		}


		if (horizontal)
		{
			real pY = localP.y > 0 ? halfHeight : -halfHeight;
			if (localP.x > 0)
			{
				P1.set(halfWidth - halfHeight, pY);
				P2.set(-halfWidth + halfHeight, pY);
			}
			else
			{
				P1.set(-halfWidth + halfHeight, pY);
				P2.set(halfWidth - halfHeight, pY);
			}
		}
		else
		{
			real pX = localP.x > 0 ? halfWidth : -halfWidth;
			if (localP.y > 0)
			{
				P1.set(pX, halfHeight - halfWidth);
				P2.set(pX, -(halfHeight - halfWidth));
			}
			else
			{
				P1.set(pX, -(halfHeight - halfWidth));
				P2.set(pX, halfHeight - halfWidth);
			}
		}

		P1 = transform.translatePoint(P1);
		P2 = transform.translatePoint(P2);
		edge[2] = P1;
		edge[3] = P2;

		auto clipResult = clipEdges(edge, clipNormal);
		result.count = clipResult.count;
		result.normal = epaResult.normal;
		for (int i = 0; i < clipResult.count; ++i)
		{
			if (!sameA && sameB)
			{
				result.pA[i] = clipResult.refEdge[i];
				result.pB[i] = clipResult.incEdge[i];
			}
			else
			{
				result.pA[i] = clipResult.incEdge[i];
				result.pB[i] = clipResult.refEdge[i];
			}
			result.penetration[i] = (result.pA[i] - result.pB[i]).norm();
		}

		result.pA[0] = epaResult.simplex.m[2].v[0].v;
		result.pB[0] = epaResult.simplex.m[2].v[1].v;
		result.penetration[0] = epaResult.penetration;
		if (result.count == 0)
			result.count++;

		return result;
	}

	ContactResult Narrowphase2D::clipCapsuleSegment(const Epa2DResult& epaResult, const Transform& transformA,
		const Shape* shapeA, const Transform& transformB, const Shape* shapeB, bool swap)
	{
		ContactResult result;
		return result;
	}

	Simplex2D Narrowphase2D::solveSimplex(const Simplex2D& simplex)
	{
		Simplex2D result;
		// suppose simplex has 3 points
		CORE_ASSERT(simplex.count == 3, "Invalid simplex");
		const Vector2& A = simplex.m[0].p;
		const Vector2& B = simplex.m[1].p;
		const Vector2& C = simplex.m[2].p;
		// 0. find barycentric coordinates of origin
		const real normAB = (B - A).square();
		const real normAC = (C - A).square();
		const real normBC = (C - B).square();
		const real det = A.y * B.x - A.x * B.y + A.x * C.y - A.y * C.x + B.y * C.x - B.x * C.y;
		CORE_ASSERT(det != 0.0f, "Invalid simplex3")
		const real u = (B.y * C.x - C.y * B.x) / det;
		const real v = (C.y * A.x - A.y * C.x) / det;
		const real w = 1 - u - v;


		// 1. find closest edge to origin
		const real h_BC = u * std::abs(u) / (normBC + 1e-9f);
		const real h_AC = v * std::abs(v) / (normAC + 1e-9f);
		const real h_AB = w * std::abs(w) / (normAB + 1e-9f);
		////default: closest edge is AB
		////if min is h_AB, closest edge is AB, no need to swap
		////if min is h_AC, closest edge is AC, swap B and C
		////if min is h_BC, closest edge is BC, swap A and C
		//result = simplex;
		//if (min == h_AC)
		//	std::swap(result.m[1], result.m[2]);
		//else if (min == h_BC)
		//	std::swap(result.m[0], result.m[2]);

		//const int isMinAB = h_AB < h_AC && h_AB < h_BC;
		//const int isMinAC = h_AC < h_BC && h_AC < h_AB;
		//const int isMinBC = h_BC < h_AB && h_BC < h_AC;
		int idx = 0;
		idx = h_AC < h_BC && h_AC < h_AB ? 1 : idx;
		idx = h_BC < h_AB && h_BC < h_AC ? 2 : idx;

		constexpr int permutations[3][3] = {
		{0, 1, 2}, // Case h_AB is min: new order is A, B, C
		{0, 2, 1}, // Case h_AC is min: new order is A, C, B
		{2, 1, 0}  // Case h_BC is min: new order is C, B, A
		};

		for (int i = 0; i < 3; ++i)
			result.m[i] = simplex.m[permutations[idx][i]];

		// 3. if u>0, v>0, w>0, origin is in triangle
		result.isContainOrigin = u > 0 && v > 0 && w > 0;
		result.count = 3;
		return result;
	}

	SolveSimplexResult Narrowphase2D::solveSimplex3(const Simplex2D& simplex)
	{
		CORE_ASSERT(simplex.count == 3,"solveSimplex3 requires simplex count=3");
		SolveSimplexResult result;
		result.simplex = simplex;
		const Vector2& A = simplex.m[0].p;
		const Vector2& B = simplex.m[1].p;
		const Vector2& C = simplex.m[2].p;
		
		// find barycentric coordinates of origin
		const real det = A.y * B.x - A.x * B.y + A.x * C.y - A.y * C.x + B.y * C.x - B.x * C.y;
		//assert(det != 0.0f);
		const real u = (B.y * C.x - C.y * B.x) / (det + 1e-9f);
		const real v = (C.y * A.x - A.y * C.x) / (det + 1e-9f);
		const real w = 1 - u - v;

		// get barycentric weight of AB, AC, BC
		const real t_AB = originToSegmentWeight(A, B);
		const real t_AC = originToSegmentWeight(A, C);
		const real t_BC = originToSegmentWeight(B, C);

		// use them two check closest feature

		// three points
		if (u >= 0 && v >= 0 && w >= 0)
		{
			result.simplex.isContainOrigin = true;
			return result;
		}

		// Check vertex case
		if (t_AC > 1 && t_AB > 1)
		{
			result.simplex.m[0] = simplex.m[0];
			result.simplex.count = 1;
			result.direction = result.simplex.m[0].p.negative();
			return result;
		}
		if (t_AB < 0 && t_BC > 1)
		{
			result.simplex.m[0] = simplex.m[1];
			result.simplex.count = 1;
			result.direction = result.simplex.m[0].p.negative();
			return result;
		}
		if (t_BC < 0 && t_AC < 0)
		{
			result.simplex.m[0] = simplex.m[2];
			result.simplex.count = 1;
			result.direction = result.simplex.m[0].p.negative();
			return result;
		}

		// Check edge case
		if (t_AB >= 0 && t_AB <= 1 && w < 0)
		{
			// Save A, B
			result.simplex.m[0] = simplex.m[0];
			result.simplex.m[1] = simplex.m[1];
			result.simplex.count = 2;
			result.direction = getDirection(result.simplex.m[0].p, result.simplex.m[1].p, true);
			return result;
		}
		if (t_BC >= 0 && t_BC <= 1 && u < 0)
		{
			// Save B, C
			result.simplex.m[0] = simplex.m[1];
			result.simplex.m[1] = simplex.m[2];
			result.simplex.count = 2;
			result.direction = getDirection(result.simplex.m[0].p, result.simplex.m[1].p, true);
			return result;
		}

		// Only left Save A, C
		result.simplex.m[0] = simplex.m[0];
		result.simplex.m[1] = simplex.m[2];
		result.simplex.count = 2;
		result.direction = getDirection(result.simplex.m[0].p, result.simplex.m[1].p, true);
		return result;
	}

	SolveSimplexResult Narrowphase2D::solveSimplex2(const Simplex2D& simplex)
	{
		CORE_ASSERT(simplex.count == 2, "solveSimplex2 requires simplex count=2");
		SolveSimplexResult result;
		result.simplex = simplex;
		const real t = originToSegmentWeight(simplex.m[0].p, simplex.m[1].p);
		if (t > 1)
		{
			// M0
			result.simplex.count = 1;
			result.simplex.m[0] = simplex.m[0];
			result.direction = simplex.m[0].p.negative();
		}
		else if (t < 0)
		{
			// M1
			result.simplex.count = 1;
			result.simplex.m[0] = simplex.m[1];
			result.direction = simplex.m[1].p.negative();
		}
		else
		{
			result.direction = getDirection(simplex.m[0].p, simplex.m[1].p, true);
		}


		return result;
	}

	SolveSimplexResult Narrowphase2D::solveSimplex1(const Simplex2D& simplex)
	{
		CORE_ASSERT(simplex.count == 1, "solveSimplex1 requires simplex count=1");
		SolveSimplexResult result;
		// construct direction : M0-O
		result.simplex = simplex;
		result.direction = simplex.m[0].p.negative();
		return result;
	}

	real Narrowphase2D::originToSegmentWeight(const Vector2& p1, const Vector2& p2)
	{
		return Algorithm2D::pointToSegmentWeight(p1, p2, {}, false);
	}
}
