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
			Vector2 ref1(p01.x, (m_halfHeight - currentRadius) * m_innerHeightFactor);
			Vector2 ref2((m_halfWidth - currentRadius) * m_innerWidthFactor, p10.y);
			Vector2 ref3, ref4;

			RenderSFMLImpl::renderLine(window, *m_settings.camera, p01, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, p10, ref2, RenderConstant::LightGray);


			ref1 = Vector2(m_halfWidth, m_halfHeight - currentRadius);
			ref2 = Vector2(p01.x, m_halfHeight - currentRadius);
			ref3 = Vector2(m_halfWidth - currentRadius, m_halfHeight);
			ref4 = Vector2(m_halfWidth - currentRadius, p10.y);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref1, ref2, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref3, ref4, RenderConstant::LightGray);

			ref1 = Vector2((m_halfWidth - currentRadius) * m_innerWidthFactor, roundCorner.y);
			ref2 = Vector2(roundCorner.x, (m_halfHeight - currentRadius) * m_innerHeightFactor);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCorner, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCorner, ref2, RenderConstant::LightGray);
			
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCenter, startRoundedPos, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, roundCenter, endRoundedPos, RenderConstant::LightGray);
			
		}

		if(m_showG1)
		{
			auto color = RenderConstant::Orange;
			//color.a = 100;
			drawCurve(window, g1Vertices, color);
		}

		if(m_showG2)
		{
			//for (size_t i = 0; i < g2Vertices.size(); ++i)
			//{
			//	RenderSFMLImpl::renderUInt(window, *m_settings.camera, g2Vertices[i], *m_settings.font,
			//		i, RenderConstant::Green, 18, Vector2(0.0f, 0.0f));
			//	RenderSFMLImpl::renderPoint(window, *m_settings.camera, g2Vertices[i], RenderConstant::Green, 2.0f);
			// 
			//}
			auto color = RenderConstant::Green;
			//color.a = 100;
			drawCurve(window, g2Vertices, color);
			
		}

		if (m_showG3)
		{
			//for (size_t i = 0; i < g3Vertices.size(); ++i)
			//{
			//	RenderSFMLImpl::renderUInt(window, *m_settings.camera, g3Vertices[i], *m_settings.font,
			//		i, RenderConstant::Blue, 18, Vector2(0.0f, 0.0f));
			//	RenderSFMLImpl::renderPoint(window, *m_settings.camera, g3Vertices[i], RenderConstant::Blue, 3.0f);
			//	//RenderSFMLImpl::renderPoint(window, *m_settings.camera, elem, RenderConstant::Green, 2.0f);
			//}

			auto color = RenderConstant::LightBlue;
			//color.a = 100;
			drawCurve(window, g3Vertices, color);

			//Vector2 p = m_rationalCubicBezier1.sample(1.0f);
			//Vector2 tangent = m_rationalCubicBezier1.tangent(1.0f);

			//float curvature = m_rationalCubicBezier1.curvatureAt(1.0f);

			//Vector2 n = tangent.perpendicular().normal();
			////Vector2 curvaturePoint = p + n * curvature * m_curvatureScaleFactor;

			//RenderSFMLImpl::renderArrow(window, *m_settings.camera, p, p + tangent, RenderConstant::Blue, 0.1f);
			//RenderSFMLImpl::renderArrow(window, *m_settings.camera, p, curvaturePoint, RenderConstant::Blue, 0.1f);
		}

		if(m_showRoundedCurvature)
		{
			drawCurvature(window, roundCurvatureStart, roundCurvatureEnd, RenderConstant::LightGray);
		}

		if(m_showBezierCurvature)
		{
			drawCurvature(window, bezierCurve1, bezierCurvature1, RenderConstant::Green);
			drawCurvature(window, bezierCurve2, bezierCurvature2, RenderConstant::Green, true);
		}

		if(m_showG3Curvature)
		{
			drawCurvature(window, rationalBezierCurve1, rationalBezierCurvature1, RenderConstant::Blue);
			drawCurvature(window, rationalBezierCurve2, rationalBezierCurvature2, RenderConstant::Blue, true);
		}
	}

	void CurveScene::onRenderUI()
	{
		ImGui::Begin("Rounded Rect");

		ImGui::SeparatorText("Rounded Param");
		ImGui::DragFloat("Half Width", &m_halfWidth, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Half Height", &m_halfHeight, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Rounded Radius Percentage", &m_percentage, 0.005f, 0.005f, 1.0f);
		ImGui::Columns(2);
		ImGui::Checkbox("Connect Inner", &m_connectInner);
		ImGui::Checkbox("Lock Corner Start", &m_lockCornerStart);
		ImGui::NextColumn();
		ImGui::Checkbox("Lock Relative Ratio", &m_lockRelativeRatio);
		ImGui::Checkbox("Lock Radius", &m_lockRadius);
		ImGui::Columns(1);
		ImGui::DragFloat("Inner Width Percentage", &m_innerWidthFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Inner Height Percentage", &m_innerHeightFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Corner Angle Percentage", &m_cornerPercentage, 0.005f, 0.0f, 0.999f);
		ImGui::DragInt("Bezier Sample Count", &m_bezierCount, 1, 8, 500);
		ImGui::DragInt("Circle Segment Count", &m_count, 2, 4, 500);
		ImGui::DragFloat("Curvature Scale Factor", &m_curvatureScaleFactor, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("Line Thickness", &m_thickness, 0.1f, 0.1f, 50.0f);

		ImGui::SeparatorText("Weights");
	   
		ImGui::DragFloat("Bezier1 Weight P0", &rationalBezierWeight1[0], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier1 Weight P1", &rationalBezierWeight1[1], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier1 Weight P2", &rationalBezierWeight1[2], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier1 Weight P3", &rationalBezierWeight1[3], 1e-5f, 1e-15f, 5.0f, "%.15f");

		ImGui::Checkbox("Connect Weights", &m_connectWeights);

		if (m_connectWeights)
		{
			for(int i = 0; i < 4; ++i)
				rationalBezierWeight2[i] = rationalBezierWeight1[i];
		}

		ImGui::DragFloat("Bezier2 Weight P0", &rationalBezierWeight2[0], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier2 Weight P1", &rationalBezierWeight2[1], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier2 Weight P2", &rationalBezierWeight2[2], 1e-5f, 1e-15f, 5.0f, "%.15f");
		ImGui::DragFloat("Bezier2 Weight P3", &rationalBezierWeight2[3], 1e-5f, 1e-15f, 5.0f, "%.15f");


		if (m_connectWeights)
		{
			for (int i = 0; i < 4; ++i)
				rationalBezierWeight1[i] = rationalBezierWeight2[i];
		}

		ImGui::SeparatorText("Optimization");
		ImGui::Checkbox("Enable", &m_numOptimize);
		ImGui::DragFloat("Optimize Residual Resolution", &m_residualResolution, 1e-6f, 1e-6f, 0.1f, "%.7f");
		ImGui::DragFloat("Optimize Speed", &m_optimizeSpeed, 1e-6f, 1e-7f, 0.1f, "%.7f");

		ImGui::SeparatorText("Visibility");
		ImGui::Checkbox("Show Reference Line", &m_showReferenceLine);
		ImGui::Checkbox("Show Rounded Curvature", &m_showRoundedCurvature);
		ImGui::Checkbox("Show Bezier Curvature", &m_showBezierCurvature);
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
		//RenderSFMLImpl::renderPolyDashedThickLine(window, *m_settings.camera, curve, color, m_thickness, 0.05f, 0.05f);
		for (size_t i = 1; i < curve.size(); ++i)
		{
			Vector2 point0 = curve[i];
			Vector2 point1 = curve[i - 1];
			RenderSFMLImpl::renderThickLine(window, *m_settings.camera, point0, point1, color, m_thickness);
			point0.y = -point0.y;
			point1.y = -point1.y;
			RenderSFMLImpl::renderThickLine(window, *m_settings.camera, point0, point1, color, m_thickness);
			point0.x = -point0.x;
			point1.x = -point1.x;
			RenderSFMLImpl::renderThickLine(window, *m_settings.camera, point0, point1, color, m_thickness);
			point0.y = -point0.y;
			point1.y = -point1.y;
			RenderSFMLImpl::renderThickLine(window, *m_settings.camera, point0, point1, color, m_thickness);
		}
	}

	void CurveScene::drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start,
		const std::vector<Vector2>& end, const sf::Color& color, bool flip)
	{
		if(start.size() != end.size())
			return;

		Vector2 lastEnd;
		for (size_t i = 0; i < start.size(); ++i)
		{
			Vector2 n = end[i] - start[i];
			n = n * m_curvatureScaleFactor;

			if(flip)
				n = -n;

			Vector2 newEnd = n + start[i];
			RenderSFMLImpl::renderLine(window, *m_settings.camera, start[i], newEnd, color);


			if (i != 0)
			{
				RenderSFMLImpl::renderLine(window, *m_settings.camera, lastEnd
					, newEnd, color);
			}

			lastEnd = newEnd;
		}
	}

	void CurveScene::computeBasicParam()
	{
		p00.set(0, m_halfHeight);
		p11.set(m_halfWidth, 0);

		float min = std::min(m_halfWidth, m_halfHeight);
		float maxRadius = min * m_percentage;

		if (!m_lockRadius)
			currentRadius = maxRadius * m_percentage;

		roundCenter.set(m_halfWidth - currentRadius, m_halfHeight - currentRadius);
		roundCorner.set(currentRadius * std::cos(Math::radians(45)),
			currentRadius * std::sin(Math::radians(45)));

		roundCorner += roundCenter;

		float mixWidth = (1.0f - m_innerWidthFactor) * (m_halfWidth - currentRadius);
		float mixHeight = (1.0f - m_innerHeightFactor) * (m_halfHeight - currentRadius);
		if(m_lockCornerStart)
		{
			if (m_lockWidthSize == 0.0f)
				m_lockWidthSize = mixWidth;

			if (m_lockHeightSize == 0.0f)
				m_lockHeightSize = mixHeight;

			m_innerWidthFactor = 1.0f - m_lockWidthSize / (m_halfWidth - currentRadius);
			m_innerHeightFactor = 1.0f - m_lockHeightSize / (m_halfHeight - currentRadius);
		}
		else
		{
			m_lockWidthSize = 0.0f;
			m_lockHeightSize = 0.0f;
		}

		if(m_connectInner)
		{
			m_innerHeightFactor = Math::clamp(1.0f - mixWidth / (m_halfHeight - currentRadius), 0.0f, 1.0f);

			m_innerWidthFactor = Math::clamp(m_innerWidthFactor, 0.0f, 1.0f);
		}

		if(m_lockRelativeRatio)
		{
			float l1 = m_relativeRatio1 * currentRadius;
			float l2 = m_relativeRatio2 * currentRadius;

			m_innerWidthFactor = 1.0f - l1 / (m_halfWidth - currentRadius);
			m_innerHeightFactor = 1.0f - l2 / (m_halfHeight - currentRadius);
		}
		else
		{
			float l1 = (1.0f - m_innerWidthFactor) * (m_halfWidth - currentRadius);
			m_relativeRatio1 = l1 / currentRadius;

			float l2 = (1.0f - m_innerHeightFactor) * (m_halfHeight - currentRadius);
			m_relativeRatio2 = l2 / currentRadius;
		}
		

		p01.set(m_innerWidthFactor * (m_halfWidth - currentRadius), m_halfHeight);
		p10.set(m_halfWidth, m_innerHeightFactor * (m_halfHeight - currentRadius));

		float round = Math::radians(45.0f * m_cornerPercentage);
		roundStartRadians = Math::radians(45.0f) - round;
		roundEndRadians = Math::radians(45.0f) + round;

		startRoundedPos = currentRadius * Vector2(std::cos(roundStartRadians), std::sin(roundStartRadians));
		endRoundedPos = currentRadius * Vector2(std::cos(roundEndRadians), std::sin(roundEndRadians));

		startRoundedPos += roundCenter;
		endRoundedPos += roundCenter;

		Vector2 endDir = roundCenter - endRoundedPos;
		endDir = endDir.normal().perpendicular();
		float t = (m_halfHeight - endRoundedPos.y) / endDir.y;
		b1P10 = endRoundedPos + t * endDir;

		endDir = roundCenter - startRoundedPos;
		endDir = endDir.normal().perpendicular();
		t = (m_halfWidth - startRoundedPos.x) / endDir.x;
		b2P10 = startRoundedPos + t * endDir;

		Vector2 P32 = endRoundedPos - b1P10;
		Vector2 P02 = p01 - b1P10;
		float u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (currentRadius * std::abs(P32.cross(P02))));
		b1P01 = p01 * u + (1.0f - u) * b1P10;

		P32 = startRoundedPos - b2P10;
		P02 = p10 - b2P10;
		u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (currentRadius * std::abs(P32.cross(P02))));
		b2P01 = p10 * u + (1.0f - u) * b2P10;
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
		scaleK = 1.0f * k;
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

		m_bezier1.setCount(m_bezierCount);
		m_bezier2.setCount(m_bezierCount);

		m_bezier1.setPoints(p01, b1P01, b1P10, endRoundedPos);
		m_bezier2.setPoints(p10, b2P01, b2P10, startRoundedPos);

		bezierCurve1 = m_bezier1.curvePoints();
		bezierCurve2 = m_bezier2.curvePoints();

		for (auto iter = bezierCurve1.begin(); iter != bezierCurve1.end() - 1; ++iter)
			g2Vertices.push_back(*iter);

		for (auto iter = roundCurvatureStart.rbegin(); iter != roundCurvatureStart.rend(); ++iter)
			g2Vertices.push_back(*iter);

		for (auto&& iter = bezierCurve2.rbegin() + 1; iter != bezierCurve2.rend(); ++iter)
			g2Vertices.push_back(*iter);


		g2Vertices.push_back(p11);


		if (m_showBezierCurvature)
		{
			bezierCurvature1.clear();
			bezierCurvature2.clear();

			bezierCurvature1 = m_bezier1.curvaturePoints();
			bezierCurvature2 = m_bezier2.curvaturePoints();
		}
	}

	void CurveScene::computeG3Vertices()
	{
		g3Vertices.clear();
		g3Vertices.push_back(p00);

		m_rationalCubicBezier1.setPoints(p01, b1P01, b1P10, endRoundedPos);
		m_rationalCubicBezier2.setPoints(p10, b2P01, b2P10, startRoundedPos);

		if (m_numOptimize)
		{

			{
				//optimize w_0 to make curvature variance pointAt P_3 close to zero
				float k0 = m_rationalCubicBezier1.curvatureAt(0.0f);
				float k1 = m_rationalCubicBezier1.curvatureAt(1.0f);

				float t2 = static_cast<float>(m_bezierCount - 1) / static_cast<float>(m_bezierCount);
				float k2 = m_rationalCubicBezier1.curvatureAt(t2);

				float bezierStep = 1.0f / static_cast<float>(m_bezierCount);


				float residual1 = (k0 - m_rationalCubicBezier1.curvatureAt(bezierStep)) / bezierStep;
				float residual2 = (k1 - k2) / bezierStep;

				if (residual2 > 0)
					rationalBezierWeight1[2] *=  (1.0f + m_optimizeSpeed);
				else
					rationalBezierWeight1[2] *=  (1.0f - m_optimizeSpeed);

				float residual3 = k1 - scaleK;

				if (residual3 > 0)
					rationalBezierWeight1[3] *=  0.9999f;
				else
					rationalBezierWeight1[3] *=  1.0001f;

				APP_INFO("res1:{}, res2:{}, res3:{}, w0:{}", residual1, residual2, residual3, m_rationalCubicBezier1.weightAt(0))

			}
		}

		m_rationalCubicBezier1.setWeights(rationalBezierWeight1[0], rationalBezierWeight1[1], rationalBezierWeight1[2], rationalBezierWeight1[3]);
		m_rationalCubicBezier2.setWeights(rationalBezierWeight2[0], rationalBezierWeight2[1], rationalBezierWeight2[2], rationalBezierWeight2[3]);

		m_rationalCubicBezier1.setCount(m_bezierCount);
		m_rationalCubicBezier2.setCount(m_bezierCount);

		rationalBezierCurve1 = m_rationalCubicBezier1.curvePoints();
		rationalBezierCurve2 = m_rationalCubicBezier2.curvePoints();

		for (auto iter = rationalBezierCurve1.begin(); iter != rationalBezierCurve1.end() - 1; ++iter)
			g3Vertices.push_back(*iter);

		for (auto iter = roundCurvatureStart.rbegin(); iter != roundCurvatureStart.rend(); ++iter)
			g3Vertices.push_back(*iter);

		for (auto iter = rationalBezierCurve2.rbegin() + 1; iter != rationalBezierCurve2.rend(); ++iter)
			g3Vertices.push_back(*iter);


		g3Vertices.push_back(p11);



		if (m_showG3Curvature)
		{
			rationalBezierCurvature1.clear();
			rationalBezierCurvature2.clear();

			rationalBezierCurvature1 = m_rationalCubicBezier1.curvaturePoints();
			rationalBezierCurvature2 = m_rationalCubicBezier2.curvaturePoints();
		}

	}
}
