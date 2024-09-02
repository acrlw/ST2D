#pragma once

#include "ST2D/Shape/AABB.h"

namespace ST
{
	struct ST_API BroadphaseObjectBinding
	{
		int32_t objectId = -1;
		int32_t bitmask = 1;
		AABB aabb;
		void* userData = nullptr;
	};

	class ST_API AbstractBroadphase
	{
	public:
		AbstractBroadphase() = default;
		virtual ~AbstractBroadphase() = default;

		virtual void clearAllObjects() = 0;
		virtual void addObject(const BroadphaseObjectBinding& binding) = 0;
		virtual void removeObject(int objectId) = 0;
		virtual void updateObject(const BroadphaseObjectBinding& binding) = 0;
		virtual std::vector<ObjectPair> queryOverlaps() = 0;
		virtual std::vector<int> queryAABB(const AABB& aabb) = 0;
		virtual std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) = 0;

	private:
	};
}