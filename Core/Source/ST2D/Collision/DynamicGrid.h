#pragma once

#include "ST2D/Shape/AABB.h"

#include "AbstractBroadphase.h"

namespace ST
{
	struct ST_API GridCellBinding
	{
		BroadphaseObjectBinding binding;
		//how many cells does this object occupy
		std::vector<GridCellBinding> cells;
	};

	struct ST_API GridCell
	{
		//how many objects are in this cell
		std::vector<GridCellBinding> bindings;
	};

	class ST_API DynamicGrid : AbstractBroadphase
	{
	public:

		void initializeGrid();
		void clearAllObjects() override;
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(int objectId, const AABB& aabb) override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;


		//private:

		real m_halfWidth = 20.0f;
		real m_halfHeight = 20.0f;

		uint32_t m_row = 20;
		uint32_t m_col = 20;

		real m_cellWidth = 0.0f;
		real m_cellHeight = 0.0f;

		Vector2 m_origin = Vector2(0.0f, 0.0f);

		Vector2 m_gridTopLeft;
		Vector2 m_gridBottomRight;

		std::vector<GridCell> m_grid;

	};
}
