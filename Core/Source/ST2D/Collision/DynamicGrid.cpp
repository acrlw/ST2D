#include "DynamicGrid.h"

#include "ST2D/Log.h"

namespace ST
{
	void DynamicGrid::initializeGrid()
	{
		m_grid.resize(m_row * m_col);

		m_cellWidth = 2.0f * m_halfWidth / static_cast<real>(m_col);
		m_cellHeight = 2.0f * m_halfHeight / static_cast<real>(m_row);

		m_gridTopLeft = Vector2(-m_halfWidth, m_halfHeight) + m_origin;
		m_gridBottomRight = Vector2(m_halfWidth, -m_halfHeight) + m_origin;

	}

	void DynamicGrid::clearAllObjects()
	{
		m_grid.clear();
		m_objects.clear();
		initializeGrid();
	}

	void DynamicGrid::addObject(const BroadphaseObjectBinding& binding)
	{
		for(auto&& elem: m_objects)
		{
			if (elem.binding.objectId == binding.objectId)
			{
				CORE_WARN("Object with ID: {} already exists in the grid", binding.objectId);
				return;
			}
		}

		AABB aabb = binding.aabb;
		Vector2 topLeft = aabb.topLeft();
		Vector2 bottomRight = aabb.bottomRight();

		int rowStart = static_cast<int>(std::floor((m_gridTopLeft.y - topLeft.y) / m_cellHeight));
		int colStart = static_cast<int>(std::floor((topLeft.x - m_gridTopLeft.x) / m_cellWidth));

		int rowEnd = static_cast<int>(std::floor((m_gridTopLeft.y - bottomRight.y) / m_cellHeight));
		int colEnd = static_cast<int>(std::floor((bottomRight.x - m_gridTopLeft.x) / m_cellWidth));

		GridObjectBinding objectBinding;
		objectBinding.binding = binding;

		for (int i = rowStart; i <= rowEnd; i++)
		{
			for (int j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos;
				cellPos.row = i;
				cellPos.col = j;

				objectBinding.cells.push_back(cellPos);

				m_grid[i * m_col + j].bindings.push_back(objectBinding);

			}
		}

		m_objects.push_back(objectBinding);



		//CORE_INFO("ID: {}, Row:({}, {}), Col:({}, {})", binding.objectId,
		//	rowStart, rowEnd, colStart, colEnd);

	}

	void DynamicGrid::removeObject(int objectId)
	{

	}

	void DynamicGrid::updateObject(int objectId, const AABB& aabb)
	{

	}

	std::vector<ObjectPair> DynamicGrid::queryOverlaps()
	{
		std::vector<ObjectPair> result;

		return result;
	}

	std::vector<int> DynamicGrid::queryAABB(const AABB& aabb)
	{
		std::vector<int> result;

		return result;
	}

	std::vector<int> DynamicGrid::queryRay(const Vector2& origin, const Vector2& direction, float maxDistance)
	{
		std::vector<int> result;

		return result;
	}
}
