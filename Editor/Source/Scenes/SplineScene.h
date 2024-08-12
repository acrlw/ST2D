#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class SplineScene : public AbstractScene
	{

	public:
		explicit SplineScene(const SceneSettings& settings)
			: AbstractScene(settings, "SplineScene")
		{
		}

		~SplineScene() override;
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
		Vector2* m_targetPoint = nullptr;
		bool m_addPoint = false;
		bool m_movePoint = false;
		bool m_snap = false;
		Vector2 m_mousePoint;
		std::vector<Vector2> m_points;
		std::vector<std::vector<Vector2>> m_splineList;
		int m_degree = 3;

	};
}