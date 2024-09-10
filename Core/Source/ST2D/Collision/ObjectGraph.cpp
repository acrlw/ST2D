#include "ObjectGraph.h"

namespace ST
{
	void ObjectGraph::buildGraph(const std::vector<ObjectPair>& pairs)
	{
		clearGraph();
		for (const ObjectPair& pair : pairs)
		{
			m_graph[pair.objectIdA].push_back(pair.objectIdB);
			m_graph[pair.objectIdB].push_back(pair.objectIdA);
			//find union
			addToUF(pair.objectIdA);
			addToUF(pair.objectIdB);
			unionUF(pair.objectIdA, pair.objectIdB);
		}
		// compute islands

	}



	void ObjectGraph::clearGraph()
	{
		m_graph.clear();
		m_islands.clear();
		m_unionFind.clear();
	}

	void ObjectGraph::addToUF(ObjectID id)
	{
		if (!m_unionFind.contains(id))
		{
			m_unionFind[id] = id;
			m_rank[id] = 1;
		}
	}

	ObjectID ObjectGraph::findUF(ObjectID id)
	{
		if (m_unionFind[id] != id)
		{
			m_unionFind[id] = findUF(m_unionFind[id]);
		}
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
