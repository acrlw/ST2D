#pragma once

#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Shape/Shape.h"
#include "Simplex.h"


namespace ST
{
	struct ST_API SweepVolume
	{
		Vector2 start, direction, end;
		real halfWidth = 0.0f;

		std::vector<Vector2> vertices() const
		{
			std::vector<Vector2> result;
			Vector2 p0 = start + direction * halfWidth;
			Vector2 p1 = start - direction * halfWidth;
			Vector2 p2 = end - direction * halfWidth;
			Vector2 p3 = end + direction * halfWidth;
			result.push_back(p1);
			result.push_back(p0);
			result.push_back(p3);
			result.push_back(p2);
			return result;
		}

		static SweepVolume fromPolygon(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3)
		{
			//p0, p1, p2, p3 is CCW
			SweepVolume result;
			result.start = (p0 + p1) * 0.5f;
			result.end = (p2 + p3) * 0.5f;
			Vector2 dir = p1 - p0;
			real length = dir.length();
			result.halfWidth = length * 0.5f;
			result.direction = dir / length;
			return result;
		}
	};

	struct ST_API SimplexVertexDistPair
	{
		SimplexVertex vertex;
		real distance = 0.0f;
	};

	struct ST_API SimplexDistPair
	{
		Simplex simplex;
		real distance = 0.0f;
		bool operator>(const SimplexDistPair& other) const
		{
			return distance > other.distance;
		}

	};

	struct ST_API Feature
	{
		//circle and ellipse, use index 0
		//edge use index 0 and 1
		Vector2 vertex[2];
		Index index[2] = { UINT32_MAX, UINT32_MAX };
	};

	struct ST_API ClipVertex
	{
		Vector2 vertex;
		bool isClip = false;
		Vector2 clipperVertex;
		bool isFinalValid = false;
	};

	struct ST_API ContactPair
	{
		//contact pair1:
		//	points[0]: pointA
		//	points[1]: pointB

		//if there is second contact pair:
		//	points[2]: pointA
		//	points[3]: pointB
		std::array<Vector2, 4> points;
		std::array<uint64_t, 2> ids{};
		uint32_t count = 0;

		void addContact(const Vector2& pointA, const Vector2& pointB)
		{
			assert(count <= 4);
			points[count++] = pointA;
			points[count++] = pointB;
		}
	};


	struct ST_API VertexPair
	{
		VertexPair() = default;
		Vector2 pointA;
		Vector2 pointB;

		bool isEmpty() const
		{
			return pointA.fuzzyEqual({ 0, 0 }) && pointB.fuzzyEqual({ 0, 0 });
		}

		bool operator==(const VertexPair& other) const
		{
			return (other.pointA.fuzzyEqual(this->pointA) && other.pointB.fuzzyEqual(this->pointB))
				|| (other.pointB.fuzzyEqual(this->pointA) && other.pointA.fuzzyEqual(this->pointB));
		}
	};

	struct ST_API CollisionInfo
	{
		Vector2 normal;
		real penetration = 0;
		Simplex simplex;
		VertexPair pair;
		//[Debug]
		Simplex originalSimplex;
		std::list<SimplexVertexDistPair> polytope;
	};



	class ST_API Narrowphase
	{
	public:

		static Simplex gjk(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const size_t& iteration = 30);

		static CollisionInfo epa(const Simplex& simplex, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const size_t& iteration = 12, const real& epsilon = Constant::GeometryEpsilon);

		static CollisionInfo findClosestSimplex(const Simplex& simplex, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const size_t& iteration = 12);

		static SimplexVertex support(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Vector2& direction);

		static VertexIndexPair findFurthestPoint(const Transform& transform, const Shape* shape, const Vector2& direction);

		static Vector2 findDirectionByEdge(const SimplexVertex& v1, const SimplexVertex& v2, bool pointToOrigin);

		static VertexIndexPair findFurthestPoint(const std::vector<Vector2>& vertices,
			const Vector2& direction);

		static ContactPair generateContacts(CollisionInfo& info, const Transform& transformA, const Shape* shapeA,
		                                    const Transform& transformB, const Shape* shapeB);

		static CollisionInfo distance(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const size_t& iteration = 30);

		static SweepVolume linearSweep(const Transform& start, const Transform& end, const Shape* shape);

		static bool linearSweepCast(const Transform& transformA, const Shape* shapeA, const Transform& transformB, const Shape* shapeB, const Vector2& direction,
			const real& maxDistance, Transform& resultA);

	private:

		static bool linearSweepBackwardCast(const Simplex& startSimplex, const Transform& transformA, const Shape* shapeA, const Transform& transformB, const Shape* shapeB, const Vector2& direction, 
			Transform& resultA);

		static bool linearSweepForwardCast(CollisionInfo& info, const Transform& transformA, const Shape* shapeA, const Transform& transformB, const Shape* shapeB, const Vector2& direction,
			Transform& resultA);

		static void reconstructSimplexByVoronoi(Simplex& simplex);

		static bool perturbSimplex(Simplex& simplex, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Vector2& dir);

		static Feature findFeatures(const Simplex& simplex, const Vector2& normal, const Transform& transform, const Shape* shape,
			const Index& AorB);

		static ContactPair clipTwoEdge(const Vector2& va1, const Vector2& va2, const Vector2& vb1, const Vector2& vb2,
			CollisionInfo& info);

		static ContactPair clipIncidentEdge(std::array<ClipVertex, 2>& incEdge, std::array<Vector2, 2> refEdge,
			const Vector2& normal, bool swap);

		static ContactPair clipPolygonPolygon(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipPolygonEdge(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipPolygonCapsule(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipPolygonRound(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipEdgeCapsule(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipEdgeRound(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipCapsuleCapsule(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipCapsuleRound(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipRoundRound(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const Feature& featureA, const Feature& featureB, CollisionInfo& info);

		static ContactPair clipEdgeVertex(const Vector2& va1, const Vector2& va2, const Vector2& vb,
			CollisionInfo& info);

		static void polytopeIterNext(std::list<SimplexVertexDistPair>::iterator& targetIter,
			std::list<SimplexVertexDistPair>& list);
		static void polytopeIterPrev(std::list<SimplexVertexDistPair>::iterator& targetIter,
			std::list<SimplexVertexDistPair>& list);

		static void buildPolytopeFromSimplex(std::list<SimplexVertexDistPair>& polytope,
			const Simplex& simplex);
	};
}
