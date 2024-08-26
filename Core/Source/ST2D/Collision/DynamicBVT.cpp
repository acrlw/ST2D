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
		AABB rootAABB;

		std::vector<BVTNodeBinding> leaves;
		leaves.reserve(m_leaves.size());

		if(m_rootIndex != -1)
		{
			rootAABB = m_nodes[m_rootIndex].aabb;

			m_rootIndex = -1;
			m_nodes.clear();
			m_freeNodes.clear();

			m_rootIndex = getNewNode();
			m_nodes[m_rootIndex].aabb = rootAABB;

			for(int leafIndex = 0; leafIndex < m_leaves.size(); ++leafIndex)
			{
				auto&& leaf = m_leaves[leafIndex];
				if(leaf.isEmpty())
					continue;

				int newNodeIndex = getNewNode();
				m_nodes[newNodeIndex].aabb = leaf.binding.aabb;
				m_leaves[leafIndex].nodeIndex = newNodeIndex;
				m_nodes[newNodeIndex].leafIndex = leafIndex;
				m_nodes[newNodeIndex].parent = m_rootIndex;

				leaves.push_back(leaf);
			}
		}

		if(rootAABB.isEmpty())
		{
			for(auto&& leaf:m_leaves)
			{
				if(!leaf.isValid())
					continue;
				rootAABB.combine(leaf.binding.aabb);
			}

			m_rootIndex = -1;
			m_nodes.clear();
			m_freeNodes.clear();
		}

		int sortIndex = rootAABB.width > rootAABB.height ? 0 : 1;

		std::ranges::sort(leaves, [&sortIndex](const BVTNodeBinding& a, const BVTNodeBinding& b)
		{
			return a.binding.aabb.position[sortIndex] < b.binding.aabb.position[sortIndex];
		});


		//rebuildTreeSplitMid(rootAABB, leaves);
		rebuildTreeSAH(rootAABB, leaves);

	}

	void DynamicBVT::printTree()
	{
		std::stack<std::tuple<int, std::string, bool>> stack;
		stack.push({ m_rootIndex, "", true });
		std::vector<std::string> lines;
		while (!stack.empty())
		{
			auto [index, prefix, isLeft] = stack.top();
			stack.pop();

			if (index == -1)
				continue;

			std::string line = prefix + (isLeft ? "©À©¤©¤ " : "©¸©¤©¤ ") + std::to_string(index);


			std::string newPrefix = prefix + (isLeft ? "©¦   " : "    ");

			if (m_nodes[index].isLeaf())
			{
				int leafIdx = m_nodes[index].leafIndex;
				int objectId = m_leaves[leafIdx].binding.objectId;
				line += "(" + std::to_string(objectId) + ")";
			}
			else
			{
				int height = m_nodes[index].height;
				line += "[" + std::to_string(height) + "]";
			}

			lines.push_back(line);

			stack.push({ m_nodes[index].right, newPrefix, false });
			stack.push({ m_nodes[index].left, newPrefix, true });
		}

		for (auto it = lines.begin(); it != lines.end(); ++it)
			std::cout << *it << "\n";

		std::cout << "--------------------\n";
	}

	void DynamicBVT::rebuildTreeSplitMid(const AABB& rootAABB, const std::vector<BVTNodeBinding>& leaves)
	{
		if(leaves.empty() || rootAABB.isEmpty())
			return;

		int sortIndex = 0;
		int sortIndex2 = 1;
		real boxSize = rootAABB.width;
		real boxSize2 = rootAABB.height;

		if (rootAABB.width > rootAABB.height)
		{
			sortIndex = 1;
			sortIndex2 = 0;
			boxSize = rootAABB.height;
			boxSize2 = rootAABB.width;
		}


		
	}

	void DynamicBVT::rebuildTreeSAH(const AABB& rootAABB, const std::vector<BVTNodeBinding>& leaves)
	{
		if (leaves.empty() || rootAABB.isEmpty())
			return;

		int bucketCount = 8;

		std::stack<std::tuple<int, std::vector<BVTNodeBinding>>> stack;
		stack.push({ m_rootIndex, leaves });

		while(!stack.empty())
		{
			auto [rootIndex, currentLeaves] = stack.top();
			stack.pop();

			AABB currentRootAABB = m_nodes[rootIndex].aabb;

			if(currentLeaves.size() <= 2)
			{
				//just one or two leaves, no need to split
				if(currentLeaves.size() == 2)
				{
					m_nodes[rootIndex].left = currentLeaves[0].nodeIndex;
					m_nodes[rootIndex].right = currentLeaves[1].nodeIndex;
				}

				continue;
			}

			int sortIndex1 = 0;

			Vector2 rootBottomLeft = currentRootAABB.bottomLeft();
			Vector2 rootTopRight = currentRootAABB.topRight();

			if (currentRootAABB.width < currentRootAABB.height)
				sortIndex1 = 1;
			

			real rootArea = currentRootAABB.surfaceArea();

			std::vector<real> leafArea;
			leafArea.reserve(currentLeaves.size());

			real bucketSize = (rootTopRight[sortIndex1] - rootBottomLeft[sortIndex1]) / static_cast<real>(bucketCount);

			std::vector<real> bucketArea;
			std::vector<real> bucketLeafCount;
			std::vector<real> bucketLeftLeafCount;
			std::vector<real> bucketRightLeafCount;

			for (int i = 0; i < bucketCount; ++i)
			{
				bucketArea.push_back(0);
				bucketLeafCount.push_back(0);
				bucketLeftLeafCount.push_back(0);
				bucketRightLeafCount.push_back(0);
			}

			for (auto&& leaf : currentLeaves)
			{
				const AABB& aabb = leaf.binding.aabb;
				real dist = aabb.position[sortIndex1] - rootBottomLeft[sortIndex1];
				int index = dist / bucketSize;
				bucketArea[index] += aabb.surfaceArea();
				bucketLeafCount[index] += 1;
			}

			real minCost = std::numeric_limits<real>::max();
			int minIndex = -1;

			for (int i = 0; i < bucketCount; ++i)
			{
				real leftArea = 0;
				real rightArea = 0;
				real leftLeafCount = 0;
				real rightLeafCount = 0;
				for (int j = 0; j < i; ++j)
				{
					leftArea += bucketArea[j];
					leftLeafCount += bucketLeafCount[j];
				}

				for (int j = i; j < bucketCount; ++j)
				{
					rightArea += bucketArea[j];
					rightLeafCount += bucketLeafCount[j];
				}

				real cost = leftArea / rootArea * leftLeafCount + rightArea / rootArea * rightLeafCount;
				bucketLeftLeafCount[i] = leftLeafCount;
				bucketRightLeafCount[i] = rightLeafCount;

				if (minCost > cost)
				{
					minCost = cost;
					minIndex = i;
				}

			}

			AABB leftRoot, rightRoot;
			std::vector<BVTNodeBinding> leftLeaves;
			std::vector<BVTNodeBinding> rightLeaves;

			for (auto&& leaf : currentLeaves)
			{
				const AABB& aabb = leaf.binding.aabb;
				real dist = aabb.position[sortIndex1] - rootBottomLeft[sortIndex1];
				int index = dist / bucketSize;
				if (index <= minIndex)
				{
					leftRoot.combine(aabb);
					leftLeaves.push_back(leaf);
				}
				else
				{
					rightRoot.combine(aabb);
					rightLeaves.push_back(leaf);
				}

			}

			if(leftLeaves.empty() || rightLeaves.empty())
			{
				// cannot split
				continue;
			}

			int leftNewNode = getNewNode();
			m_nodes[leftNewNode].parent = rootIndex;
			m_nodes[leftNewNode].aabb = leftRoot;
			m_nodes[rootIndex].left = leftNewNode;

			int rightNewNode = getNewNode();
			m_nodes[rightNewNode].parent = rootIndex;
			m_nodes[rightNewNode].aabb = rightRoot;
			m_nodes[rootIndex].right = rightNewNode;

			
			for(auto&& leaf: leftLeaves)
				m_nodes[leaf.nodeIndex].parent = leftNewNode;

			for(auto&& leaf: rightLeaves)
				m_nodes[leaf.nodeIndex].parent = rightNewNode;


			stack.push({ leftNewNode, leftLeaves });
			stack.push({ rightNewNode, rightLeaves });
			

		}

		for(int i = 0;i < m_leaves.size();++i)
		{
			updateHeight(m_leaves[i].nodeIndex);
		}

	}

	void DynamicBVT::checkHeight()
	{
		std::deque<int> stack;
		stack.push_back(m_rootIndex);
		int maxHeight = 0;
		while (!stack.empty())
		{
			int currentIndex = stack.front();
			stack.pop_front();
			int leftIndex = m_nodes[currentIndex].left;
			int rightIndex = m_nodes[currentIndex].right;
			if (m_nodes[currentIndex].height != 1 + std::max(m_nodes[leftIndex].height, m_nodes[rightIndex].height))
			{
				CORE_ASSERT(false, "Invalid height");
			}
			maxHeight = std::max(maxHeight, m_nodes[currentIndex].height);
			if (m_nodes[leftIndex].height == 0)
			{
				CORE_ASSERT(m_nodes[leftIndex].left == -1 && m_nodes[leftIndex].right == -1, "Invalid leaf node");
			}
			else
				stack.push_back(leftIndex);

			if (m_nodes[rightIndex].height == 0)
			{
				CORE_ASSERT(m_nodes[rightIndex].left == -1 && m_nodes[rightIndex].right == -1, "Invalid leaf node");
			}
			else
				stack.push_back(rightIndex);
		}
		CORE_ASSERT(maxHeight == m_nodes[m_rootIndex].height, "Invalid height");
	}

	void DynamicBVT::updateHeightAndAABB(int nodeIndex)
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

	void DynamicBVT::updateHeight(int nodeIndex)
	{
		CORE_ASSERT(nodeIndex >= 0, "Invalid node index");

		int currentIndex = m_nodes[nodeIndex].parent;
		while (currentIndex != -1)
		{
			int leftIndex = m_nodes[currentIndex].left;
			int rightIndex = m_nodes[currentIndex].right;
			m_nodes[currentIndex].height = 1 + std::max(m_nodes[leftIndex].height, m_nodes[rightIndex].height);
			currentIndex = m_nodes[currentIndex].parent;
		}
	}

	void DynamicBVT::updateAABB(int nodeIndex)
	{
		CORE_ASSERT(nodeIndex >= 0, "Invalid node index");

		int currentIndex = m_nodes[nodeIndex].parent;
		while (currentIndex != -1)
		{
			int leftIndex = m_nodes[currentIndex].left;
			int rightIndex = m_nodes[currentIndex].right;
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
			//root is a branch node, insert the new leaf into the tree
			int targetIndex = findBestNode(newNodeIndex);
			int parentIndex = m_nodes[targetIndex].parent;
			int mergeIndex = mergeTwoNodes(newNodeIndex, targetIndex);

			(m_nodes[parentIndex].left == targetIndex ? m_nodes[parentIndex].left : m_nodes[parentIndex].right) = mergeIndex;

			m_nodes[mergeIndex].parent = parentIndex;
			m_nodes[parentIndex].height = 1 + std::max(m_nodes[m_nodes[parentIndex].left].height, m_nodes[m_nodes[parentIndex].right].height);

			updateHeight(newNodeIndex);

			int currentIndex = newNodeIndex;

			while(currentIndex != m_rootIndex)
			{
				rotateNode(currentIndex);
				currentIndex = m_nodes[currentIndex].parent;
			}
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

		updateHeightAndAABB(siblingIndex);

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
		updateHeightAndAABB(nodeIndex);
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
			real leftCost = AABB::combine(m_nodes[m_nodes[currentIndex].left].aabb, m_nodes[nodeIndex].aabb).surfaceArea()
				- m_nodes[m_nodes[currentIndex].left].aabb.surfaceArea();
			real rightCost = AABB::combine(m_nodes[m_nodes[currentIndex].right].aabb, m_nodes[nodeIndex].aabb).surfaceArea()
				- m_nodes[m_nodes[currentIndex].right].aabb.surfaceArea();

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


    //       A             A              A              A  
    //      / \		      / \		     / \		        / \	
    //     B   C		     B   C		    C   B		   C   B
    //    / \		    / \			       / \	          / \	
    //   D   E		   E   D			      D   E	         E   D	
    //  / \			      / \			 / \	                / \	
    // F   G			     F   G		    F   G		       F   G		
    //       LL             LR            RL             RR
    
    //                           | 
    //                           | to
    //                           v 
    
    //       B		  	  D                D                B
    //      / \          / \              / \              / \	
    //     D   A		    B   A		     A   B		      A   D
    //    / \ / \      / \ / \          / \ / \          / \ / \
    	//   F  G E  C    E  F G  C        C  F G  E        C  E F  G
    
    
    	// F is new added leaf, default is left child of D

	void DynamicBVT::rotateNode(int nodeIndex)
	{
		int F = nodeIndex;
		int D = m_nodes[nodeIndex].parent;

		//1. filter some cases that we don't need to rotate

		if (D == -1)
			return;

		int B = m_nodes[D].parent;

		if(B == -1)
			return;

		if(B == m_rootIndex)
		{
			m_nodes[B].aabb = AABB::combine(m_nodes[m_nodes[B].left].aabb, m_nodes[m_nodes[B].right].aabb);
			return;
		}

		int A = m_nodes[B].parent;
		int G = m_nodes[D].right;

		if(F == G)
		{
			// it's not strict node we have specified (rotate node must be on the left side)
			// this case may happen when we rotate leaf node recursively (rotate leaf's parent, etc...)
			// just swap F and G for D
			int temp = m_nodes[D].left;
			m_nodes[D].left = m_nodes[D].right;
			m_nodes[D].right = temp;
			G = m_nodes[D].right;
		}

		bool isLeft1 = m_nodes[A].left == B;
		bool isLeft2 = m_nodes[B].left == D;

		int C = isLeft1 ? m_nodes[A].right : m_nodes[A].left;
		int E = isLeft2 ? m_nodes[B].right : m_nodes[B].left;


		bool unbalanced = std::abs(m_nodes[B].height - m_nodes[C].height) > 1;

		if(!unbalanced)
		{
			updateHeightAndAABB(D);
			return;
		}

		//2. need to rotate the tree

		bool minimalTree = A == m_rootIndex;

		int parentA = -1;
		bool isLeftA = false;

		if (!minimalTree)
		{
			parentA = m_nodes[A].parent;
			isLeftA = m_nodes[parentA].left == A;
		}

		if (isLeft1 && isLeft2)
		{
			//LL
			m_nodes[B].right = A;
			m_nodes[A].parent = B;

			m_nodes[A].left = E;
			m_nodes[E].parent = A;

			m_nodes[A].height = 1 + std::max(m_nodes[E].height, m_nodes[C].height);
			m_nodes[B].height = 1 + std::max(m_nodes[A].height, m_nodes[D].height);

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
				updateHeightAndAABB(B);
			}
		}
		else if (isLeft1 && !isLeft2)
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

			m_nodes[B].height = 1 + std::max(m_nodes[E].height, m_nodes[F].height);
			m_nodes[A].height = 1 + std::max(m_nodes[G].height, m_nodes[C].height);
			m_nodes[D].height = 1 + std::max(m_nodes[B].height, m_nodes[A].height);

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
				updateHeightAndAABB(D);
			}
		}
		else if (!isLeft1 && isLeft2)
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

			m_nodes[A].height = 1 + std::max(m_nodes[C].height, m_nodes[F].height);
			m_nodes[B].height = 1 + std::max(m_nodes[G].height, m_nodes[E].height);
			m_nodes[D].height = 1 + std::max(m_nodes[A].height, m_nodes[B].height);

			m_nodes[A].aabb = AABB::combine(m_nodes[C].aabb, m_nodes[F].aabb);
			m_nodes[B].aabb = AABB::combine(m_nodes[E].aabb, m_nodes[G].aabb);
			m_nodes[D].aabb = AABB::combine(m_nodes[A].aabb, m_nodes[B].aabb);

			if (minimalTree)
			{
				m_nodes[D].parent = -1;
				m_rootIndex = D;
			}
			else
			{
				(isLeftA ? m_nodes[parentA].left : m_nodes[parentA].right) = D;
				m_nodes[D].parent = parentA;
				updateHeightAndAABB(D);
			}
		}
		else
		{
			//RR
			m_nodes[B].left = A;
			m_nodes[A].parent = B;

			m_nodes[A].right = E;
			m_nodes[E].parent = A;

			m_nodes[A].height = 1 + std::max(m_nodes[C].height, m_nodes[E].height);
			m_nodes[B].height = 1 + std::max(m_nodes[A].height, m_nodes[D].height);

			m_nodes[A].aabb = AABB::combine(m_nodes[C].aabb, m_nodes[E].aabb);
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
				updateHeightAndAABB(B);
			}
		}

	}



}
