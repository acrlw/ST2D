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
		std::vector<int> m_objectIds;

		std::vector<Vector2> m_positions;
		std::vector<Vector2> m_velocities;
		std::vector<Complex> m_rotations;
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

		DynamicBVT m_dbvt;
		DynamicGrid m_grid;
		ObjectGraph m_objectGraph;
	};
}