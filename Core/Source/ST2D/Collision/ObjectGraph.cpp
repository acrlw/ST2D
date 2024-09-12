#include "ObjectGraph.h"

#include "ST2D/Log.h"

namespace ST
{
	void ObjectGraph::buildGraph(const std::vector<ObjectPair>& pairs)
	{
		for (const ObjectPair& pair : pairs)
		{
			//find union
			addToUF(pair.objectIdA);
			addToUF(pair.objectIdB);
			unionUF(pair.objectIdA, pair.objectIdB);
		}

		for (const auto& pair : pairs)
		{
			ObjectID root = findUF(pair.objectIdA);
			auto& subgraph = m_subGraph[root];
			subgraph[pair.objectIdA].push_back(pair.objectIdB);
			subgraph[pair.objectIdB].push_back(pair.objectIdA);
		}

		//for (const auto& key : m_unionFind | std::views::keys)
		//{
		//	ObjectID root = findUF(key);
		//	m_islandGraph[root].push_back(key);
		//}

		CORE_INFO("Island count: {0}", m_subGraph.size());
		//for (const auto& [key, value] : m_islandGraph)
		//{
		//	std::string result = std::format("Island ObjectID ({0}) : ", key);
		//	for (const auto& id : value)
		//		result += std::format("({}, {}) ", id.objectIdA, id.objectIdB);
		//	CORE_INFO(result);
		//}

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

		// edge coloring



	}



	void ObjectGraph::clearGraph()
	{
		m_rank.clear();
		m_unionFind.clear();
		m_subGraph.clear();
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
