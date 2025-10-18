#pragma once

#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Shape/AbstractShape.h"
#include "ST2D/Math/Transform2D.h"

namespace ST
{
	//define max points in polytope
	// test for 1e-5 eps
	// max vertices: 128
	// max faces: 256
	// max iter: 128
	constexpr size_t MaxPolytopeVertices = 16;
	constexpr size_t MaxPolytopeFaces = 32;
	constexpr size_t MaxGJKEPAIteration = 16;

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
		std::array<real, 2> penetration = {0, 0};
		Vector2 normal;
		int count = 0;
	};

	struct ST_API ClipEdge
	{
		std::array<Vector2, 2> refEdge;
		std::array<Vector2, 2> incEdge;
		int count = 0;
	};

	struct ST_API MaxPenetrationResult
	{
		uint32_t refIdx;
		uint32_t incIdx;
		Vector2 normal;
		real penetration;
	};

	class ST_API Narrowphase2D
	{
	public:
		static Simplex2D gjk(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const uint32_t& iteration = MaxGJKEPAIteration, const real& epsilon = Constant::GeometryEpsilon, bool earlyStop = true, const Vector2& initialDirection = Vector2(1, 0));

		static Epa2DResult epa(const Simplex2D& simplex, const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const uint32_t& iteration = MaxGJKEPAIteration, const real& epsilon = Constant::GeometryEpsilon);

		static Distance2DResult distance(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const uint32_t& iteration = MaxGJKEPAIteration, const real& epsilon = Constant::GeometryEpsilon);

		static Distance2DResult distanceRound(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const real& radius1, const real& radius2, const uint32_t& iteration = 30, const real& epsilon = Constant::GeometryEpsilon);

		static MinkowskiDiff support(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
		                             const AbstractShape* shapeB, const Vector2& direction);

		static FurthestVertex findFurthestVertex(const Transform2D& transform, const AbstractShape* shape, const Vector2& direction);

		static FurthestVertex findFurthestVertex(const std::vector<Vector2>& vertices,
			const Vector2& direction);

		
		static FurthestVertex findFurthestVertexHillClimbing(const Vector2* vertices, const int32_t& count,
		                                              const Vector2& direction,
		                                              int32_t startIndex = 0);

		static FurthestVertex findFurthestVertexSupportField(const AbstractShape* shape, const Vector2& direction);

		static Vector2 getDirection(const Vector2 p1, const Vector2 p2, bool pointToOrigin);

		static ContactResult generateContacts(const Epa2DResult& epaResult, const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB);

		static MaxPenetrationResult findMaxPenetration(const Vector2* polygonA, const uint32_t& countA, 
			const Vector2* polygonB, const uint32_t& countB);

		static ContactResult collidePolygons(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const real& skinRadiusA = 0, const real& skinRadiusB = 0);

		static ContactResult collideCircles(const Vector2& positionA, const real& radiusA, const Vector2& positionB, const real& radiusB);

		static ContactResult collideCapsuleCircle(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB);

		static ContactResult collideCapsuleCircle(const Vector2& A1, const Vector2& A2, const real& radiusA, const Vector2& positionB, const real& radiusB);

		static ContactResult collideCapsules(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB);

		static ContactResult collideCapsules(const Vector2& A1, const Vector2& A2, const Vector2& B1, const Vector2& B2, const real& radiusA, const real& radiusB);

		static ContactResult collideCapsulePolygon(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, const real& skinRadiusA = 0, const real& skinRadiusB = 0);

		static ContactResult collideCapsulePolygon(const Transform2D& transformA, const real& halfLength, const real& radius,
			const Transform2D& transformB, const AbstractShape* shapeB, const real& skinRadiusA = 0, const real& skinRadiusB = 0);

		static ContactResult collideCapsulePolygon(const Vector2& A1, const Vector2& A2, const real& radiusA, const Transform2D& transformB,
			const AbstractShape* shapeB, const real& skinRadiusA = 0, const real& skinRadiusB = 0);

		static ContactResult collideCirclePolygon(const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB);

		static ContactResult collideCirclePolygon(const Vector2& positionA, const real& radiusA, const Transform2D& transformB, const AbstractShape* shapeB);

		static ClipEdge clipEdges(const Vector2& ref0, const Vector2& ref1, const Vector2& inc0, const Vector2& inc1);
		static ClipEdge clipEdges(const std::array<Vector2, 2>& refEdge, const std::array<Vector2, 2>& incEdge);
		static ClipEdge clipEdges(const std::array<Vector2, 4>& edge);
	private:
		static std::array<Vector2, 2> getPolygonClipEdge(const Transform2D& transform, const AbstractShape* shape, const Vector2& clipNormal, const Vector2& supportPoint, const int32_t& idx);


		static ContactResult clipPolygons(const Epa2DResult& epaResult, const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB);

		static ContactResult clipPolygonSegment(const Epa2DResult& epaResult, const Transform2D& transformA, const AbstractShape* shapeA, const Transform2D& transformB,
			const AbstractShape* shapeB, bool swap = false);


		static Polytope initializePolytope(const Simplex2D& simplex);

		static SolveSimplexResult solveSimplex3(const Simplex2D& simplex);

		static SolveSimplexResult solveSimplex2(const Simplex2D& simplex);

		inline static real originToSegmentWeight(const Vector2& p1, const Vector2& p2);
	};

}