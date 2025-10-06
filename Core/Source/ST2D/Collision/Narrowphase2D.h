#pragma once

#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Shape/Shape.h"

namespace ST
{
	//define max points in polytope
	constexpr size_t MaxPolytopeVertices = 32;
	constexpr size_t MaxPolytopeFaces = 32;

	struct ST_API FurthestVertex
	{
		Vector2 v;
		int32_t i = -1;
	};
	struct ST_API MinkowskiDiff
	{
		MinkowskiDiff() = default;
		std::array<FurthestVertex, 2> v; //v[0] from shape A, v[1] from shape B
		Vector2 p;
	};

	struct ST_API Simplex2D
	{
		std::array<MinkowskiDiff, 3> m; //v[0] and v[1] is the segment closest to origin, v[2] is the newest vertex
		uint32_t count = 0;
		bool isContainOrigin = false;
	};


	enum class EpaState
	{
		Valid,
		MaxIteration,
		MaxFaces,
		InvalidInput,
	};

	struct ST_API Polytope
	{
		std::array<MinkowskiDiff, MaxPolytopeVertices> vertices;
		//face idx -> (vertex idx 1, vertex idx 2)
		std::array<std::array<uint32_t, 2>, MaxPolytopeFaces> faces = { 0 };
		//active face idx, same norm as faces, 0 means inactive
		std::array<uint32_t, MaxPolytopeFaces> activeFaces = { 0 };
		std::array<real, MaxPolytopeFaces> distances = { 0.0f };
		uint32_t nVertex = 0;
		uint32_t nFace = 0;
		uint32_t closestFaceIdx = 0;
	};

	struct ST_API Epa2DResult
	{
		Vector2 normal;
		real penetration = 0.0f;
		EpaState state = EpaState::InvalidInput;
		Simplex2D simplex;
		Polytope polytope; // debug info
	};

	struct ST_API Distance2DResult
	{
		real distance = 0.0f;
		Simplex2D simplex;
		std::array<Vector2, 2> closestPoints; //closestPoints[0] from shape A, closestPoints[1] from shape B
	};

	struct ST_API SolveSimplexResult
	{
		Simplex2D simplex;
		Vector2 direction;
	};

	struct ST_API ContactResult
	{
		std::array<Vector2, 2> pA;
		std::array<Vector2, 2> pB;
		std::array<real, 2> penetration;
		Vector2 normal;
		int count = 0;
	};

	struct ST_API ClipEdge
	{
		std::array<Vector2, 2> refEdge;
		std::array<Vector2, 2> incEdge;
		int count = 0;
	};

	class ST_API Narrowphase2D
	{
	public:
		static Simplex2D gjk(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const uint32_t& iteration = 30, const Vector2& initialDirection = Vector2(0, 0));

		static Epa2DResult epa(const Simplex2D& simplex, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const uint32_t& iteration = 30, const real& epsilon = Constant::GeometryEpsilon);

		static Distance2DResult distance(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const uint32_t& iteration = 30, const real& epsilon = Constant::GeometryEpsilon);

		static Distance2DResult distanceRound(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, const real& radius1, const real& radius2, const uint32_t& iteration = 30, const real& epsilon = Constant::GeometryEpsilon);

		static MinkowskiDiff support(const Transform& transformA, const Shape* shapeA, const Transform& transformB,
		                             const Shape* shapeB, const Vector2& direction);

		static FurthestVertex findFurthestVertex(const Transform& transform, const Shape* shape, const Vector2& direction);

		static FurthestVertex findFurthestVertex(const std::vector<Vector2>& vertices,
			const Vector2& direction);

		static FurthestVertex findFurthestVertexHillClimbing(const std::vector<Vector2>& vertices, const Vector2& direction, int32_t startIndex = 0);

		static Vector2 getDirection(const Vector2 p1, const Vector2 p2, bool pointToOrigin);

		static ContactResult generateContacts(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB);

	private:

		static ClipEdge clipEdges(const std::array<Vector2, 4>& edge, const Vector2& clipNormal);

		static ContactResult clipPolygonPolygon(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB);

		static ContactResult clipPolygonSegment(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, bool swap = false);

		static ContactResult clipCapsulePolygon(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, bool swap = false);

		static ContactResult clipCapsuleCapsule(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB);

		static ContactResult clipCapsuleSegment(const Epa2DResult& epaResult, const Transform& transformA, const Shape* shapeA, const Transform& transformB,
			const Shape* shapeB, bool swap = false);

		static Polytope initializePolytope(const Simplex2D& simplex);

		static Simplex2D solveSimplex(const Simplex2D& simplex);

		static SolveSimplexResult solveSimplex3(const Simplex2D& simplex);

		static SolveSimplexResult solveSimplex2(const Simplex2D& simplex);

		static SolveSimplexResult solveSimplex1(const Simplex2D& simplex);

		inline static real originToSegmentWeight(const Vector2& p1, const Vector2& p2);
	};

}