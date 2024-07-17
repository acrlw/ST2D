#include "CurveScene.h"
#include "Render.h"

namespace STEditor
{
	CurveScene::~CurveScene()
	{
		
	}

	void CurveScene::onLoad()
	{
		g1Vertices.reserve(m_count);
		g2Vertices.reserve(m_count);
		g3Vertices.reserve(m_count);
	}

	void CurveScene::onUnLoad()
	{
	}

	void CurveScene::onUpdate(float deltaTime)
	{
		computeBasicParam();

		computeG1Vertices();
		computeG2Vertices();
		computeG3Vertices();
	}

	void CurveScene::onRender(sf::RenderWindow& window)
	{
		if(m_showReferenceLine)
		{
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCenter, roundCorner, RenderConstant::LightGray);
			Vector2 ref1(p01.x, 0.0f);
			Vector2 ref2(0.0f, p10.y);
			Vector2 ref3, ref4;

			RenderSFMLImpl::renderLine(window, *m_settings.camera, p01, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, p10, ref2, RenderConstant::LightGray);


			ref1 = Vector2(m_halfWidth, m_halfHeight - currentRadius);
			ref2 = Vector2(0.0f, m_halfHeight - currentRadius);
			ref3 = Vector2(m_halfWidth - currentRadius, m_halfHeight);
			ref4 = Vector2(m_halfWidth - currentRadius, 0.0f);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref1, ref2, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref3, ref4, RenderConstant::LightGray);

