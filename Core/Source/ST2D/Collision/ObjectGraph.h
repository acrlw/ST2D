#pragma once

#include "ST2D/Core.h"

namespace ST
{
	struct GraphNode
	{
		ObjectID id;
		ObjectID ufParent;
		int rank = 1;
		std::set<ObjectPair> edges;
		bool visited = false;
	};

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

		std::unordered_map<ObjectID, GraphNode> m_nodes;
		std::set<ObjectID> m_roots;

		std::unordered_map<ObjectPair, int, ObjectPairHash> m_edgeToColor;
		std::map<int, std::vector<ObjectPair>> m_colorToEdges;

		std::unordered_set<ObjectID> m_enableColorRepeated;

	};
}