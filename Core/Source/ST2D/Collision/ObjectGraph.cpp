#include "ObjectGraph.h"

#include "ST2D/Log.h"

namespace ST
{
	void ObjectGraph::incrementalUpdateEdges(const std::vector<ObjectPair>& edges)
	{
		if (edges.empty())
			return;

		std::set<ObjectPair> srcEdges;
		for (auto& edge : edges)
			srcEdges.insert(edge);

		auto it1 = m_edges.begin();
		auto it2 = srcEdges.begin();
		std::vector<ObjectPair> newEdges, removedEdges;

		while(it1 != m_edges.end() && it2 != srcEdges.end())
		{
			if (*it1 == *it2)
			{
				++it1;
				++it2;
			}
			else if (*it1 < *it2)
			{
				removedEdges.push_back(*it1);
				++it1;
			}
			else
			{
				newEdges.push_back(*it2);
				++it2;
			}
		}

		while (it1 != m_edges.end())
		{
			removedEdges.emplace_back(*it1);
			++it1;
		}

		while (it2 != srcEdges.end())
		{
			newEdges.emplace_back(*it2);
			++it2;
		}

		for (auto&& edge : newEdges)
			addEdge(edge);

		for (auto&& edge : removedEdges)
			removeEdge(edge);

	}

	void ObjectGraph::addEdge(const ObjectPair& edge)
	{
		//check if the edge is already added
		if (m_edges.contains(edge))
			return;

		std::array enableRepeated = { m_enableColorRepeated.contains(edge.idA), m_enableColorRepeated.contains(edge.idB) };

		if (!enableRepeated[0])
			addToUF(edge.idA);

		if (!enableRepeated[1])
			addToUF(edge.idB);

		if (!enableRepeated[0] && !enableRepeated[1])
			unionUF(edge.idA, edge.idB);

		m_edgeToColor[edge] = -1;

		ObjectID root = -1;

		if (!enableRepeated[0])
			root = findUF(edge.idA);
		else if (!enableRepeated[1])
			root = findUF(edge.idB);

		if (root != -1 && !m_roots.contains(root))
			m_roots.insert(root);

		int color = 0;

		std::set<int> usedColors;

		std::array nodeStack = { edge.idA, edge.idB };

		for(int i = 0;i < 2;++i)
		{
			ObjectID currentNode = nodeStack[i];

			const auto& nodeEdges = m_nodes[currentNode].edges;

			if (!enableRepeated[i])
			{
				for (const auto& nodeEdge : nodeEdges)
				{
					if (m_edgeToColor.contains(nodeEdge) && m_edgeToColor[nodeEdge] != -1)
						usedColors.insert(m_edgeToColor[nodeEdge]);
				}
			}
		}


		for (const auto& usedColor : usedColors)
		{
			if (color != usedColor)
				break;
			color++;
		}

		m_maxColor = std::max(m_maxColor, color);

		m_edgeToColor[edge] = color;

		m_nodes[edge.idA].edges.insert(edge);
		m_nodes[edge.idB].edges.insert(edge);

		m_edges.insert(edge);
	}

	void ObjectGraph::removeEdge(const ObjectPair& edge)
	{
		if (!m_edges.contains(edge))
			return;

		m_edges.erase(edge);

		m_nodes[edge.idA].edges.erase(edge);
		m_nodes[edge.idB].edges.erase(edge);

		m_edgeToColor.erase(edge);
	}

	void ObjectGraph::addEnableColorRepeated(ObjectID id)
	{
		if (m_enableColorRepeated.contains(id))
			return;
		m_enableColorRepeated.insert(id);
	}


