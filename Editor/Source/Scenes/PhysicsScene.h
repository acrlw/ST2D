#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	struct SingleContact
	{
		Vector2 localA;
		Vector2 localB;
		Vector2 rA;
		Vector2 rB;
		Vector2 vA;
		Vector2 vB;
		Vector2 velocityBias;
		real bias = 0.0f;
		real penetration = 0.0f;
		real effectiveMassNormal = 0.0f;
		real effectiveMassTangent = 0.0f;
		real sumNormalImpulse = 0.0f;
		real sumTangentImpulse = 0.0f;
	};

	struct ContactManifold
	{
		ObjectPair ids;
		ContactPair pair;
		std::array<SingleContact, 2> contacts;
		int count = 0;
		Vector2 normal;
		Vector2 tangent;
		real restitution = 0.0f;
		real friction = 1.0f;
		real penetration = 0.0f;
		Matrix2x2 normalMass;
		Matrix2x2 k;
		bool enable = true;
	};


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
		void step(float dt);

		void createObjects();
		void clearObjects();

		void generateAndColorContacts(float dt);
		void updateBroadphase(float dt);
		void integrateVelocities(float dt);
		void solveVelocities(float dt);
		void integratePositions(float dt);
		void solvePositions(float dt);
		void setUpConstraint(float dt);

		void solveContactVelocity(const ObjectPair& pair);
		void solveContactPosition(const ObjectPair& pair);

		real computeInertia(real mass, const Shape* shape);

		std::vector<int> m_objectIds;


		std::vector<Vector2> m_positions;
		std::vector<real> m_rotations;

		std::vector<Vector2> m_velocities;
		std::vector<float> m_angularVelocities;
		std::vector<Vector2> m_forces;
		std::vector<float> m_torques;
		std::vector<float> m_masses;
		std::vector<float> m_inertias;
		std::vector<float> m_invMasses;
		std::vector<float> m_invInertias;
		std::vector<float> m_restitutions;
		std::vector<float> m_frictions;

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

		int m_count = 2;

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
		bool m_showFrictionMagnitude = false;
		bool m_showFrictionNormal = false;
		bool m_showGraphColoring = false;

		bool m_showVelocity = false;
		bool m_showVelocityMagnitude = false;
		bool m_showAngularVelocity = false;

		bool m_enableDamping = true;
		bool m_enableGravity = true;
		bool m_solveByGraphColoring = false;
		bool m_enableGraphColoring = false;
		bool m_enableGrid = false;

		bool m_simulate = false;
		bool m_enableWarmstart = true;
		bool m_enableVelocityBlockSolver = false;
		bool m_enablePositionBlockSolver = false;
		bool m_parallelProcessing = false;
		bool m_useSIMD = false;
		bool m_flagInitial = true;

		float m_linearVelocityDamping = 0.9f;
		float m_angularVelocityDamping = 0.9f;

		float m_biasFactor = 0.3f;
		float m_slop = 0.001f;
		int m_frequency = 60;

		size_t m_stepCount = 0;

		int m_solveVelocityCount = 1;
		int m_solvePositionCount = 1;

		std::unordered_map<ObjectPair, ContactManifold, ObjectPairHash> m_contactManifolds;

		ThreadPool m_threadPool;

		std::vector<ST::ObjectPair> m_objectPairs;
	};
}