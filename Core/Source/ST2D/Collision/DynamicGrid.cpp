#include "DynamicGrid.h"

#include "ST2D/Log.h"

namespace ST
{
	void DynamicGrid::initializeGrid()
	{
		m_usedCells.clear();
		m_grid.clear();

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
				m_grid[i * m_col + j].push_back(objectBinding);
				m_usedCells[cellPos].push_back(objectBinding);

			}
		}

		m_objects.push_back(objectBinding);



		//CORE_INFO("ID: {}, Row:({}, {}), Col:({}, {})", binding.objectId,
		//	rowStart, rowEnd, colStart, colEnd);

	}

	void DynamicGrid::removeObject(int objectId)
	{
		std::vector<CellPosition> cells;
		for (auto iter = m_objects.begin(); iter != m_objects.end(); iter++)
		{
			if (iter->binding.objectId == objectId)
			{
				cells = iter->cells;
				m_objects.erase(iter);
				break;
			}
		}
		if (cells.empty())
			return;

		for (auto&& elem : cells)
		{
			std::erase_if(m_usedCells[elem],
			              [objectId](const GridObjectBinding& binding)
			              {
				              return binding.binding.objectId == objectId;
			              });
		}
	}

	void DynamicGrid::updateObject(const BroadphaseObjectBinding& binding)
	{
		incrementalUpdate(binding);
	}

	std::vector<ObjectPair> DynamicGrid::queryOverlaps()
	{
		std::vector<ObjectPair> result;

		if (m_usedCells.empty())
			return result;

		for (auto&& [key, cell] : m_usedCells)
		{
			if (cell.size() < 2)
				continue;

			for (int i = 0; i < cell.size(); i++)
			{
				for (int j = i + 1; j < cell.size(); j++)
				{
					ObjectPair pair;
					pair.objectIdA = cell[i].binding.objectId;
					pair.objectIdB = cell[j].binding.objectId;

					result.push_back(pair);
				}
			}
		}


		return result;
	}

	std::vector<int> DynamicGrid::queryAABB(const AABB& aabb)
	{
		std::vector<int> result;

		if (m_grid.empty())
			return result;

		Vector2 topLeft = aabb.topLeft();
		Vector2 bottomRight = aabb.bottomRight();

		int rowStart = static_cast<int>(std::floor((m_gridTopLeft.y - topLeft.y) / m_cellHeight));
		int colStart = static_cast<int>(std::floor((topLeft.x - m_gridTopLeft.x) / m_cellWidth));

		int rowEnd = static_cast<int>(std::floor((m_gridTopLeft.y - bottomRight.y) / m_cellHeight));
		int colEnd = static_cast<int>(std::floor((bottomRight.x - m_gridTopLeft.x) / m_cellWidth));

		


		return result;
	}

	std::vector<int> DynamicGrid::queryRay(const Vector2& origin, const Vector2& direction, float maxDistance)
	{
		std::vector<int> result;

		if (m_grid.empty())
			return result;


		return result;
	}

	void DynamicGrid::incrementalUpdate(const BroadphaseObjectBinding& binding)
	{

	}
}
