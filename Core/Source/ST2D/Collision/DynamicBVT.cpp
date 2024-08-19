#include "DynamicBVT.h"

#include <deque>

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

	void DynamicBVT::rebuildTree()
	{
		m_rootIndex = -1;
		m_nodes.clear();
		m_freeNodes.clear();

	}

	void DynamicBVT::recomputeHeightAndAABB(int nodeIndex)
	{
		CORE_ASSERT(nodeIndex >= 0, "Invalid node index");

		int currentIndex = m_nodes[nodeIndex].parent;
		while (currentIndex != -1)
		{
			int leftIndex = m_nodes[currentIndex].left;
			int rightIndex = m_nodes[currentIndex].right;
			m_nodes[currentIndex].height = 1 + std::max(m_nodes[leftIndex].height, m_nodes[rightIndex].height);
			m_nodes[currentIndex].aabb = AABB::combine(m_nodes[leftIndex].aabb, m_nodes[rightIndex].aabb);
			currentIndex = m_nodes[currentIndex].parent;
		}
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

			(m_nodes[parentIndex].left == targetIndex ? m_nodes[parentIndex].left : m_nodes[parentIndex].right) = mergeIndex;

			m_nodes[mergeIndex].parent = parentIndex;
			m_nodes[parentIndex].height = 1 + std::max(m_nodes[m_nodes[parentIndex].left].height, m_nodes[m_nodes[parentIndex].right].height);

			rotateNode(newNodeIndex);

			recomputeHeightAndAABB(newNodeIndex);
		}
	}

	void DynamicBVT::removeLeaf(int objectId)
	{
		int leafIndex = -1;
		int leafNodeIndex = -1;
		for (int i = 0; i < m_leaves.size(); ++i)
		{
			if (m_leaves[i].binding.objectId == objectId)
			{
				leafIndex = i;
				leafNodeIndex = m_leaves[i].nodeIndex;
				break;
			}
		}

		if (leafNodeIndex == -1)
			return;

		if (leafNodeIndex == m_rootIndex)
		{
			//root is the leaf
			m_rootIndex = -1;
			freeNode(leafNodeIndex);
			freeLeafNode(leafIndex);
			return;
		}

		int parentIndex = m_nodes[leafNodeIndex].parent;
		int siblingIndex = m_nodes[parentIndex].left == leafNodeIndex ? m_nodes[parentIndex].right : m_nodes[parentIndex].left;

		if (parentIndex == m_rootIndex)
		{
			//root is the parent of the leaf
			m_rootIndex = siblingIndex;
			freeNode(leafNodeIndex);
			freeNode(parentIndex);
			freeLeafNode(leafIndex);
			return;
		}

		int grandParentIndex = m_nodes[parentIndex].parent;

		(m_nodes[grandParentIndex].left == parentIndex ? m_nodes[grandParentIndex].left : m_nodes[grandParentIndex].right) = siblingIndex;

		m_nodes[siblingIndex].parent = grandParentIndex;

		recomputeHeightAndAABB(siblingIndex);

		freeNode(leafNodeIndex);
		freeLeafNode(leafIndex);
	}

	void DynamicBVT::updateLeaf(int objectId, const AABB& aabb)
	{
		int nodeIndex = -1;
		for(int i = 0; i < m_leaves.size(); ++i)
		{
			if(m_leaves[i].binding.objectId == objectId)
			{
				nodeIndex = m_leaves[i].nodeIndex;
				m_leaves[i].binding.aabb = aabb;
				break;
			}
		}
		m_nodes[nodeIndex].aabb = aabb;
		recomputeHeightAndAABB(nodeIndex);
	}

	int DynamicBVT::findBestNode(int nodeIndex) const
	{
		return fullFindBestNode(nodeIndex);
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

				if (currentIndex == m_rootIndex)
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
		m_nodes[nodeIndex].reset();
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
		m_leaves[nodeIndex].reset();
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
		int F = nodeIndex;
		int D = m_nodes[nodeIndex].parent;
		int B = m_nodes[D].parent;

		if(B == m_rootIndex)
			return;

		int A = m_nodes[B].parent;
		int G = m_nodes[D].right;

		CORE_ASSERT(F != G, "Invalid Index");

		bool isLeft1 = m_nodes[A].left == B;
		bool isLeft2 = m_nodes[B].left == D;

		int C = isLeft1 ? m_nodes[A].right : m_nodes[A].left;
		int E = isLeft2 ? m_nodes[B].right : m_nodes[B].left;


		bool unbalanced = std::abs(m_nodes[B].height - m_nodes[C].height) > 1;

		if(!unbalanced)
			return;

		bool minimalTree = A == m_rootIndex;

		int parentA = -1;
		bool isLeftA = false;

		if(!minimalTree)
		{
			parentA = m_nodes[A].parent;
			isLeftA = m_nodes[parentA].left == A;
		}

		if(isLeft1 && isLeft2)
		{
			//LL
			m_nodes[B].right = A;
			m_nodes[A].parent = B;

			m_nodes[A].left = E;
			m_nodes[E].parent = A;

			m_nodes[A].height = 1;
			m_nodes[B].height = 2;

			m_nodes[A].aabb = AABB::combine(m_nodes[E].aabb, m_nodes[C].aabb);
			m_nodes[B].aabb = AABB::combine(m_nodes[A].aabb, m_nodes[D].aabb);

			if (minimalTree)
			{
				m_nodes[B].parent = -1;
				m_rootIndex = B;
			}
			else
			{
				(isLeftA ? m_nodes[parentA].left : m_nodes[parentA].right) = B;
				m_nodes[B].parent = parentA;
				m_nodes[parentA].height = 1 + std::max(m_nodes[m_nodes[parentA].left].height, m_nodes[m_nodes[parentA].right].height);
				m_nodes[parentA].aabb = AABB::combine(m_nodes[m_nodes[parentA].left].aabb, m_nodes[m_nodes[parentA].right].aabb);
			}
		}
		else if(isLeft1 && !isLeft2)
		{
			//LR
			m_nodes[D].right = A;
			m_nodes[A].parent = D;

			m_nodes[D].left = B;
			m_nodes[B].parent = D;

			m_nodes[B].right = F;
			m_nodes[F].parent = B;

			m_nodes[A].left = G;
			m_nodes[G].parent = A;

			m_nodes[B].height = 1;
			m_nodes[A].height = 1;
			m_nodes[D].height = 2;

			m_nodes[A].aabb = AABB::combine(m_nodes[G].aabb, m_nodes[C].aabb);
			m_nodes[B].aabb = AABB::combine(m_nodes[F].aabb, m_nodes[E].aabb);
			m_nodes[D].aabb = AABB::combine(m_nodes[B].aabb, m_nodes[A].aabb);


			if (minimalTree)
			{
				m_nodes[D].parent = -1;
				m_rootIndex = D;
			}
			else
			{
				(isLeftA ? m_nodes[parentA].left : m_nodes[parentA].right) = D;
				m_nodes[D].parent = parentA;
				m_nodes[parentA].height = 1 + std::max(m_nodes[m_nodes[parentA].left].height, m_nodes[m_nodes[parentA].right].height);
			}
		}
		else if(!isLeft1 && isLeft2)
		{
			//RL
			m_nodes[D].left = A;
			m_nodes[A].parent = D;

			m_nodes[D].right = B;
			m_nodes[B].parent = D;

			m_nodes[A].right = F;
			m_nodes[F].parent = A;

			m_nodes[B].left = G;
			m_nodes[G].parent = B;

			m_nodes[A].height = 1;
			m_nodes[B].height = 1;
			m_nodes[D].height = 2;

			m_nodes[A].aabb = AABB::combine(m_nodes[C].aabb, m_nodes[F].aabb);
			m_nodes[B].aabb = AABB::combine(m_nodes[E].aabb, m_nodes[G].aabb);
			m_nodes[D].aabb = AABB::combine(m_nodes[A].aabb, m_nodes[B].aabb);

			if(minimalTree)
			{
				
			}
		}
		else
		{
			//RR

		}

	}
}
