#include "DynamicGrid.h"

#include <ranges>
#include <unordered_set>

#include "ST2D/Log.h"

namespace ST
{
	void DynamicGrid::initializeGrid()
	{
		m_objects.clear();
		m_usedCells.clear();

		m_cellWidth = 2.0f * m_halfWidth / static_cast<real>(m_col);
		m_cellHeight = 2.0f * m_halfHeight / static_cast<real>(m_row);

		m_gridTopLeft = Vector2(-m_halfWidth, m_halfHeight) + m_origin;
		m_gridBottomRight = Vector2(m_halfWidth, -m_halfHeight) + m_origin;
	}

	void DynamicGrid::clearAllObjects()
	{
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

		uint32_t rowStart = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - topLeft.y) / m_cellHeight));
		uint32_t rowEnd = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - bottomRight.y) / m_cellHeight));
		uint32_t colStart = static_cast<uint32_t>(std::floor((topLeft.x - m_gridTopLeft.x) / m_cellWidth));
		uint32_t colEnd = static_cast<uint32_t>(std::floor((bottomRight.x - m_gridTopLeft.x) / m_cellWidth));


		GridObjectBinding objectBinding;
		objectBinding.binding = binding;

		for (uint32_t i = rowStart; i <= rowEnd; i++)
		{
			for (uint32_t j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos;
				cellPos.row = i;
				cellPos.col = j;

				objectBinding.cells.push_back(cellPos);

			}
		}

		m_objects.push_back(objectBinding);

		for (uint32_t i = rowStart; i <= rowEnd; i++)
		{
			for (uint32_t j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos;
				cellPos.row = i;
				cellPos.col = j;

				m_usedCells[cellPos].push_back(objectBinding);
			}
		}


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
			if (!m_usedCells.contains(elem))
				continue;

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
		std::unordered_set<ObjectPair, ObjectPairHash> uniquePairs;

		if (m_usedCells.empty())
			return result;

		int counter = 0;

		for (auto& value : m_usedCells | 
			std::views::values | 
			std::views::filter([](const GridCellObjectsList& cell)
				{ return cell.size() > 1; }))
		{
			for (int i = 0; i < value.size(); i++)
			{
				for (int j = i + 1; j < value.size(); j++)
				{
					counter++;
					if (!(value[i].binding.bitmask & value[j].binding.bitmask))
						continue;

					if (!value[i].binding.aabb.collide(value[j].binding.aabb))
						continue;

					ObjectPair pair(value[i].binding.objectId, value[j].binding.objectId);
					if (!uniquePairs.contains(pair))
					{
						uniquePairs.insert(pair);
						result.push_back(pair);
					}
				}
			}
		}

		CORE_INFO("[Grid] Overlaps Query Counter: {}", counter);

		return result;
	}

	std::vector<int> DynamicGrid::queryAABB(const AABB& aabb)
	{
		std::vector<int> result;
		std::unordered_set<int> uniqueObjects;

		if (m_usedCells.empty())
			return result;

		int counter = 0;

		Vector2 topLeft = aabb.topLeft();
		Vector2 bottomRight = aabb.bottomRight();

		uint32_t rowStart = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - topLeft.y) / m_cellHeight));
		uint32_t colStart = static_cast<uint32_t>(std::floor((topLeft.x - m_gridTopLeft.x) / m_cellWidth));

		uint32_t rowEnd = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - bottomRight.y) / m_cellHeight));
		uint32_t colEnd = static_cast<uint32_t>(std::floor((bottomRight.x - m_gridTopLeft.x) / m_cellWidth));

		for (auto& [key, value] : m_usedCells |
			std::views::filter([rowStart, rowEnd, colStart, colEnd](const std::pair<CellPosition, GridCellObjectsList>& cell)
				{
					return cell.first.row >= rowStart && cell.first.row <= rowEnd &&
						cell.first.col >= colStart && cell.first.col <= colEnd;
				}))
		{
			for (auto& elem : value)
			{
				counter++;
				if (!aabb.collide(elem.binding.aabb))
					continue;

				if (!uniqueObjects.contains(elem.binding.objectId))
				{
					uniqueObjects.insert(elem.binding.objectId);
					result.push_back(elem.binding.objectId);
				}
			}
			
		}

		CORE_INFO("[Grid] AABB Query Counter: {}", counter);

		return result;
	}

	std::vector<int> DynamicGrid::queryRay(const Vector2& origin, const Vector2& direction, float maxDistance)
	{
		std::vector<int> result;

		if (m_usedCells.empty())
			return result;


		return result;
	}

	void DynamicGrid::incrementalUpdate(const BroadphaseObjectBinding& binding)
	{
		int index = -1;
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i].binding.objectId == binding.objectId)
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			CORE_WARN("Object with ID: {} does not exist in the grid", binding.objectId);
			return;
		}

		AABB aabb = binding.aabb;
		Vector2 topLeft = aabb.topLeft();
		Vector2 bottomRight = aabb.bottomRight();

		uint32_t rowStart = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - topLeft.y) / m_cellHeight));
		uint32_t rowEnd = static_cast<uint32_t>(std::floor((m_gridTopLeft.y - bottomRight.y) / m_cellHeight));
		uint32_t colStart = static_cast<uint32_t>(std::floor((topLeft.x - m_gridTopLeft.x) / m_cellWidth));
		uint32_t colEnd = static_cast<uint32_t>(std::floor((bottomRight.x - m_gridTopLeft.x) / m_cellWidth));

		std::vector<CellPosition> newCells;

		for (uint32_t i = rowStart; i <= rowEnd; i++)
		{
			for (uint32_t j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos;
				cellPos.row = i;
				cellPos.col = j;

				newCells.push_back(cellPos);

			}
		}



	}
}
