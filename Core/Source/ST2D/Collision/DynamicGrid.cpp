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

		CellIndex rowStart, rowEnd, colStart, colEnd;

		getGridIndicesFromAABB(binding.aabb, rowStart, rowEnd, colStart, colEnd);

		GridObjectBinding objectBinding;
		objectBinding.binding = binding;

		for (CellIndex i = rowStart; i <= rowEnd; i++)
		{
			for (CellIndex j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos(i, j);

				objectBinding.cells.insert(cellPos);

			}
		}

		m_objects.push_back(objectBinding);

		for (CellIndex i = rowStart; i <= rowEnd; i++)
		{
			for (CellIndex j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos(i, j);

				m_usedCells[cellPos].push_back(objectBinding);
			}
		}


		//CORE_INFO("ID: {}, Row:({}, {}), Col:({}, {})", binding.objectId,
		//	rowStart, rowEnd, colStart, colEnd);

	}

	void DynamicGrid::removeObject(int objectId)
	{
		std::set<CellPosition> cells;
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

		int32_t rowStart, rowEnd, colStart, colEnd;

		getGridIndicesFromAABB(aabb, rowStart, rowEnd, colStart, colEnd);

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

		int counter = 0;

		Vector2 start = origin;
		Vector2 end = origin + direction * maxDistance;

		CellIndex rStart, rEnd, cStart, cEnd;

		getGridIndicesFromVector(start, rStart, cStart);
		getGridIndicesFromVector(end, rEnd, cEnd);

		CellIndex dx = rEnd - rStart;
		CellIndex dy = cEnd - cStart;

		if (rStart == rEnd && cStart == cEnd)
		{
			//ray is in a single cell
			CellPosition cellPos(rStart, cStart);

			if (!m_usedCells.contains(cellPos))
				return result;

			for (auto&& elem : m_usedCells[cellPos])
			{
				if (elem.binding.aabb.raycast(start, direction))
					result.push_back(elem.binding.objectId);
			}

			return result;
		}

		std::vector<CellPosition> rayCells;



		CORE_INFO("[Grid] Ray Query Counter: {}", counter);

		return result;
	}

	void DynamicGrid::getGridIndicesFromAABB(const AABB& aabb, CellIndex& rowStart, CellIndex& rowEnd, CellIndex& colStart,
		CellIndex& colEnd) const
	{
		Vector2 bottomLeft = aabb.bottomLeft();
		Vector2 topRight = aabb.topRight();

		rowStart = static_cast<CellIndex>(std::floor((bottomLeft.y - m_gridShift.y) / m_cellHeight));
		rowEnd = static_cast<CellIndex>(std::floor((topRight.y - m_gridShift.y) / m_cellHeight));
		colStart = static_cast<CellIndex>(std::floor((bottomLeft.x - m_gridShift.x) / m_cellWidth));
		colEnd = static_cast<CellIndex>(std::floor((topRight.x - m_gridShift.x) / m_cellWidth));
	}

	void DynamicGrid::getGridIndicesFromAABB(const AABB& aabb, CellPosition& start, CellPosition& end) const
	{
		Vector2 bottomLeft = aabb.bottomLeft();
		Vector2 topRight = aabb.topRight();

		start.row = static_cast<CellIndex>(std::floor((bottomLeft.y - m_gridShift.y) / m_cellHeight));
		end.row = static_cast<CellIndex>(std::floor((topRight.y - m_gridShift.y) / m_cellHeight));
		start.col = static_cast<CellIndex>(std::floor((bottomLeft.x - m_gridShift.x) / m_cellWidth));
		end.col = static_cast<CellIndex>(std::floor((topRight.x - m_gridShift.x) / m_cellWidth));
	}

	void DynamicGrid::getGridIndicesFromVector(const Vector2& position, CellIndex& row, CellIndex& col) const
	{
		row = static_cast<CellIndex>(std::floor((position.y - m_gridShift.y) / m_cellHeight));
		col = static_cast<CellIndex>(std::floor((position.x - m_gridShift.x) / m_cellWidth));
	}

	void DynamicGrid::getGridIndicesFromVector(const Vector2& position, CellPosition& cell) const
	{
		cell.row = static_cast<CellIndex>(std::floor((position.y - m_gridShift.y) / m_cellHeight));
		cell.col = static_cast<CellIndex>(std::floor((position.x - m_gridShift.x) / m_cellWidth));
	}

	void DynamicGrid::getVectorFromGridIndices(const CellIndex& row, const CellIndex& col, Vector2& position) const
	{
		position.x = m_gridShift.x + col * m_cellWidth;
		position.y = m_gridShift.y + row * m_cellHeight;
	}

	void DynamicGrid::getVectorFromGridIndices(const CellPosition& cell, Vector2& position) const
	{
		position.x = m_gridShift.x + cell.col * m_cellWidth;
		position.y = m_gridShift.y + cell.row * m_cellHeight;
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

		m_objects[index].binding = binding;

		AABB aabb = binding.aabb;

		CellIndex rowStart, rowEnd, colStart, colEnd;

		getGridIndicesFromAABB(aabb, rowStart, rowEnd, colStart, colEnd);

		std::set<CellPosition> newCells;

		for (CellIndex i = rowStart; i <= rowEnd; i++)
		{
			for (CellIndex j = colStart; j <= colEnd; j++)
			{
				CellPosition cellPos(i, j);

				newCells.insert(cellPos);

			}
		}

		// double pointer to compute symmetric difference

		std::vector<CellPosition> cellsToRemove, cellsToAdd;

		auto it1 = m_objects[index].cells.begin();
		auto it2 = newCells.begin();

		while (it1 != m_objects[index].cells.end() && it2 != newCells.end())
		{
			if (*it1 < *it2)
			{
				cellsToRemove.emplace_back(*it1);
				++it1;
			}
			else if (*it2 < *it1)
			{
				cellsToAdd.emplace_back(*it2);
				++it2;
			}
			else
			{
				++it1;
				++it2;
			}
		}

		while (it1 != m_objects[index].cells.end())
		{
			cellsToRemove.emplace_back(*it1);
			++it1;
		}

		while (it2 != newCells.end())
		{
			cellsToAdd.emplace_back(*it2);
			++it2;
		}

		for (auto&& elem : cellsToRemove)
		{

			if (!m_usedCells.contains(elem))
				continue;

			std::erase_if(m_usedCells[elem],
				[&binding](const GridObjectBinding& bd)
				{
					return binding.objectId == bd.binding.objectId;
				});
		}

		for (auto&& elem : cellsToAdd)
		{
			m_usedCells[elem].push_back(m_objects[index]);
		}

		m_objects[index].cells = std::move(newCells);

	}
}
