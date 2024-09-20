#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class PhysicsScene : public AbstractScene
	{
	public:
		explicit PhysicsScene()
			: AbstractScene("PhysicsScene")
		{
		}

		~PhysicsScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;


	private:

		void createObjects();
		void clearObjects();

		std::vector<int> m_objectIds;

		std::vector<Transform> m_transforms;
		std::vector<Vector2> m_velocities;
		std::vector<float> m_angularVelocities;
		std::vector<Vector2> m_forces;
		std::vector<float> m_torques;
		std::vector<float> m_masses;
		std::vector<float> m_inertias;
		std::vector<float> m_invMasses;
		std::vector<float> m_invInertias;
		std::vector<float> m_restitution;
		std::vector<float> m_friction;

		std::vector<ST::AABB> m_aabbs;
		std::vector<uint32_t> m_bitmasks;
		std::vector<ST::Shape*> m_shapes;


		std::vector<int> m_dbvtStack;
		float m_expandRatio = 0.03f;

		DynamicBVT m_dbvt;
		DynamicGrid m_grid;
		ObjectGraph m_objectGraph;

		ST::Rectangle rect;
		ST::Rectangle land;
		ST::Ellipse ellipse;
		ST::Capsule capsule;
		ST::Circle circle;
		ST::Polygon polygon;
		ST::Edge edge;

		IdPool m_objectIdPool;

		int m_count = 5;

		ObjectID m_landId;

		bool m_showObject = true;
		bool m_showObjectID = false;
		bool m_showDBVT = false;
		bool m_showAABB = false;
		bool m_showGrid = false;
		bool m_showJoint = false;
		bool m_showTransform = false;
		bool m_showContacts = false;
		bool m_showContactNormal = false;
		bool m_showContactsMagnitude = false;

		bool m_showGraphColor = false;
	};
}