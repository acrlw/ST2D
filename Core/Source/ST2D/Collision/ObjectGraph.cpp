#include "ObjectGraph.h"

#include "ST2D/Log.h"

namespace ST
{
	void ObjectGraph::addEnableColorRepeated(ObjectID id)
	{
		if (m_enableColorRepeated.contains(id))
			return;
		m_enableColorRepeated.insert(id);
	}

	void ObjectGraph::buildGraph(const std::vector<ObjectPair>& edges)
	{
		for (const auto& edge : edges)
		{
			//find union

			addToUF(edge.objectIdA);
			addToUF(edge.objectIdB);
			unionUF(edge.objectIdA, edge.objectIdB);

			//prepare for edge coloring
			m_nodeToEdges[edge.objectIdA].push_back(edge);
			m_nodeToEdges[edge.objectIdB].push_back(edge);
			m_edgeToColor[edge] = -1;
		}


		for (const auto& edge : edges)
		{
			ObjectID root = findUF(edge.objectIdA);
			auto& subgraph = m_subGraph[root];
			subgraph[edge.objectIdA].push_back(edge.objectIdB);
			subgraph[edge.objectIdB].push_back(edge.objectIdA);

			//subgraph[edge.objectIdA].push_back(edge.objectIdB);
			//subgraph[edge.objectIdB].push_back(edge.objectIdA);

			// edge coloring
			
		}

		for (const auto& edge : edges)
		{
			std::set<int> usedColors;

			for (const auto& edgeA : m_nodeToEdges[edge.objectIdA])
			{
				if (edgeA.key == edge.key)
					continue;

				if (m_edgeToColor[edgeA] != -1)
					usedColors.insert(m_edgeToColor[edgeA]);
			}

			for (const auto& edgeB : m_nodeToEdges[edge.objectIdB])
			{
				if (edgeB.key == edge.key)
					continue;

				if (m_edgeToColor[edgeB] != -1)
					usedColors.insert(m_edgeToColor[edgeB]);
			}

			int color = 0;
			for (const auto& usedColor : usedColors)
			{
				if (color != usedColor)
					break;
				color++;
			}

			m_edgeToColor[edge] = color;
		}


		CORE_INFO("Island count: {0}", m_subGraph.size());

		for(const auto& [key, value] : m_subGraph)
		{
			std::string result = std::format("SubGraph ObjectID ({0}) : \n", key);
			for (const auto& [id, ids] : value)
			{
				result += std::format("[{0}] : ", id);
				for (const auto& id : ids)
					result += std::format("{0} ", id);

				result += "\n";
			}
			CORE_INFO(result);
		}


		for (const auto& [key, value] : m_edgeToColor)
			m_colorToEdges[value].push_back(key);

		CORE_INFO("Color count: {0}", m_colorToEdges.size());
		for (const auto& [key, value] : m_colorToEdges)
		{
			std::string result = std::format("Color ({0}) : \n", key);
			for (const auto& edge : value)
				result += std::format("({0}, {1}) ", edge.objectIdA, edge.objectIdB);

			CORE_INFO(result);
		}


	}



	void ObjectGraph::clearGraph()
	{
		m_rank.clear();
		m_unionFind.clear();
		m_subGraph.clear();
		m_colorToEdges.clear();
		m_edgeToColor.clear();
		m_nodeToEdges.clear();
		m_enableColorRepeated.clear();
		m_visited.clear();
	}

	void ObjectGraph::addToUF(ObjectID id)
	{
		if (m_unionFind.contains(id))
			return;

		m_unionFind[id] = id;
		m_rank[id] = 1;
	}

	ObjectID ObjectGraph::findUF(ObjectID id)
	{
		if (m_unionFind[id] != id)
			m_unionFind[id] = findUF(m_unionFind[id]);
		
		return m_unionFind[id];
	}

	void ObjectGraph::unionUF(ObjectID id1, ObjectID id2)
	{
		ObjectID root1 = findUF(id1);
		ObjectID root2 = findUF(id2);
		if (root1 != root2)
		{
			if (m_rank[root1] < m_rank[root2])
			{
				m_unionFind[root1] = root2;
			}
			else if (m_rank[root1] > m_rank[root2])
			{
				m_unionFind[root2] = root1;
			}
			else
			{
				m_unionFind[root2] = root1;
				m_rank[root1]++;
			}
		}
	}
}
