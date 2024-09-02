#pragma once

#include "ST2D/Core.h"

namespace ST
{
	struct Island
	{
		std::vector<ObjectPair> pairs;

	};

	class ObjectGraph
	{
	public:

		void buildGraph(const std::vector<ObjectPair>& pairs);


	private:
	};
}