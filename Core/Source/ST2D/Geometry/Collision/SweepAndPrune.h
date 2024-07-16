#pragma once

#include "ST2D/Geometry/Shape/AABB.h"

namespace ST
{
	class ST_API SweepAndPrune
	{
	public:

		static std::vector<std::pair<ShapePrimitive*, ShapePrimitive*>> generate(const std::vector<ShapePrimitive*>& bodyList);
		static std::vector<ShapePrimitive*> query(const std::vector<ShapePrimitive*>& bodyList, const AABB& region);
	};
}

