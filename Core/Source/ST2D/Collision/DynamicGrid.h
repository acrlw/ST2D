#pragma once

#include "ST2D/Shape/AABB.h"

#include "AbstractBroadphase.h"
#include <set>

namespace ST
{
	using CellIndex = int32_t;
	using CellKey = int64_t;

	union ST_API CellPosition
	{
		struct
		{
			CellIndex row;
			CellIndex col;
		};
		CellKey key = 0;

		CellPosition(const CellKey& k) : key(k) {}

		CellPosition(const CellIndex& r = 0, const CellIndex& c = 0) : row(r), col(c) {}

		bool operator==(const CellPosition& other) const { return key == other.key; }

		bool operator<(const CellPosition& other) const  { return key < other.key; }

		bool operator>(const CellPosition& other) const  { return key > other.key; }

		bool operator<=(const CellPosition& other) const { return key <= other.key; }

		bool operator>=(const CellPosition& other) const { return key >= other.key; }
	};

	struct CellPositionHash
	{
		std::size_t operator()(const CellPosition& p) const
		{
			return static_cast<std::size_t>(p.key);
		}
	};

	struct ST_API GridObjectBinding
	{
		BroadphaseObjectBinding binding;
		//which cells this object is in
		std::set<CellPosition> cells;
	};


	class ST_API DynamicGrid : AbstractBroadphase
	{
	public:

		using GridCellObjectsList = std::vector<GridObjectBinding>;

		void clearAllObjects() override;
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(const BroadphaseObjectBinding& binding) override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;

		void getGridIndicesFromAABB(const AABB& aabb, CellIndex& rowStart, CellIndex& rowEnd, CellIndex& colStart, CellIndex& colEnd) const;
		void getGridIndicesFromAABB(const AABB& aabb, CellPosition& start, CellPosition& end) const;
		void getGridIndicesFromVector(const Vector2& position, CellIndex& row, CellIndex& col) const;
		void getGridIndicesFromVector(const Vector2& position, CellPosition& cell) const;
		void getVectorFromGridIndices(const CellIndex& row, const CellIndex& col, Vector2& position) const;
		void getVectorFromGridIndices(const CellPosition& cell, Vector2& position) const;

		Vector2 gridShift() const { return m_gridShift; }
		void setGridShift(const Vector2& shift) { m_gridShift = shift; }

		real cellWidth() const { return m_cellWidth; }
		real cellHeight() const { return m_cellHeight; }

		const std::map<CellPosition, GridCellObjectsList>& usedCells() const { return m_usedCells; }

		private:


		void incrementalUpdate(const BroadphaseObjectBinding& binding);

		real m_cellWidth = 1.0f;
		real m_cellHeight = 1.0f;

		Vector2 m_gridShift;

		std::map<CellPosition, GridCellObjectsList> m_usedCells;
		std::vector<GridObjectBinding> m_objects;
	};
}