	void ObjectGraph::buildGraph(const std::vector<ObjectPair>& edges)
	{
		ZoneScopedN("[ObjectGraph] buildGraph");

		for (const auto& edge : edges)
		{
			//find union
			bool enableRepeatedA = m_enableColorRepeated.contains(edge.idA);
			bool enableRepeatedB = m_enableColorRepeated.contains(edge.idB);

			if(!enableRepeatedA)
				addToUF(edge.idA);

			if(!enableRepeatedB)
				addToUF(edge.idB);

			if (!enableRepeatedA && !enableRepeatedB)
				unionUF(edge.idA, edge.idB);

			//prepare for edge coloring
			m_nodes[edge.idA].edges.insert(edge);
			m_nodes[edge.idB].edges.insert(edge);

			m_edges.insert(edge);

			m_edgeToColor[edge] = -1;
		}

		for (const auto& edge : edges)
		{
			bool enableRepeatedA = m_enableColorRepeated.contains(edge.idA);
			bool enableRepeatedB = m_enableColorRepeated.contains(edge.idB);

			ObjectID root = -1;

			if (!enableRepeatedA)
				root = findUF(edge.idA);
			else if (!enableRepeatedB)
				root = findUF(edge.idB);

			if (root != -1 && !m_roots.contains(root))
				m_roots.insert(root);


			if (enableRepeatedA && enableRepeatedB)
			{
				// this should not happen, such as collision pair of two static bodies 
				CORE_ASSERT(false, "Objects that can have duplicate colors cannot be processed together.");
			}
		}

		int maxColor = 0;

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

				if (m_nodes[currentNode].visited)
					continue;

				m_nodes[currentNode].visited = true;

				const auto& nodeEdges = m_nodes[currentNode].edges;

				for (const auto& edge : nodeEdges)
				{
					if (m_edgeToColor[edge] == -1)
					{
						bool enableRepeatedA = m_enableColorRepeated.contains(edge.idA);
						bool enableRepeatedB = m_enableColorRepeated.contains(edge.idB);

						if (!enableRepeatedA)
						{
							for (const auto& edgeA : m_nodes[edge.idA].edges)
							{
								if (edgeA.key == edge.key)
									continue;

								if (m_edgeToColor[edgeA] != -1)
									usedColors.insert(m_edgeToColor[edgeA]);
							}
						}

						if (!enableRepeatedB)
						{
							for (const auto& edgeB : m_nodes[edge.idB].edges)
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
						maxColor = std::max(maxColor, color);
						m_edgeToColor[edge] = color;

					}
				}

				for (const auto& nextEdge : m_nodes[currentNode].edges)
				{
					if (nextEdge.idA == currentNode)
						nodeStack.push_back(nextEdge.idB);
					else
						nodeStack.push_back(nextEdge.idA);
				}
			}

		}
		m_maxColor = maxColor;
		



	}

	void ObjectGraph::clearGraph()
	{
		m_maxColor = 0;
		m_edges.clear();
		m_nodes.clear();
		m_colorToEdges.clear();
		m_edgeToColor.clear();
		m_enableColorRepeated.clear();
		m_roots.clear();
	}

	void ObjectGraph::outputColorResult()
	{
		m_colorToEdges.clear();
		m_colorToEdges.resize(m_maxColor + 1);
		for (const auto& [key, value] : m_edgeToColor)
			m_colorToEdges[value].push_back(key);
	}

	void ObjectGraph::printGraph()
	{
		CORE_INFO("Island count: {0}", m_roots.size());
		CORE_INFO("Color count: {0}", m_colorToEdges.size());

		for(const auto& root: m_roots)
		{
			CORE_INFO("Root ID: ({})", root);
		}

		for (int i = 0; i < m_colorToEdges.size(); i++)
		{
			std::string result = std::format("Color ({0}) : \n", i);
			for (const auto& edge : m_colorToEdges[i])
				result += std::format("({0}, {1}) ", edge.idA, edge.idB);

			CORE_INFO(result);
		}
	}

	void ObjectGraph::addToUF(ObjectID id)
	{
		if (m_nodes.contains(id))
			return;

		m_nodes[id].ufParent = id;
	}

	ObjectID ObjectGraph::findUF(ObjectID id)
	{
		if(m_nodes[id].ufParent != id)
			m_nodes[id].ufParent = findUF(m_nodes[id].ufParent);
		
		return m_nodes[id].ufParent;
	}

	void ObjectGraph::unionUF(ObjectID id1, ObjectID id2)
	{
		ObjectID root1 = findUF(id1);
		ObjectID root2 = findUF(id2);
		if (root1 != root2)
		{
			if (m_nodes[root1].rank < m_nodes[root2].rank)
			{
				m_nodes[root1].ufParent = root2;
			}
			else if (m_nodes[root1].rank > m_nodes[root2].rank)
			{
				m_nodes[root2].ufParent = root1;
			}
			else
			{
				if (root1 < root2)
				{
					m_nodes[root2].ufParent = root1;
					m_nodes[root1].rank++;
				}
				else
				{
					m_nodes[root1].ufParent = root2;
					m_nodes[root2].rank++;
				}
			}
		}
	}
}
