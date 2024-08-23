#include "DynamicGrid.h"

namespace ST
{
	void DynamicGrid::clearAllObjects()
	{
	}

	void DynamicGrid::addObject(const BroadphaseObjectBinding& binding)
	{
	}

	void DynamicGrid::removeObject(int objectId)
	{
	}

	void DynamicGrid::updateObject(int objectId, const AABB& aabb)
	{
	}

	std::vector<ObjectPair> DynamicGrid::queryOverlaps()
	{
	}

	std::vector<int> DynamicGrid::queryAABB(const AABB& aabb)
	{
	}

	std::vector<int> DynamicGrid::queryRay(const Vector2& origin, const Vector2& direction, float maxDistance)
	{
	}
}
