#pragma once

#include "ST2D/Core.h"

namespace ST
{

	using SubGraph = std::unordered_map<ObjectID, std::vector<ObjectID>>;

	class ST_API ObjectGraph
	{
	public:


		void addEnableColorRepeated(ObjectID id);

		// require there is no duplicate pairs
		void buildGraph(const std::vector<ObjectPair>& edges);

		void clearGraph();
		

	//private:

		void addToUF(ObjectID id);
		ObjectID findUF(ObjectID id);
		void unionUF(ObjectID id1, ObjectID id2);

		std::unordered_map<ObjectID, ObjectID> m_unionFind;
		std::unordered_map<ObjectID, int> m_rank;
		std::set<ObjectID> m_roots;

		std::unordered_map<ObjectID, std::set<ObjectPair>> m_nodeToEdges;
		std::unordered_map<ObjectPair, int, ObjectPairHash> m_edgeToColor;
		std::unordered_map<int, std::vector<ObjectPair>> m_colorToEdges;

		std::unordered_set<ObjectID> m_enableColorRepeated;
		std::unordered_map<ObjectID, bool> m_visited;
	};
}