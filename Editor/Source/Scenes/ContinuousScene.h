#pragma once

#include "AbstractScene.h"

namespace STEditor
{

	class ContinuousScene : public AbstractScene
	{
	public:
		explicit ContinuousScene()
			: AbstractScene("ContinuousScene")
		{
		}
		~ContinuousScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;

	private:
		void reset();
		void createObjects();
		void step(real dt);
		void integratePos(real dt);

		ST::Rectangle bullet;
		ST::Rectangle rect;
		ST::Rectangle land;
		ST::Polygon triangle;
		ST::Ellipse ellipse;
		ST::Capsule capsule;
		ST::Circle circle;
		ST::Polygon polygon;
		ST::Segment edge;

		std::vector<ObjectID> m_objectIds;
		std::vector<Vector2> m_positions;
		std::vector<real> m_rotations;
		std::vector<Vector2> m_velocities;
		std::vector<real> m_angularVelocities;
		std::vector<ST::Shape*> m_shapes;
		std::vector<ST::AABB> m_aabbs;

		ObjectID m_landId;

		bool m_showObject = true;
		bool m_showObjectID = false;
		bool m_showAABB = false;
		bool m_showGrid = false;
		bool m_showTransform = false;


		bool m_showVelocityArrow = false;
		bool m_showVelocityMagnitude = false;
		bool m_showVelocityNormal = false;
		bool m_showVelocityColormap = false;
		bool m_showAngularVelocity = false;

		bool m_showBulletTrajPrediction = true;
		bool m_showFeature = true;

		bool m_enableDamping = false;
		bool m_enableGravity = false;

		bool m_simulate = false;

		float m_linearVelocityDamping = 0.9f;
		float m_angularVelocityDamping = 0.9f;
		float m_maxVelocityColormap = 10.0f;

		float m_biasFactor = 0.3f;
		float m_slop = 0.001f;
		int m_frequency = 60;
		float m_timeStep = 1.0f / m_frequency;

		Vector2 m_initVelocity{ 500, 50 };
		real m_initAngularVelocity = 500.0f;

		size_t m_stepCount = 0;
		double m_stepLastFrameTime = 0.0;
		double m_currentFrameTime = 0.0;

		size_t m_idIndex = 0;

		int m_showTrajIndex = 0;
		int m_maxTrajIndex = 3;
		int m_trajSampleCount = 200;
		ObjectID m_bulletId = 0;
		ObjectID m_objectId = 0;
		float m_t = 0.0f;
	};
}