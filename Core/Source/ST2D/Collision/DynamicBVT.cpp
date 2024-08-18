#include "DynamicBVT.h"

#include "ST2D/Log.h"

namespace ST
{
	void DynamicBVT::addObject(const BroadphaseObjectBinding& binding)
	{
		//check if the object is already in the tree
		for(auto&& elem: m_leaves)
		{
			if (elem.binding.objectId == binding.objectId)
			{
				//update the object
				CORE_ASSERT(false, "Object {} is duplicated in the tree", binding.objectId);
				return;
			}
		}

		BVTNodeBinding nodeBinding;
		nodeBinding.binding = binding;
		insertLeaf(nodeBinding);
	}

	void DynamicBVT::removeObject(int objectId)
	{
		removeLeaf(objectId);
	}

	void DynamicBVT::updateObject(int objectId, const AABB& aabb)
	{
		updateLeaf(objectId, aabb);
	}

	void DynamicBVT::clearAllObjects()
	{
		m_nodes.clear();
		m_leaves.clear();
		m_freeNodes.clear();
		m_freeLeaves.clear();
		m_rootIndex = -1;
	}

	std::vector<ObjectPair> DynamicBVT::queryOverlaps()
	{
		std::vector<ObjectPair> result;
		return result;
	}

	std::vector<int> DynamicBVT::queryAABB(const AABB& aabb)
	{
		std::vector<int> result;

		return result;
	}

	std::vector<int> DynamicBVT::queryRay(const Vector2& origin, const Vector2& direction, float maxDistance)
	{
		std::vector<int> result;
		return result;
	}

	void DynamicBVT::insertLeaf(const BVTNodeBinding& leaf)
	{
		int leafIndex = getNewLeafNode();
		m_leaves[leafIndex] = leaf;
		int newNodeIndex = getNewNode();
		m_nodes[newNodeIndex].aabb = leaf.binding.aabb;
		m_leaves[leafIndex].nodeIndex = newNodeIndex;
		m_nodes[newNodeIndex].leafIndex = leafIndex;

		if (m_rootIndex == -1)
		{
			//no root node, new leaf is the root
			m_rootIndex = newNodeIndex;
		}
		else if (m_rootIndex == 0)
		{
			//root is a leaf, create a new root, merge the two leaves
			int mergeIndex = mergeTwoNodes(newNodeIndex, m_rootIndex);
			m_rootIndex = mergeIndex;
		}
		else
		{
			//root is an internal node, insert the new leaf into the tree
			int targetIndex = findBestNode(newNodeIndex);
			int parentIndex = m_nodes[targetIndex].parent;
			int mergeIndex = mergeTwoNodes(newNodeIndex, targetIndex);
			if (m_nodes[parentIndex].left == targetIndex)
				m_nodes[parentIndex].left = mergeIndex;
			else
				m_nodes[parentIndex].right = mergeIndex;
			m_nodes[mergeIndex].parent = parentIndex;


		}
	}

	void DynamicBVT::removeLeaf(int objectId)
	{

	}

	void DynamicBVT::updateLeaf(int objectId, const AABB& aabb)
	{

	}

	int DynamicBVT::findBestNode(int nodeIndex) const
	{
		return greedyFindBestNode(nodeIndex);
	}

	int DynamicBVT::greedyFindBestNode(int nodeIndex) const
	{
		int currentIndex = m_rootIndex;
		while(m_nodes[currentIndex].height > 0)
		{
			real leftCost = AABB::combine(m_nodes[m_nodes[currentIndex].left].aabb, m_nodes[nodeIndex].aabb).surfaceArea();
			real rightCost = AABB::combine(m_nodes[m_nodes[currentIndex].right].aabb, m_nodes[nodeIndex].aabb).surfaceArea();
			if (leftCost < rightCost)
				currentIndex = m_nodes[currentIndex].left;
			else if (leftCost > rightCost)
				currentIndex = m_nodes[currentIndex].right;
			else
			{
				real leftDist = (m_nodes[m_nodes[currentIndex].left].aabb.position - m_nodes[nodeIndex].aabb.position).lengthSquare();
				real rightDist = (m_nodes[m_nodes[currentIndex].right].aabb.position - m_nodes[nodeIndex].aabb.position).lengthSquare();
				if (leftDist < rightDist)
					currentIndex = m_nodes[currentIndex].left;
				else
					currentIndex = m_nodes[currentIndex].right;
			}
		}
		return currentIndex;
	}

	int DynamicBVT::fullFindBestNode(int nodeIndex) const
	{
		int targetIndex = -1;
		real minCost = std::numeric_limits<real>::max();
		for(auto&& leaf:m_leaves)
		{
			bool validLeaf = leaf.nodeIndex != -1 && leaf.nodeIndex != nodeIndex;
			if(!validLeaf)
				continue;

			int currentIndex = leaf.nodeIndex;
			real nodeArea = m_nodes[currentIndex].aabb.surfaceArea();
			real cost = nodeArea;
			while (true)
			{
				real currentArea = m_nodes[currentIndex].aabb.surfaceArea();
				real combineArea = AABB::combine(m_nodes[currentIndex].aabb, m_nodes[nodeIndex].aabb).surfaceArea();
				cost += combineArea - currentArea;

				bool reachTop = currentIndex == m_rootIndex;
				if (reachTop)
					break;

				currentIndex = m_nodes[currentIndex].parent;
			}

			if (minCost > cost)
			{
				minCost = cost;
				targetIndex = leaf.nodeIndex;
			}
		}

		return targetIndex;
	}

	int DynamicBVT::getNewNode()
	{
		if (m_freeNodes.empty())
		{
			BVTNode node;
			node.valid = true;
			m_nodes.push_back(node);
			return m_nodes.size() - 1;
		}

		int result = m_freeNodes.back();
		m_freeNodes.pop_back();
		return result;
	}

	void DynamicBVT::freeNode(int nodeIndex)
	{
		m_nodes[nodeIndex].valid = false;
		m_freeNodes.push_back(nodeIndex);
	}

	int DynamicBVT::getNewLeafNode()
	{
		if (m_freeLeaves.empty())
		{
			BVTNodeBinding leaf;
			m_leaves.push_back(leaf);
			return m_leaves.size() - 1;
		}
		int result = m_freeLeaves.back();
		m_freeLeaves.pop_back();
		return result;
	}

	void DynamicBVT::freeLeafNode(int nodeIndex)
	{
		m_leaves[nodeIndex].nodeIndex = -1;
		m_freeLeaves.push_back(nodeIndex);
	}

	int DynamicBVT::mergeTwoNodes(int nodeIndexA, int nodeIndexB)
	{
		int newIndex = getNewNode();
		m_nodes[nodeIndexA].parent = newIndex;
		m_nodes[nodeIndexB].parent = newIndex;
		m_nodes[newIndex].left = nodeIndexA;
		m_nodes[newIndex].right = nodeIndexB;
		m_nodes[newIndex].aabb = AABB::combine(m_nodes[nodeIndexA].aabb, m_nodes[nodeIndexB].aabb);
		m_nodes[newIndex].height = 1 + std::max(m_nodes[nodeIndexA].height, m_nodes[nodeIndexB].height);
		return newIndex;
	}

	void DynamicBVT::rotateNode(int nodeIndex)
	{
		if (m_nodes[nodeIndex].height < 2)
			return;
		

	}
}
