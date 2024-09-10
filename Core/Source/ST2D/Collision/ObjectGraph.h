#pragma once

#include "ST2D/Core.h"

namespace ST
{
	struct ST_API Island
	{
		std::vector<ObjectPair> pairs;
	};

	class ST_API ObjectGraph
	{
	public:

		// require there is no duplicate pairs
		void buildGraph(const std::vector<ObjectPair>& pairs);
		void clearGraph();

		void addToUF(ObjectID id);
		ObjectID findUF(ObjectID id);
		void unionUF(ObjectID id1, ObjectID id2);

	private:
		std::unordered_map<ObjectID, std::vector<ObjectID>> m_graph;
		std::vector<Island> m_islands;
		std::unordered_map<ObjectID, ObjectID> m_unionFind;
		std::unordered_map<ObjectID, int> m_rank;
		std::unordered_map<ObjectID, std::vector<ObjectID>> m_islandGraph;
	};
}