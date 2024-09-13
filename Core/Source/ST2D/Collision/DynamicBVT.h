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
			return height == 0 && leafIndex != -1;
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
			return nodeIndex != -1 && binding.objectId != -1;
		}
		bool isEmpty()const
		{
			return binding.objectId == -1;
		}
		void reset()
		{
			*this = BVTNodeBinding();
		}
	};


	class ST_API DynamicBVT : public AbstractBroadphase
	{
	public:
		void addObject(const BroadphaseObjectBinding& binding) override;
		void removeObject(int objectId) override;
		void updateObject(const BroadphaseObjectBinding& binding) override;
		void clearAllObjects() override;
		std::vector<ObjectPair> queryOverlaps() override;
		std::vector<int> queryAABB(const AABB& aabb) override;
		std::vector<int> queryRay(const Vector2& origin, const Vector2& direction, float maxDistance) override;

		void rebuildTree();

		void printTree();

		void directBuildTree(int nodeIndex, const std::vector<BVTNodeBinding>& leaves);

		void rebuildTreeSAH(const AABB& rootAABB, const std::vector<BVTNodeBinding>& leaves);

		void setOnlyInsert(bool onlyInsert);

		const std::vector<BVTNode>& nodes() const;

		int rootIndex() const;

		void checkHeight() const;

	private:


		void rotateNode(int nodeIndex);

		void updateHeightAndAABB(int nodeIndex);

		void updateHeight(int nodeIndex);
		void updateAABB(int nodeIndex);
		void updateAABB(int nodeIndex, int stopIndex);

		void onlyInsertLeaf(const BVTNodeBinding& leaf);
		void insertLeaf(const BVTNodeBinding& leaf);
		void directInsertLeaf(int leafIndex);
		void removeLeaf(int objectId);
		void directRemoveLeaf(int nodeIndex);
		void updateLeaf(const BroadphaseObjectBinding& binding);

		int findBestLeafNode(int nodeIndex) const;

		int greedyFindBestLeafNode(int nodeIndex) const;
		int fullFindBestLeafNode(int nodeIndex) const;

		int getNewNode();
		void freeNode(int nodeIndex);
		int getNewLeafNode();
		void freeLeafNode(int nodeIndex);

		int mergeTwoNodes(int nodeIndexA, int nodeIndexB);

		std::vector<BVTNode> m_nodes;
		std::vector<BVTNodeBinding> m_leaves;
		std::vector<int> m_freeNodes;
		std::vector<int> m_freeLeaves;

		int m_rootIndex = -1;

		//if true, only insert new object to list of leaves, not perform any tree rotation and update
		//this is used when need insert bulk of objects and rebuild tree later
		bool m_onlyInsert = false;
	};

	
}
