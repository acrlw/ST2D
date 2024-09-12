#pragma once

#include "ST2D/Core.h"

namespace ST
{
	struct ST_API GraphNode
	{
		ObjectID id;
		bool visited = false;
	};


	struct ST_API ObjectSubGraph
	{
		std::unordered_map<ObjectID, std::vector<ObjectID>> subGraph;
	};

	using SubGraph = std::unordered_map<ObjectID, std::vector<ObjectID>>;

	class ST_API ObjectGraph
	{
	public:


		// require there is no duplicate pairs
		void buildGraph(const std::vector<ObjectPair>& edges);
		void clearGraph();


	//private:

		void addToUF(ObjectID id);
		ObjectID findUF(ObjectID id);
		void unionUF(ObjectID id1, ObjectID id2);

		std::vector<std::vector<ObjectPair>> m_colorGraph;
		std::unordered_map<ObjectID, ObjectID> m_unionFind;
		std::unordered_map<ObjectID, int> m_rank;
		std::unordered_map<ObjectID, SubGraph> m_subGraph;

		std::map<ObjectID, std::vector<ObjectPair>> m_nodeToEdges;
		std::map<ObjectPair, int> m_edgeToColor;
		std::map<int, std::vector<ObjectPair>> m_colorToEdges;
	};
}