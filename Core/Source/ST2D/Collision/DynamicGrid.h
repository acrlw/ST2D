#pragma once

#include "ST2D/Shape/AABB.h"

#include "AbstractBroadphase.h"

namespace ST
{
	struct ST_API CellPosition
	{
		uint32_t row = 0;
		uint32_t col = 0;
	};
	struct ST_API GridObjectBinding
	{
		BroadphaseObjectBinding binding;
		//which cells this object is in
		std::vector<CellPosition> cells;
	};

	struct ST_API GridCell
	{
		//how many objects are in this cell
		std::vector<GridObjectBinding> bindings;
	};

	class ST_API DynamicGrid : AbstractBroadphase
	{
	public:

		void initializeGrid();
		void clearAllObjects() override;
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(const BroadphaseObjectBinding& binding) override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;


		//private:

		void incrementalUpdate(const BroadphaseObjectBinding& binding);

		real m_halfWidth = 50.0f;
		real m_halfHeight = 50.0f;

		uint32_t m_row = 200;
		uint32_t m_col = 200;

		real m_cellWidth = 0.0f;
		real m_cellHeight = 0.0f;

		Vector2 m_origin = Vector2(0.0f, 0.0f);

		Vector2 m_gridTopLeft;
		Vector2 m_gridBottomRight;

		std::vector<GridCell> m_grid;
		std::vector<GridObjectBinding> m_objects;

	};
}
