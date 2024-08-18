#pragma once

#include "AbstractBroadphase.h"
#include "ST2D/Shape/AABB.h"

namespace ST
{
	struct ST_API BVTNode
	{
		bool valid = false;
		int parent = -1;
		int left = -1;
		int right = -1;
		int leafIndex = -1; // index in leaves
		int height = 0;
		AABB aabb;

		bool isLeaf()const
		{
			return valid && left == -1 && right == -1 && parent != -1;
		}
		void reset()
		{
			*this = BVTNode();
		}
	};

	struct ST_API BVTNodeBinding
	{
		int nodeIndex = -1; // index in tree
		BroadphaseObjectBinding binding;

		bool isValid()const
		{
			return nodeIndex != -1;
		}
	};

	class ST_API DynamicBVT : public AbstractBroadphase
	{
	public:
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(int objectId, const AABB& aabb) override;
		void clearAllObjects() override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;

		//private:

		void insertLeaf(const BVTNodeBinding& leaf);
		void removeLeaf(int objectId);
		void updateLeaf(int objectId, const AABB& aabb);

		int findBestNode(int nodeIndex) const;

		int greedyFindBestNode(int nodeIndex) const;
		int fullFindBestNode(int nodeIndex) const;

		int getNewNode();
		void freeNode(int nodeIndex);
		int getNewLeafNode();
		void freeLeafNode(int nodeIndex);

		int mergeTwoNodes(int nodeIndexA, int nodeIndexB);

		void rotateNode(int nodeIndex);

		std::vector<BVTNode> m_nodes;
		std::vector<BVTNodeBinding> m_leaves;
		std::vector<int> m_freeNodes;
		std::vector<int> m_freeLeaves;

		int m_rootIndex = -1;
	};

	
}
