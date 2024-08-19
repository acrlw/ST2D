#pragma once

#include "AbstractScene.h"
#include "ST2D/Shape/Rectangle.h"
#include "ST2D/Shape/Circle.h"
#include "ST2D/Shape/Polygon.h"
#include <random>

namespace STEditor
{
	class BroadphaseScene : public AbstractScene
	{
	public:
		explicit BroadphaseScene(const SceneSettings& settings)
			: AbstractScene(settings, "BroadphaseScene")
		{
		}

		~BroadphaseScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(sf::RenderWindow& window) override;
		void onRenderUI() override;
		void onMousePress(sf::Event& event) override;
		void onMouseRelease(sf::Event& event) override;
		void onMouseMove(sf::Event& event) override;
		void onMouseDoubleClick(sf::Event& event) override;
		void onKeyRelease(sf::Event& event) override;
		void onKeyPressed(sf::Event& event) override;

	private:
		void createShapes();

		bool m_showAABB = true;
		bool m_showGrid = false;
		bool m_showBVT = true;
		bool m_showTransform = false;
		bool m_showObjectId = true;

		ST::Rectangle m_rectangle;
		ST::Circle m_circle;
		ST::Polygon m_polygon;
		ST::Capsule m_capsule;
		ST::Polygon m_triangle;
		std::array<ST::Shape*, 5> m_shapesArray = { &m_rectangle, &m_circle, &m_capsule, &m_triangle, &m_polygon };

		std::vector<Transform> m_transforms;
		std::vector<ST::AABB> m_aabbs;
		std::vector<uint32_t> m_bitmasks;
		std::vector<ST::Shape*> m_shapes;
		std::vector<int> m_objectIds;

		DynamicBVT m_dbvt;
		DynamicGrid m_grid;

		int m_count = 5;
		int m_maxHeight = 0;
		int m_currentHeight = 0;
		float m_expandRatio = 0.2f;

		IdPool m_objectIdPool;
	};
}