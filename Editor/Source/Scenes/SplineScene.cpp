#include "SplineScene.h"


namespace STEditor
{
	SplineScene::~SplineScene()
	{

	}

	void SplineScene::onLoad()
	{

	}

	void SplineScene::onUnLoad()
	{

	}

	void SplineScene::onUpdate(float deltaTime)
	{

	}

	void SplineScene::onDraw(sf::RenderWindow& window)
	{



		for(auto& list: m_splineList)
		{
			RenderSFMLImpl::renderPolyDashedLine(window, *m_settings.camera, list, RenderConstant::Cyan);
			for(size_t i = 0; i < list.size(); i++)
			{
				RenderSFMLImpl::renderPoint(window, *m_settings.camera, list[i], RenderConstant::Cyan, 4);

			}
		}


		if(m_addPoint)
		{
			RenderSFMLImpl::renderPolyDashedLine(window, *m_settings.camera, m_points, RenderConstant::Gray);
			for (size_t i = 0; i < m_points.size(); i++)
			{
				RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_points[i], RenderConstant::Gray, 4);

			}

			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_mousePoint, RenderConstant::Green, 4);
		}

		if(m_points.size() > 3)
		{
			std::vector<real> knotVector;
			std::vector<real> weights;

			for(size_t i = 0; i < m_points.size(); i++)
				weights.push_back(1);



		}
	}

	void SplineScene::onRenderUI()
	{
		ImGui::Begin("Spline");

		ImGui::DragInt("Degree", &m_degree, 1, 3, 10);

		ImGui::End();
	}

	void SplineScene::onMousePress(sf::Event& event)
	{
		Vector2 pos(event.mouseButton.x, event.mouseButton.y);
		pos = m_settings.camera->screenToWorld(pos);
		if(m_addPoint)
		{
			Vector2 finalPoint = pos;
			if(m_snap)
			{
				//snap to grid, 0.1 is the grid size
				finalPoint.x = std::round(finalPoint.x * 10) / 10;
				finalPoint.y = std::round(finalPoint.y * 10) / 10;
			}
			m_points.push_back(finalPoint);
		}
		else
		{
			if(event.mouseButton.button == sf::Mouse::Left)
			{
				for(auto& list: m_splineList)
				{
					for(size_t i = 0; i < list.size(); i++)
					{
						if(pos.distance(list[i]) < 1e-2)
						{
							m_targetPoint = &list[i];
							m_movePoint = true;
							break;
						}
					}
				}
			}
		}
	}

	void SplineScene::onMouseRelease(sf::Event& event)
	{
		m_movePoint = false;
	}

	void SplineScene::onMouseMove(sf::Event& event)
	{
		Vector2 pos(event.mouseMove.x, event.mouseMove.y);
		m_mousePoint = m_settings.camera->screenToWorld(pos);

		if (m_snap)
		{
			//snap to grid, 0.1 is the grid size
			m_mousePoint.x = std::round(m_mousePoint.x * 10) / 10;
			m_mousePoint.y = std::round(m_mousePoint.y * 10) / 10;
		}
		if(m_movePoint && m_targetPoint != nullptr)
		{
			*m_targetPoint = m_mousePoint;
		}
	}

	void SplineScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void SplineScene::onKeyRelease(sf::Event& event)
	{
		if(event.key.code == sf::Keyboard::A)
		{
			m_addPoint = false;
			m_splineList.push_back(m_points);
			m_points.clear();
		}
		else if(event.key.code == sf::Keyboard::S)
			m_snap = false;
	}

	void SplineScene::onKeyPressed(sf::Event& event)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			m_addPoint = true;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			m_snap = true;
	}

}


