#pragma once

#include "ST2D/Geometry/Shape/AABB.h"

namespace ST
{
	//TODO 20220704
	//1. Incremental Update Bodies, calculating the different between two cellList
	//2. Position combine to u64 and split into two u32
	//3. Raycast query bodies
	class ST_API UniformGrid
	{
	public:
		UniformGrid(const real& width = 400.0f, const real& height = 400.0f, uint32_t rows = 400,
			uint32_t columns = 400);
		std::vector<std::pair<ShapePrimitive*, ShapePrimitive*>> generate();
		std::vector<ShapePrimitive*> raycast(const Vector2& p, const Vector2& d);

		void updateAll();
		void update(ShapePrimitive* body);
		void insert(ShapePrimitive* body);
		void remove(ShapePrimitive* body);
		void clearAll();
		std::vector<ShapePrimitive*> query(const AABB& aabb);


		int rows() const;
		void setRows(const int& size);

		int columns() const;
		void setColumns(const int& size);

		real width() const;
		void setWidth(const real& size);

		real height() const;
		void setHeight(const real& size);

		struct Position
		{
			Position() = default;

			Position(const uint32_t& _x, const uint32_t& _y) : x(_x), y(_y)
			{
			}

			uint32_t x = 0;
			uint32_t y = 0;

			bool operator<(const Position& rhs) const
			{
				if (x < rhs.x)
					return true;
				if (x == rhs.x)
					return y < rhs.y;
				return false;
			}

			bool operator>(const Position& rhs) const
			{
				if (x > rhs.x)
					return true;
				if (x == rhs.x)
					return y > rhs.y;
				return false;
			}

			bool operator==(const Position& rhs) const
			{
				return x == rhs.x && y == rhs.y;
			}
		};

		//AABB query cells
		std::vector<Position> queryCells(const AABB& aabb);

		//ray cast query cells
		std::vector<Position> queryCells(const Vector2& start, const Vector2& direction);
		real cellHeight() const;
		real cellWidth() const;

		std::map<Position, std::vector<ShapePrimitive*>> m_cellsToBodies;
		std::map<ShapePrimitive*, std::vector<Position>> m_bodiesToCells;

		void fullUpdate(ShapePrimitive* body);
		void incrementalUpdate(ShapePrimitive* body);

	private:
		enum class Operation
		{
			Add,
			Delete
		};

		void updateGrid();
		void changeGridSize();
		void updateBodies();
		std::vector<std::pair<Operation, Position>> compareCellList(
			const std::vector<Position>& oldCellList, const std::vector<Position>& newCellList);
		real m_width = 100.0f;
		real m_height = 100.0f;
		uint32_t m_rows = 200;
		uint32_t m_columns = 200;


		real m_cellWidth = 0.0f;
		real m_cellHeight = 0.0f;
	};
}
