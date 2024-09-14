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
			bool enableRepeatedA = m_enableColorRepeated.contains(edge.objectIdA);
			bool enableRepeatedB = m_enableColorRepeated.contains(edge.objectIdB);
			if(!enableRepeatedA)
				addToUF(edge.objectIdA);

			if(!enableRepeatedB)
				addToUF(edge.objectIdB);

			if (!enableRepeatedA && !enableRepeatedB)
				unionUF(edge.objectIdA, edge.objectIdB);

			//prepare for edge coloring
			m_nodeToEdges[edge.objectIdA].insert(edge);
			m_nodeToEdges[edge.objectIdB].insert(edge);
			m_edgeToColor[edge] = -1;

			m_visited[edge.objectIdA] = false;
			m_visited[edge.objectIdB] = false;
		}

		for (const auto& edge : edges)
		{
			bool enableRepeatedA = m_enableColorRepeated.contains(edge.objectIdA);
			bool enableRepeatedB = m_enableColorRepeated.contains(edge.objectIdB);

			ObjectID root = -1;

			if (!enableRepeatedA)
				root = findUF(edge.objectIdA);
			else if (!enableRepeatedB)
				root = findUF(edge.objectIdB);
			else if (enableRepeatedA && enableRepeatedB) // this should not happen
				__debugbreak();

			if (!m_roots.contains(root))
				m_roots.insert(root);
		}

		for(const auto& root: m_roots)
		{
			std::vector<ObjectID> nodeStack;
			std::set<int> usedColors;
			nodeStack.push_back(root);

			for (const auto& repeated : m_enableColorRepeated)
				nodeStack.push_back(repeated);

			while (!nodeStack.empty())
			{
				ObjectID currentNode = nodeStack.back();
				nodeStack.pop_back();

				usedColors.clear();

				if (m_visited[currentNode])
					continue;

				m_visited[currentNode] = true;

				const auto& nodeEdges = m_nodeToEdges[currentNode];

				for (const auto& edge : nodeEdges)
				{
					if (m_edgeToColor[edge] == -1)
					{
						bool enableRepeatedA = m_enableColorRepeated.contains(edge.objectIdA);
						bool enableRepeatedB = m_enableColorRepeated.contains(edge.objectIdB);

						if (!enableRepeatedA)
						{
							for (const auto& edgeA : m_nodeToEdges[edge.objectIdA])
							{
								if (edgeA.key == edge.key)
									continue;

								if (m_edgeToColor[edgeA] != -1)
									usedColors.insert(m_edgeToColor[edgeA]);
							}
						}

						if (!enableRepeatedB)
						{
							for (const auto& edgeB : m_nodeToEdges[edge.objectIdB])
							{
								if (edgeB.key == edge.key)
									continue;

								if (m_edgeToColor[edgeB] != -1)
									usedColors.insert(m_edgeToColor[edgeB]);
							}
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
				}

				for (const auto& nextEdge : m_nodeToEdges[currentNode])
				{
					if (nextEdge.objectIdA == currentNode)
						nodeStack.push_back(nextEdge.objectIdB);
					else
						nodeStack.push_back(nextEdge.objectIdA);
				}
			}

		}


		CORE_INFO("Island count: {0}", m_roots.size());
		for(const auto& root: m_roots)
		{
			CORE_INFO("Root ID: ({})", root);
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
		m_colorToEdges.clear();
		m_edgeToColor.clear();
		m_nodeToEdges.clear();
		m_enableColorRepeated.clear();
		m_visited.clear();
		m_roots.clear();
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
				if (root1 < root2)
				{
					m_unionFind[root2] = root1;
					m_rank[root1]++;
				}
				else
				{
					m_unionFind[root1] = root2;
					m_rank[root2]++;
				}
			}
		}
	}
}
