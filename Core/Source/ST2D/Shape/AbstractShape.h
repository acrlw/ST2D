#pragma once

#include "ST2D/Log.h"
#include "ST2D/Algorithms/Algorithm2D.h"
#include "ST2D/Math/Complex.h"
#include "ST2D/Math/Matrix2x2.h"

namespace ST
{

	enum class ShapeType
	{
		Polygon,
		Segment,
		Capsule,
		Circle,
		Ellipse
	};
	class ST_API AbstractShape
	{
	public:

		ShapeType type() const
		{
			return m_type;
		}

		virtual void scale(const real& factor) = 0;

		virtual ~AbstractShape()
		{
		};
		virtual bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) = 0;
		virtual Vector2 center() const = 0;

	protected:
		ShapeType m_type;
	};


	/**
	 * \brief Basic Shape Description Primitive. Including shape and transform.
	 */
	//struct ST_API ShapePrimitive
	//{
	//	ShapePrimitive() = default;

	//	AbstractShape* shape = nullptr;
	//	Transform2D transform;
	//	uint32_t bitmask = 0;
	//	uint32_t uuid = 0;

	//	void* data = nullptr;

	//	bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) const
	//	{
	//		if (shape == nullptr)
	//			return false;
	//		return shape->contains(transform.inverseTranslatePoint(point), epsilon);
	//	}
	//};



	//using PairID = uint64_t;

	//inline PairID mixPairUUID(uint32_t idA, uint32_t idB)
	//{
	//	//Combine two 32-bit id into one 64-bit id in binary form
	//	//By Convention: bodyA.id < bodyB.id
	//	auto bodyAId = idA;
	//	auto bodyBId = idB;
	//	if (bodyAId > bodyBId)
	//		std::swap(bodyAId, bodyBId);

	//	auto pair = std::pair{ bodyAId, bodyBId };
	//	const auto result = reinterpret_cast<uint64_t&>(pair);
	//	return result;
	//}

	//struct ST_API ShapePair
	//{
	//	static ShapePair makeShapePair(ShapePrimitive* A, ShapePrimitive* B)
	//	{
	//		ShapePair pair;
	//		pair.bodyA = A;
	//		pair.bodyB = B;
	//		pair.id = mixPairUUID(A->uuid, B->uuid);
	//		return pair;
	//	}

	//	ShapePrimitive* bodyA;
	//	ShapePrimitive* bodyB;
	//	PairID id;
	//};

}