			ref1 = Vector2(roundCorner.x, 0.0f);
			ref2 = Vector2(0.0f, roundCorner.y);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCorner, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCorner, ref2, RenderConstant::LightGray);
			
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCenter, startRoundedPos, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCenter, endRoundedPos, RenderConstant::LightGray);
			
		}

		if(m_showG1)
		{
			drawCurve(window, g1Vertices, RenderConstant::Red);
		}

		if(m_showG2)
		{
			drawCurve(window, g2Vertices, RenderConstant::Green);
		}

		if (m_showG3)
		{
			drawCurve(window, g3Vertices, RenderConstant::Blue);
		}

		if(m_showRoundedCurvature)
		{
			drawCurvature(window, roundCurvatureStart, roundCurvatureEnd, RenderConstant::LightGray);
		}

		if(m_showBezierCurvature)
		{
			drawCurvature(window, bezierCurve1, bezierCurvature1, RenderConstant::Green);
			drawCurvature(window, bezierCurve2, bezierCurvature2, RenderConstant::Green);
		}

		if(m_showG3Curvature)
		{
			
			drawCurvature(window, rationalBezierCurve1, rationalBezierCurvature1, RenderConstant::Blue);
			drawCurvature(window, rationalBezierCurve2, rationalBezierCurvature2, RenderConstant::Blue);
		}
	}

	void CurveScene::onRenderUI()
	{
		ImGui::Begin("Rounded Rect");

		ImGui::SeparatorText("Rounded Param");
		ImGui::DragFloat("Half Width", &m_halfWidth, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Half Height", &m_halfHeight, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Rounded Radius Percentage", &m_percentage, 0.005f, 0.005f, 1.0f);
		ImGui::DragFloat("Inner Width Percentage", &m_innerWidthFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Inner Height Percentage", &m_innerHeightFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Corner Angle Percentage", &m_cornerPercentage, 0.005f, 0.0f, 0.995f);
		ImGui::DragInt("Bezier Sample Count", &m_bezierCount, 1, 8, 500);
		ImGui::DragInt("Circle Segment Count", &m_count, 2, 4, 500);
		ImGui::DragFloat("Curvature Scale Factor", &m_curvatureScaleFactor, 0.01f, 0.01f, 1.0f);

		ImGui::SeparatorText("Weights");
	   
		ImGui::DragFloat("Bezier1 Weight P0", &m_rationalCubicBezier1.weightAt(0), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier1 Weight P1", &m_rationalCubicBezier1.weightAt(1), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier1 Weight P2", &m_rationalCubicBezier1.weightAt(2), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier1 Weight P3", &m_rationalCubicBezier1.weightAt(3), 1e-4f, 1e-7f, 5.0f, "%.7f");
		
		ImGui::Checkbox("Connect Weights", &m_connectWeights);

		if(m_connectWeights)
		{
			m_rationalCubicBezier2.weightAt(0) = m_rationalCubicBezier1.weightAt(0);
			m_rationalCubicBezier2.weightAt(1) = m_rationalCubicBezier1.weightAt(1);
			m_rationalCubicBezier2.weightAt(2) = m_rationalCubicBezier1.weightAt(2);
			m_rationalCubicBezier2.weightAt(3) = m_rationalCubicBezier1.weightAt(3);
		}

		ImGui::DragFloat("Bezier2 Weight P0", &m_rationalCubicBezier2.weightAt(0), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier2 Weight P1", &m_rationalCubicBezier2.weightAt(1), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier2 Weight P2", &m_rationalCubicBezier2.weightAt(2), 1e-4f, 1e-7f, 5.0f, "%.7f");
		ImGui::DragFloat("Bezier2 Weight P3", &m_rationalCubicBezier2.weightAt(3), 1e-4f, 1e-7f, 5.0f, "%.7f");


		if (m_connectWeights)
		{
			m_rationalCubicBezier1.weightAt(0) = m_rationalCubicBezier2.weightAt(0);
			m_rationalCubicBezier1.weightAt(1) = m_rationalCubicBezier2.weightAt(1);
			m_rationalCubicBezier1.weightAt(2) = m_rationalCubicBezier2.weightAt(2);
			m_rationalCubicBezier1.weightAt(3) = m_rationalCubicBezier2.weightAt(3);
		}

		ImGui::SeparatorText("Optimization");
		ImGui::Checkbox("Enable", &m_numOptimize);
		ImGui::DragFloat("Optimize Residual Resolution", &m_residualResolution, 1e-6f, 1e-6f, 0.1f, "%.7f");
		ImGui::DragFloat("Optimize Speed", &m_optimizeSpeed, 1e-6f, 1e-7f, 0.1f, "%.7f");
		
		ImGui::SeparatorText("Visibility");
		ImGui::Checkbox("Show Rounded Curvature", &m_showRoundedCurvature);
		ImGui::Checkbox("Show Bezier Curvature", &m_showBezierCurvature);
		ImGui::Checkbox("Show Reference Line", &m_showReferenceLine);
		ImGui::Checkbox("Show Quintic Bezier Curve", &m_showQuinticBezier);
		ImGui::Checkbox("Show Quintic Bezier Curvature", &m_showQuinticBezierCurvature);
		ImGui::Checkbox("Show G1 Continuity", &m_showG1);
		ImGui::Checkbox("Show G2 Continuity", &m_showG2);
		ImGui::Checkbox("Show G3 Continuity", &m_showG3);
		ImGui::Checkbox("Show G3 Curvature", &m_showG3Curvature);

		ImGui::End();
	}

	void CurveScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void CurveScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void CurveScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void CurveScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void CurveScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void CurveScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}

	void CurveScene::drawCurve(sf::RenderWindow& window, const std::vector<Vector2>& curve, const sf::Color& color) const
	{
		for (size_t i = 1; i < curve.size(); ++i)
		{
			Vector2 point0 = curve[i];
			Vector2 point1 = curve[i - 1];
			RenderSFMLImpl::renderLine(window, *m_settings.camera, point0, point1, color);
			point0.y = -point0.y;
			point1.y = -point1.y;
			RenderSFMLImpl::renderLine(window, *m_settings.camera, point0, point1, color);
			point0.x = -point0.x;
			point1.x = -point1.x;
			RenderSFMLImpl::renderLine(window, *m_settings.camera, point0, point1, color);
			point0.y = -point0.y;
			point1.y = -point1.y;
			RenderSFMLImpl::renderLine(window, *m_settings.camera, point0, point1, color);
		}
	}

	void CurveScene::drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start,
		const std::vector<Vector2>& end, const sf::Color& color)
	{
		for (size_t i = 1; i < start.size(); ++i)
		{
			RenderSFMLImpl::renderLine(window, *m_settings.camera, end[i - 1]
				, end[i], color);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, start[i], end[i], color);
		}
	}

	void CurveScene::computeBasicParam()
	{
		p00.set(0, m_halfHeight);
		p11.set(m_halfWidth, 0);

		float min = std::min(m_halfWidth, m_halfHeight);
		float maxRadius = min * m_percentage;
		currentRadius = maxRadius * m_percentage;

		roundCenter.set(m_halfWidth - currentRadius, m_halfHeight - currentRadius);
		roundCorner.set(currentRadius * std::cos(Math::radians(45)),
			currentRadius * std::sin(Math::radians(45)));
		
		roundCorner += roundCenter;

		p01.set(m_innerWidthFactor * (m_halfWidth - currentRadius), m_halfHeight);
		p10.set(m_halfWidth, m_innerHeightFactor * (m_halfHeight - currentRadius));

		float round = Math::radians(45.0f * m_cornerPercentage);
		roundStartRadians = Math::radians(45.0f) - round;
		roundEndRadians = Math::radians(45.0f) + round;

		startRoundedPos = currentRadius * Vector2(std::cos(roundStartRadians), std::sin(roundStartRadians));
		endRoundedPos = currentRadius * Vector2(std::cos(roundEndRadians), std::sin(roundEndRadians));

		startRoundedPos += roundCenter;
		endRoundedPos += roundCenter;
	}

	void CurveScene::computeG1Vertices()
	{
		g1Vertices.clear();
		g1Vertices.push_back(p11);

		float step = Constant::Pi * 0.5f / static_cast<float>(m_count);

		for (int i = 0; i <= m_count; ++i)
		{
			float angle = static_cast<float>(i) * step;
			Vector2 p(currentRadius * std::cos(angle), currentRadius * std::sin(angle));
			p += roundCenter;

			g1Vertices.push_back(p);
		}

		g1Vertices.push_back(p00);

		//for rounded curvature
		float k = 1.0f / currentRadius;
		scaleK = m_curvatureScaleFactor * k;
		//draw curvature of rounded
		roundCurvatureStart.clear();
		roundCurvatureEnd.clear();

		float roundedStep = roundEndRadians - roundStartRadians;
		float roundedCount = static_cast<float>(m_count) * 0.5f;
		roundedStep /= roundedCount;

		for (float i = 0; i <= roundedCount; i += 1.0f)
		{
			float angle = i * roundedStep + roundStartRadians;
			Vector2 from(std::cos(angle), std::sin(angle));
			from = currentRadius * from + roundCenter;

			Vector2 point(std::cos(angle), std::sin(angle));
			point *= scaleK;
			point += from;

			roundCurvatureEnd.push_back(point);
			roundCurvatureStart.push_back(from);
		}
	}

	void CurveScene::computeG2Vertices()
	{
		g2Vertices.clear();
		g2Vertices.push_back(p00);

		m_bezier1[0] = p01;
		m_bezier1[3] = endRoundedPos;

		m_bezier2[0] = p10;
		m_bezier2[3] = startRoundedPos;

		Vector2 endDir = roundCenter - endRoundedPos;
		endDir = endDir.normal().perpendicular();
		float t = (m_halfHeight - endRoundedPos.y) / endDir.y;
		Vector2 p = endRoundedPos + t * endDir;
		m_bezier1[2] = p;

		endDir = roundCenter - startRoundedPos;
		endDir = endDir.normal().perpendicular();
		t = (m_halfWidth - startRoundedPos.x) / endDir.x;
		p = startRoundedPos + t * endDir;
		m_bezier2[2] = p;

		Vector2 P32 = m_bezier1[3] - m_bezier1[2];
		Vector2 P02 = m_bezier1[0] - m_bezier1[2];
		float u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (currentRadius * std::abs(P32.cross(P02))));
		m_bezier1[1] = m_bezier1[0] * u + (1.0f - u) * m_bezier1[2];

		P32 = m_bezier2[3] - m_bezier2[2];
		P02 = m_bezier2[0] - m_bezier2[2];
		u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (currentRadius * std::abs(P32.cross(P02))));
		m_bezier2[1] = m_bezier2[0] * u + (1.0f - u) * m_bezier2[2];

		bezierCurve1 = m_bezier1.samplePoints(m_bezierCount);
		bezierCurve2 = m_bezier2.samplePoints(m_bezierCount);

		for (auto iter = bezierCurve1.begin(); iter != bezierCurve1.end() - 1; ++iter)
			g2Vertices.push_back(*iter);
		
		for (auto iter = roundCurvatureStart.rbegin() + 1; iter != roundCurvatureStart.rend(); ++iter)
			g2Vertices.push_back(*iter);

		for (auto&& iter = bezierCurve2.rbegin() + 1; iter != bezierCurve2.rend(); ++iter)
			g2Vertices.push_back(*iter);
		

		g2Vertices.push_back(p11);

		
		if (m_showBezierCurvature)
		{
			bezierCurvature1.clear();
			bezierCurvature2.clear();

			bezierCurvature1 = m_bezier1.curvaturePoints(m_bezierCount, m_curvatureScaleFactor);
			bezierCurvature2 = m_bezier2.curvaturePoints(m_bezierCount, m_curvatureScaleFactor, true);
		}
	}

	void CurveScene::computeG3Vertices()
	{
		g3Vertices.clear();
		g3Vertices.push_back(p00);

		m_rationalCubicBezier1.pointAt(0) = m_bezier1[0];
		m_rationalCubicBezier1.pointAt(1) = m_bezier1[1];
		m_rationalCubicBezier1.pointAt(2) = m_bezier1[2];
		m_rationalCubicBezier1.pointAt(3) = m_bezier1[3];

		m_rationalCubicBezier2.pointAt(0) = m_bezier2[0];
		m_rationalCubicBezier2.pointAt(1) = m_bezier2[1];
		m_rationalCubicBezier2.pointAt(2) = m_bezier2[2];
		m_rationalCubicBezier2.pointAt(3) = m_bezier2[3];

		if (m_numOptimize)
		{
			float resolution = m_residualResolution;
			{
				//optimize w_0 to make curvature variance at P_3 close to zero
				float k0 = m_rationalCubicBezier1.curvature(0.0f);
				float k1 = m_rationalCubicBezier1.curvature(1.0f);

				float t2 = static_cast<float>(m_bezierCount - 1) / static_cast<float>(m_bezierCount);
				float k2 = m_rationalCubicBezier1.curvature(t2);

				float bezierStep = 1.0f / static_cast<float>(m_bezierCount);


				float residual1 = (k0 - m_rationalCubicBezier1.curvature(bezierStep)) / bezierStep;
				float residual2 = (k1 - k2) / bezierStep;
				
				if (residual2 > 0)
					m_rationalCubicBezier1.weightAt(0) *= (1.0f + m_optimizeSpeed);
				else
					m_rationalCubicBezier1.weightAt(0) *= (1.0f - m_optimizeSpeed);
				
				float residual3 = k1 - scaleK / m_curvatureScaleFactor;

				if (residual3 > 0)
					m_rationalCubicBezier1.weightAt(1) *= 0.9999f;
				else
					m_rationalCubicBezier1.weightAt(1) *= 1.0001f;

				APP_INFO("res1:{}, res2:{}, res3:{}, w0:{}", residual1, residual2, residual3, m_rationalCubicBezier1.weightAt(0))

			}
		}

		rationalBezierCurve1 = m_rationalCubicBezier1.samplePoints(m_bezierCount);

		for (auto iter = rationalBezierCurve1.begin(); iter != rationalBezierCurve1.end() - 1; ++iter)
			g3Vertices.push_back(*iter);

		for (auto iter = roundCurvatureStart.rbegin();
			iter != roundCurvatureStart.rend(); ++iter)
			g3Vertices.push_back(*iter);

		rationalBezierCurve2 = m_rationalCubicBezier2.samplePoints(m_bezierCount);

		for (auto iter = rationalBezierCurve2.rbegin() + 1; iter != rationalBezierCurve2.rend(); ++iter)
			g3Vertices.push_back(*iter);


		g3Vertices.push_back(p11);

		if (m_showBezierCurvature)
		{
			rationalBezierCurvature1.clear();
			rationalBezierCurvature2.clear();

			rationalBezierCurvature1 = m_rationalCubicBezier1.curvaturePoints(m_bezierCount, m_curvatureScaleFactor);
			rationalBezierCurvature2 = m_rationalCubicBezier2.curvaturePoints(m_bezierCount, m_curvatureScaleFactor, true);
		}

	}
}
