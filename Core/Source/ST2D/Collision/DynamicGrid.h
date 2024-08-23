#pragma once

#include "ST2D/Shape/AABB.h"

#include "AbstractBroadphase.h"

namespace ST
{
	union ST_API GridCellPosition
	{
		struct
		{
			uint32_t x = 0;
			uint32_t y = 0;
		} pos;
		uint64_t value = 0;
	};


	struct ST_API GridCellBinding
	{
		BroadphaseObjectBinding binding;
		std::vector<GridCellBinding> indices;
	};

	class ST_API DynamicGrid : AbstractBroadphase
	{
	public:
		void clearAllObjects() override;
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(int objectId, const AABB& aabb) override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;


		//private:



		real m_width = 100.0f;
		real m_height = 100.0f;

		uint32_t m_row = 10;
		uint32_t m_col = 10;


	};
}
