#pragma once

#include "ST2D/Algorithms/Algorithm2D.h"

namespace ST
{
	struct ST_API VertexIndexPair
	{
		Vector2 vertex;
		Index index;
	};

	struct ST_API SimplexVertex
	{
		SimplexVertex() = default;

		SimplexVertex(const Vector2& point_a, const Vector2& point_b, const Index& index_a = UINT32_MAX,
			const Index& index_b = UINT32_MAX)
		{
			point[0] = point_a;
			point[1] = point_b;
			result = point[0] - point[1];
			index[0] = index_a;
			index[1] = index_b;
		}

		bool operator ==(const SimplexVertex& rhs) const
		{
			return point[0] == rhs.point[0] && point[1] == rhs.point[1];
		}

		bool operator !=(const SimplexVertex& rhs) const
		{
			return !(point[0] == rhs.point[0] && point[1] == rhs.point[1]);
		}

		bool isEmpty() const
		{
			return point[0].isOrigin() && point[1].isOrigin() && result.isOrigin() && index[0] == UINT32_MAX && index[1]
				== UINT32_MAX;
		}

		bool isIndexAValid() const
		{
			return index[0] != UINT32_MAX;
		}

		bool isIndexBValid() const
		{
			return index[1] != UINT32_MAX;
		}

		void clear()
		{
			point[0].clear();
			point[1].clear();
			result.clear();
			index[0] = UINT32_MAX;
			index[1] = UINT32_MAX;
		}

		//point[0] : pointA
		//point[1] : pointB
		Vector2 point[2];
		Vector2 result;
		//for polygon, the index of the vertex
		//index[0] : indexA
		//index[1] : indexB
		Index index[2];
	};

	/**
	 * \brief Simplex structure
	 */
	struct ST_API Simplex
	{
		Simplex() = default;
		std::array<SimplexVertex, 3> vertices;
		size_t count = 0;
		bool isContainOrigin = false;
		bool containsOrigin(bool strict = false) const;
		static bool containOrigin(const Simplex& simplex, bool strict = false);
		bool contains(const SimplexVertex& vertex, const real& epsilon = Constant::GeometryEpsilon)const;
		void addSimplexVertex(const SimplexVertex& vertex);
		void removeByIndex(const Index& index);
		void removeEnd();
		void removeAll();
	};
}
