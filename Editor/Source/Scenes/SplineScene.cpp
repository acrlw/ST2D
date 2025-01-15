#include "SplineScene.h"


namespace STEditor
{
	SplineScene::~SplineScene()
	{

	}

	void SplineScene::onLoad()
	{
		m_controlPoints.emplace_back(0.4f, 1.0f, 1.0f);
		m_controlPoints.emplace_back( 0.8f, 1.0f, 1.0f );
		m_controlPoints.emplace_back( 1.0f, 0.8f, 1.0f );
		m_controlPoints.emplace_back( 1.0f, 0.4f, 1.0f );

		updateKnots();
		updateSamplePoints();
	}

	void SplineScene::onUnLoad()
	{

	}

	void SplineScene::onUpdate(float deltaTime)
	{

	}

	void SplineScene::onRender(Renderer2D& renderer)
	{
		if (m_addPoint)
			renderer.point(m_mousePoint, Palette::Blue, 8.0f);

		if (!m_samplePoints.empty())
		{
			renderer.polyLines(m_samplePoints, Palette::Yellow);
		}
		std::vector<Vector2> points;
		if (!m_controlPoints.empty())
		{
			for (auto&& elem : m_controlPoints)
			{
				Vector2 point(elem.x, elem.y);
				points.emplace_back(elem.x, elem.y);
				renderer.point(point, Palette::Green, 8.0f);
			}
		}
		else
			return;


		if (m_controlPoints.size() > 1)
			renderer.polyLines(points, Palette::LightCyan);

	}

	void SplineScene::onRenderUI()
	{
		ImGui::Begin("Spline");
		ImGui::Checkbox("Snap", &m_snap);
		ImGui::Checkbox("Add/Edit", &m_addPoint);
		std::string knotFmtStr = "Knots: ";
		for (auto&& elem : m_knots)
			knotFmtStr += std::format("{:.2f} ", elem);
		
		ImGui::Text(knotFmtStr.c_str());


		for (int i = 0;i < m_controlPoints.size(); ++i)
		{
			std::string point = std::format("Point {}", i);
			ImGui::DragFloat3(point.c_str(), &m_controlPoints[i].x, 0.01f);
		}
		ImGui::Columns(1);
		ImGui::End();
	}

	void SplineScene::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_A)
				m_addPoint = !m_addPoint;
			if (key == GLFW_KEY_S)
				m_snap = !m_snap;
		}
		
	}

	void SplineScene::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{
		m_mousePoint = renderer.screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });
		if (m_snap)
		{
			//snap to 0.1
			m_mousePoint.x = std::round(m_mousePoint.x * 10) / 10;
			m_mousePoint.y = std::round(m_mousePoint.y * 10) / 10;
		}
		if (m_movePoint)
		{
			m_controlPoints[m_selectedIndex].x = m_mousePoint.x;
			m_controlPoints[m_selectedIndex].y = m_mousePoint.y;
			updateSamplePoints();
		}
	}


	void SplineScene::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;

		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (m_addPoint)
			{
				m_controlPoints.emplace_back(m_mousePoint.x, m_mousePoint.y, 1.0f);
				
				updateKnots();
				updateSamplePoints();
			}
			else
			{
				m_selectedIndex = -1;
				m_movePoint = false;
				if (!m_controlPoints.empty())
				{
					for (int i = 0; i < m_controlPoints.size(); ++i)
					{
						Vector2 p(m_controlPoints[i].x, m_controlPoints[i].y);
						if (p.distance(m_mousePoint) < 0.1f)
						{
							m_selectedIndex = i;
							break;
						}
					}

					if (m_selectedIndex != -1)
						m_movePoint = true;
				}
			}
		}
		else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (!m_controlPoints.empty())
			{
				if (m_controlPoints.size() > 4)
				{
					for (int i = 0; i < m_controlPoints.size(); ++i)
					{
						Vector2 p(m_controlPoints[i].x, m_controlPoints[i].y);
						if (p.distance(m_mousePoint) < 0.1f)
						{
							m_controlPoints.erase(m_controlPoints.begin() + i);
							
							updateKnots();
							updateSamplePoints();
							break;
						}
					}
				}
			}
		}
		else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_movePoint = false;
		}

		
	}

	real SplineScene::deBoor(real u, int i, int k)
	{
		return 0.0f;
	}

	Vector2 SplineScene::samplePoint(real u)
	{

		Vector2 curvePoint;
		return curvePoint;
	}

	void SplineScene::updateKnots()
	{
		m_knots.clear();
		m_knots.resize(m_controlPoints.size() + m_degree + 1);
		for (int i = 0;i < 4; ++i)
		{
			m_knots[i] = 0;
			m_knots[m_knots.size() - 1 - i] = 1;
		}
		if (m_controlPoints.size() > 4)
		{
			int size = m_knots.size() - 6;

			for (int i = 3; i < size + 3; ++i)
			{
				m_knots[i] = static_cast<real>(i - 3) / static_cast<real>(size);
			}
		}
		updateSamplePoints();
		updateCurvaturePoints();
	}

	void SplineScene::updateSamplePoints()
	{
		m_samplePoints.clear();
		if (m_controlPoints.size() < 4)
			return;

		for (int i = 0;i < m_sampleCount; ++i)
		{
			real u = static_cast<real>(i) / static_cast<real>(m_sampleCount - 1);
			Vector2 point = samplePoint(u);
			m_samplePoints.emplace_back(point.x, point.y);
		}

	}



	void SplineScene::updateCurvaturePoints()
	{
		m_curvatureStart.clear();
		m_curvatureEnd.clear();

		if (m_controlPoints.size() < 4)
			return;


	}

	void SplineScene::nurbsDerivative(real u, int order)
	{

	}

	void SplineScene::nurbsCurvature(real u)
	{

	}
}


