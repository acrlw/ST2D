#pragma once

#include "Shape.h"

namespace ST
{

	struct ST_API AABB
	{
		AABB() = default;
		AABB(const Vector2& topLeft, const real& boxWidth, const real& boxHeight);
		AABB(const Vector2& topLeft, const Vector2& bottomRight);
		real width = 0;
		real height = 0;
		Vector2 position;
		inline Vector2 topLeft()const;
		inline Vector2 topRight()const;
		inline Vector2 bottomLeft()const;
		inline Vector2 bottomRight()const;

		inline real minimumX()const;
		inline real minimumY()const;
		inline real maximumX()const;
		inline real maximumY()const;

		bool collide(const AABB& other) const;
		void expand(const real& factor);
		void scale(const real& factor);
		void clear();
		AABB& combine(const AABB& other);
		real surfaceArea()const;
		real volume()const;
		bool isSubset(const AABB& other)const;
		bool isEmpty()const;
		bool operator==(const AABB& other)const;
		bool raycast(const Vector2& start, const Vector2& direction)const;

		static AABB fromShape(const Transform& transform, const Shape* shape, const real& factor = 0);

		static AABB fromBox(const Vector2& topLeft, const Vector2& bottomRight);
		

		static bool collide(const AABB& src, const AABB& target);
		

		static AABB combine(const AABB& src, const AABB& target, const real& factor = 0);
		

		static bool isSubset(const AABB& a, const AABB& b);

		static void expand(AABB& aabb, const real& factor = 0.0);

		static bool raycast(const AABB& aabb, const Vector2& start, const Vector2& direction);

	};


}