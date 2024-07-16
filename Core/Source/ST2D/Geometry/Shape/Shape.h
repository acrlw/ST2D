#pragma once

#include "ST2D/Math/Matrix2x2.h"

namespace ST
{

	enum class ShapeType
	{
		Polygon,
		Edge,
		Capsule,
		Circle,
		Ellipse
	};
	class ST_API Shape
	{
	public:

		ShapeType type() const
		{
			return m_type;
		}

		virtual void scale(const real& factor) = 0;

		virtual ~Shape()
		{
		};
		virtual bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) = 0;
		virtual Vector2 center() const = 0;

	protected:
		ShapeType m_type;
	};

	struct ST_API Transform
	{
		//refer https://docs.unity3d.com/ScriptReference/Transform.html
		Vector2 position;
		real rotation = 0;
		real scale = 1.0f;

		Vector2 translatePoint(const Vector2& source) const
		{
			return Matrix2x2(rotation).multiply(source) * scale + position;
		}

		Vector2 inverseTranslatePoint(const Vector2& source) const
		{
			return Matrix2x2(-rotation).multiply(source - position) / scale;
		}

		Vector2 inverseRotatePoint(const Vector2& point) const
		{
			return Matrix2x2(-rotation).multiply(point);
		}
	};

	struct ST_API ExtraData
	{
		uint32_t bitmask;
		uint32_t uuid;
		void* data = nullptr;
	};

	/**
	 * \brief Basic Shape Description Primitive. Including shape and transform.
	 */
	struct ST_API ShapePrimitive
	{
		ShapePrimitive() = default;
		ExtraData userData;
		Shape* shape = nullptr;
		Transform transform;

		bool contains(const Vector2& point, const real& epsilon = Constant::GeometryEpsilon) const
		{
			if (shape == nullptr)
				return false;
			return shape->contains(transform.inverseTranslatePoint(point), epsilon);
		}
	};



	using PairID = uint64_t;

	inline PairID mixPairUUID(uint32_t idA, uint32_t idB)
	{
		//Combine two 32-bit id into one 64-bit id in binary form
		//By Convention: bodyA.id < bodyB.id
		auto bodyAId = idA;
		auto bodyBId = idB;
		if (bodyAId > bodyBId)
			std::swap(bodyAId, bodyBId);

		auto pair = std::pair{ bodyAId, bodyBId };
		const auto result = reinterpret_cast<uint64_t&>(pair);
		return result;
	}

	struct ST_API ShapePair
	{
		static ShapePair makeShapePair(ShapePrimitive* A, ShapePrimitive* B)
		{
			ShapePair pair;
			pair.bodyA = A;
			pair.bodyB = B;
			pair.id = mixPairUUID(A->userData.uuid, B->userData.uuid);
			return pair;
		}

		ShapePrimitive* bodyA;
		ShapePrimitive* bodyB;
		PairID id;
	};

}
