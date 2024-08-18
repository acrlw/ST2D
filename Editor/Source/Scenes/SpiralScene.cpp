#include "SpiralScene.h"
#include <gsl/gsl_poly.h>


namespace STEditor
{
	SpiralScene::~SpiralScene()
	{

	}

	void SpiralScene::onLoad()
	{
		m_spiralCurve.setSpiralShape(&m_oneWeight);
	}

	void SpiralScene::onUnLoad()
	{

	}

	void SpiralScene::onUpdate(float deltaTime)
	{
		computeReference();
		computeSpiral();
		computeG1();
		computeG2();

	}

	void SpiralScene::onRender(sf::RenderWindow& window)
	{
		drawReference(window);
		if(m_showG1Continuity)
			drawG1(window);

		drawSpiral(window);

		if(m_showCurvatureBezier)
		{
			m_bezier.setCount(N);
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(0), RenderConstant::Gray, 4.0);
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(1), RenderConstant::Gray, 4.0);
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(2), RenderConstant::Gray, 4.0);
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(3), RenderConstant::Gray, 4.0);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_bezier.pointAt(0), m_bezier.pointAt(1), RenderConstant::Gray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_bezier.pointAt(2), m_bezier.pointAt(3), RenderConstant::Gray);
			RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_bezier.curvePoints(), RenderConstant::Blue);

			Vector2 x = m_bezier.pointAt(3);
			Vector2 y = m_bezier.pointAt(3);
			x.y = 0;
			y.x = 0;
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_bezier.pointAt(3), x, RenderConstant::Gray);
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_bezier.pointAt(3), y, RenderConstant::Gray);
		}

		if(m_showG2Continuity)
			drawG2(window);
	}

	void SpiralScene::onRenderUI()
	{
		ImGui::Begin("Spiral");

		ImGui::SeparatorText("Spiral Params");

		ImGui::DragInt("Sample Count", &N, 1, 10, 10000);

		ImGui::Combo("Continuity", &m_currentShapeIndex, "G2\0G3\0G4\0G5\0G6\0G7\0G3 - One Weight Bezier\0G3 - Two Weights Bezier\0");

		if(m_currentShapeIndex == 0)
		{
			m_spiralCurve.setSpiralShape(&m_g2Spiral);
		}
		else if(m_currentShapeIndex == 1)
		{
			m_spiralCurve.setSpiralShape(&m_g3Spiral);
		}
		else if (m_currentShapeIndex == 2)
		{
			m_spiralCurve.setSpiralShape(&m_g4Spiral);
		}
		else if (m_currentShapeIndex == 3)
		{
			m_spiralCurve.setSpiralShape(&m_g5Spiral);
		}
		else if (m_currentShapeIndex == 4)
		{
			m_spiralCurve.setSpiralShape(&m_g6Spiral);
		}
		else if (m_currentShapeIndex == 5)
		{
			m_spiralCurve.setSpiralShape(&m_g7Spiral);
		}
		else if(m_currentShapeIndex == 6)
		{
			m_spiralCurve.setSpiralShape(&m_oneWeight);
			real L = m_oneWeight.computeArcLength();
			real Cf = 1.0f / m_currentRadius;
			m_bezier.setPoints({}, { m_weight * L, 0 }, { (1.0 - m_weight) * L, Cf }, { L, Cf });
		}
		else if(m_currentShapeIndex == 7)
		{
			m_spiralCurve.setSpiralShape(&m_twoWeight);
			real L = m_twoWeight.computeArcLength();
			real Cf = 1.0f / m_currentRadius;
			m_bezier.setPoints({}, { m_weight1 * L, 0 }, { (1.0 - m_weight2) * L, Cf }, { L, Cf });
		}

		ImGui::SeparatorText("One Weight Bezier");
		ImGui::DragFloat("Weight", &m_weight, 0.01f, 0.35f, 1.0f);
		ImGui::SeparatorText("Two Weights Bezier");
		ImGui::DragFloat("Weight1", &m_weight1, 0.001f, 0.01f, 1.0f);
		ImGui::DragFloat("Weight2", &m_weight2, 0.001f, 0.01f, 1.0f);


		if(!m_spiral.empty())
			ImGui::Text("End Offset: (%f, %f)", m_spiral.front().x, m_spiral.front().y);
		ImGui::Text("Radius: %f", m_currentRadius);

		ImGui::SeparatorText("Rounded Params");
		ImGui::DragFloat("Half Width", &m_halfWidth, 0.01f, 1.0f, 50.0f);
		ImGui::DragFloat("Half Height", &m_halfHeight, 0.01f, 1.0f, 50.0f);
		ImGui::DragFloat("Rounded Radius Percentage", &m_percentage, 0.001f, 0.001f, 1.0f);
		ImGui::Checkbox("Connect Inner", &m_connectInner);
		ImGui::SameLine();
		ImGui::Checkbox("Lock Corner Start", &m_lockCornerStart);
		ImGui::SameLine();
		ImGui::Checkbox("Lock Radius", &m_lockRadius);

		ImGui::DragFloat("Inner Width Percentage", &m_innerWidthFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Inner Height Percentage", &m_innerHeightFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Corner Angle Percentage", &m_cornerPercentage, 0.001f, 0.0f, 0.999f);
		ImGui::DragInt("Circle Segment Count", &m_circleSegment, 2, 4, 500);
		ImGui::DragFloat("Curvature Scale Factor", &m_curvatureScaleFactor, 0.01f, 0.01f, 1.0f);



		m_oneWeight.setWeight(m_weight);
		m_twoWeight.setWeights(m_weight1, m_weight2);

		ImGui::Text("Spiral Arc Length: %f", m_L);

		ImGui::SeparatorText("Visibility");
		ImGui::Checkbox("Show G1 Continuity", &m_showG1Continuity);
		ImGui::Checkbox("Show G2 Continuity", &m_showG2Continuity);
		ImGui::Checkbox("Show Spiral", &m_showSpiral);
		ImGui::Checkbox("Show Reference Line", &m_showReferenceLine);
		ImGui::Checkbox("Show Rounded Curvature", &m_showRoundedCurvature);
		ImGui::Checkbox("Show G2 Curvature", &m_showG2Curvature);
		ImGui::Checkbox("Show Spiral Curvature", &m_showSpiralCurvature);
		ImGui::Checkbox("Show Curvature Bezier", &m_showCurvatureBezier);
		ImGui::Checkbox("Curvature Flip", &m_curvatureFlip);
		ImGui::Checkbox("Draw Spiral Connector", &m_drawSpiralConnector);
		ImGui::DragFloat("Thickness", &m_thickness, 0.05f, 0.1f, 20.0f);

		if(ImGui::Button("Fit Inner") && !m_spiral.empty())
		{
			Vector2 p = m_spiral.front();
			m_innerWidthFactor = p.x / (m_halfWidth - m_currentRadius);
			m_innerHeightFactor = m_innerWidthFactor;
		}


		ImGui::End();
	}

	void SpiralScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void SpiralScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void SpiralScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void SpiralScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void SpiralScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void SpiralScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}

	void SpiralScene::computeReference()
	{
		m_p00.set(0, m_halfHeight);
		m_p11.set(m_halfWidth, 0);

		float min = std::min(m_halfWidth, m_halfHeight);
		float maxRadius = min * m_percentage;

		if(!m_lockRadius)
			m_currentRadius = maxRadius * m_percentage;

		m_roundCenter.set(m_halfWidth - m_currentRadius, m_halfHeight - m_currentRadius);
		m_roundCorner.set(m_currentRadius * std::cos(Math::radians(45)),
			m_currentRadius * std::sin(Math::radians(45)));

		m_roundCorner += m_roundCenter;

		float mixWidth = (1.0f - m_innerWidthFactor) * (m_halfWidth - m_currentRadius);
		float mixHeight = (1.0f - m_innerHeightFactor) * (m_halfHeight - m_currentRadius);
		if (m_lockCornerStart)
		{
			if (m_lockWidthSize == 0.0f)
				m_lockWidthSize = mixWidth;

			if (m_lockHeightSize == 0.0f)
				m_lockHeightSize = mixHeight;

			m_innerWidthFactor = 1.0f - m_lockWidthSize / (m_halfWidth - m_currentRadius);
			m_innerHeightFactor = 1.0f - m_lockHeightSize / (m_halfHeight - m_currentRadius);
		}
		else
		{
			m_lockWidthSize = 0.0f;
			m_lockHeightSize = 0.0f;
		}

		if (m_connectInner)
		{
			m_innerHeightFactor = Math::clamp(1.0f - mixWidth / (m_halfHeight - m_currentRadius), 0.0f, 1.0f);

			m_innerWidthFactor = Math::clamp(m_innerWidthFactor, 0.0f, 1.0f);
		}


		m_p01.set(m_innerWidthFactor * (m_halfWidth - m_currentRadius), m_halfHeight);
		m_p10.set(m_halfWidth, m_innerHeightFactor * (m_halfHeight - m_currentRadius));

		float round = Math::radians(45.0f * m_cornerPercentage);
		m_roundStartRadians = Math::radians(45.0f) - round;
		m_roundEndRadians = Math::radians(45.0f) + round;

		m_startRoundedPos = m_currentRadius * Vector2(std::cos(m_roundStartRadians), std::sin(m_roundStartRadians));
		m_endRoundedPos = m_currentRadius * Vector2(std::cos(m_roundEndRadians), std::sin(m_roundEndRadians));

		m_startRoundedPos += m_roundCenter;
		m_endRoundedPos += m_roundCenter;

		Vector2 endDir = m_roundCenter - m_endRoundedPos;
		endDir = endDir.normal().perpendicular();
		float t = (m_halfHeight - m_endRoundedPos.y) / endDir.y;
		m_b1P10 = m_endRoundedPos + t * endDir;

		endDir = m_roundCenter - m_startRoundedPos;
		endDir = endDir.normal().perpendicular();
		t = (m_halfWidth - m_startRoundedPos.x) / endDir.x;
		m_b2P10 = m_startRoundedPos + t * endDir;

		Vector2 P32 = m_endRoundedPos - m_b1P10;
		Vector2 P02 = m_p01 - m_b1P10;
		float u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (m_currentRadius * std::abs(P32.cross(P02))));
		m_b1P01 = m_p01 * u + (1.0f - u) * m_b1P10;

		P32 = m_startRoundedPos - m_b2P10;
		P02 = m_p10 - m_b2P10;
		u = 1.5f * (std::pow(P32.dot(P32), 1.5f) / (m_currentRadius * std::abs(P32.cross(P02))));
		m_b2P01 = m_p10 * u + (1.0f - u) * m_b2P10;

		//m_spiralCircleCenter.x = m_roundCenter.x;
		//m_spiralCircleCenter.y = m_currentRadius;

		m_spiralStartRadians = Math::radians(-45.0f) - Math::radians(45.0f * m_cornerPercentage);
		m_spiralEndRadians = Math::radians(-45.0f) + Math::radians(45.0f * m_cornerPercentage);
		m_spiralStartRoundedPos = m_currentRadius * Vector2(std::cos(m_spiralStartRadians), std::sin(m_spiralStartRadians));
		m_spiralEndRoundedPos = m_currentRadius * Vector2(std::cos(m_spiralEndRadians), std::sin(m_spiralEndRadians));

		m_spiralAxialSymmetryDir.set(std::cos(Math::radians(-45.0f)), std::sin(Math::radians(-45.0f)));
		

	}

	void SpiralScene::computeSpiral()
	{

		//transform += m_p01;

		Vector2 tangent = Vector2(std::cos(m_spiralStartRadians), std::sin(m_spiralStartRadians)).perpendicular();
		real C_f = 1.0f / m_currentRadius;
		real a_c = std::acos(tangent.x);

		m_spiralCurve.spiralShape()->setParam(C_f, a_c);
		m_spiralCurve.setCount(N);
		
		m_spiral = m_spiralCurve.curvePoints();
		m_spiralCurvatureStart = m_spiral;
		m_spiralCurvatureEnd = m_spiralCurve.curvaturePoints();

		Vector2 back = m_spiral.back();
		Vector2 transform(0, -m_halfHeight);

		Vector2 offset = back - m_currentRadius * Vector2(std::cos(m_spiralStartRadians), std::sin(m_spiralStartRadians));

		m_spiralCircleCenter = offset;

		m_spiralSymmetry.clear();
		m_spiralSymmetryCurvatureStart.clear();
		m_spiralSymmetryCurvatureEnd.clear();

		Vector2 end = GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, {0,0});
		transform += Vector2(m_halfWidth - end.x, 0);

		offset += transform;

		m_spiralCircleCenter = offset;
		m_spiralStartRoundedPos += offset;
		m_spiralEndRoundedPos += offset;
		for (int i = 0; i < N; ++i)
		{
			m_spiral[i] += transform;
			m_spiralSymmetry.emplace_back(GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, m_spiral[i]));

			Vector2 p1 = m_spiralCurvatureStart[i] + transform;
			Vector2 p2 = m_spiralCurvatureEnd[i] + transform;

			Vector2 np1 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0.0 }, p1);
			Vector2 np2 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0.0 }, p2);


			m_spiralCurvatureStart[i] = p1;
			m_spiralCurvatureEnd[i] = p2;

			if(m_curvatureFlip)
			{
				m_spiralCurvatureStart[i] = np1;
				m_spiralCurvatureEnd[i] = np2;
			}

			p1 = GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, p1);
			p2 = GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, p2);

			if (m_curvatureFlip)
			{
				p1 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0.0 }, p1);
				p2 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0.0 }, p2);
			}

			m_spiralSymmetryCurvatureStart.emplace_back(p1);
			m_spiralSymmetryCurvatureEnd.emplace_back(p2);

		}

		
	}

	void SpiralScene::computeG1()
	{
		m_g1Vertices.clear();
		m_g1Vertices.push_back(m_p11);

		float step = Constant::Pi * 0.5f / static_cast<float>(m_circleSegment);

		for (int i = 0; i <= m_circleSegment; ++i)
		{
			float angle = static_cast<float>(i) * step;
			Vector2 p(m_currentRadius * std::cos(angle), m_currentRadius * std::sin(angle));
			p += m_roundCenter;

			m_g1Vertices.push_back(p);
		}

		m_g1Vertices.push_back(m_p00);

		//for rounded curvature
		float k = 1.0f / m_currentRadius;
		//draw curvature of rounded
		m_roundCurvatureStart.clear();
		m_roundCurvatureEnd.clear();

		float roundedStep = m_roundEndRadians - m_roundStartRadians;
		float roundedCount = static_cast<float>(m_circleSegment) * 0.5f;
		roundedStep /= roundedCount;

		for (float i = 0; i <= roundedCount; i += 1.0f)
		{
			float angle = i * roundedStep + m_roundStartRadians;
			Vector2 from(std::cos(angle), std::sin(angle));
			from = m_currentRadius * from + m_roundCenter;

			Vector2 point(std::cos(angle), std::sin(angle));
			point *= k;
			point += from;

			m_roundCurvatureEnd.push_back(point);
			m_roundCurvatureStart.push_back(from);
		}

		m_spiralRoundCurvatureStart.clear();
		m_spiralRoundCurvatureEnd.clear();

		for (float i = 0; i <= roundedCount; i += 1.0f)
		{
			float angle = i * roundedStep + m_spiralStartRadians;
			Vector2 from(std::cos(angle), std::sin(angle));
			from = m_currentRadius * from + m_spiralCircleCenter;

			Vector2 point(std::cos(angle), std::sin(angle));
			point *= k;
			point += from;

			if(m_curvatureFlip)
			{
				from = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, from);
				point = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, point);
			}

			m_spiralRoundCurvatureStart.push_back(from);
			m_spiralRoundCurvatureEnd.push_back(point);
		}
	}

	void SpiralScene::computeG2()
	{
		m_g2Vertices.clear();
		m_g2Vertices.push_back(m_p00);

		m_g2CubicBezier1.setCount(N);
		m_g2CubicBezier2.setCount(N);

		m_g2CubicBezier1.setPoints(m_p01, m_b1P01, m_b1P10, m_endRoundedPos);
		m_g2CubicBezier2.setPoints(m_p10, m_b2P01, m_b2P10, m_startRoundedPos);

		m_cubicBezierPoints1 = m_g2CubicBezier1.curvePoints();
		m_cubicBezierPoints2 = m_g2CubicBezier2.curvePoints();

		for (auto iter = m_cubicBezierPoints1.begin(); iter != m_cubicBezierPoints1.end() - 1; ++iter)
			m_g2Vertices.push_back(*iter);

		for (auto iter = m_roundCurvatureStart.rbegin(); iter != m_roundCurvatureStart.rend(); ++iter)
			m_g2Vertices.push_back(*iter);

		for (auto&& iter = m_cubicBezierPoints2.rbegin() + 1; iter != m_cubicBezierPoints2.rend(); ++iter)
			m_g2Vertices.push_back(*iter);


		m_g2Vertices.push_back(m_p11);


		if (m_showG2Curvature)
		{
			m_cubicBezierCurvatureStart1.clear();
			m_cubicBezierCurvatureStart2.clear();
			m_cubicBezierCurvatureEnd1.clear();
			m_cubicBezierCurvatureEnd2.clear();

			m_cubicBezierCurvatureStart1 = m_cubicBezierPoints1;
			m_cubicBezierCurvatureStart2 = m_cubicBezierPoints2;

			m_cubicBezierCurvatureEnd1 = m_g2CubicBezier1.curvaturePoints();
			m_cubicBezierCurvatureEnd2 = m_g2CubicBezier2.curvaturePoints();
		}
	}

	void SpiralScene::drawG1(sf::RenderWindow& window)
	{
		RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p00, m_p01, RenderConstant::Green);
		RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p11, m_p10, RenderConstant::Green);

		RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_roundCurvatureStart, RenderConstant::Green);


		if(m_showRoundedCurvature)
			drawCurvature(window, m_roundCurvatureStart, m_roundCurvatureEnd, RenderConstant::Gray, false);
	}

	void SpiralScene::drawG2(sf::RenderWindow& window)
	{
		drawCurve(window, m_g2Vertices, RenderConstant::Green);
		if(m_showG2Curvature)
		{
			drawCurvature(window, m_cubicBezierCurvatureStart1, m_cubicBezierCurvatureEnd1, RenderConstant::Green, false);
			drawCurvature(window, m_cubicBezierCurvatureStart2, m_cubicBezierCurvatureEnd2, RenderConstant::Green, true);
			drawCurvature(window, m_roundCurvatureStart, m_roundCurvatureEnd, RenderConstant::Green, false);
		}
	}

	void SpiralScene::drawSpiral(sf::RenderWindow& window)
	{
		if(m_spiral.size() < 2)
			return;

		//RenderSFMLImpl::renderLine(window, *m_settings.camera, { 0, -1 }, m_spiral.front(), RenderConstant::Green);
		//RenderSFMLImpl::renderLine(window, *m_settings.camera, { 1, 0 }, m_spiralSymmetry.front(), RenderConstant::Green);


		//Vector2 v = -m_endRoundedPos.perpendicular();
		//RenderSFMLImpl::renderArrow(window, *m_settings.camera, {}, v, RenderConstant::Red);

		auto color = RenderConstant::Yellow;

		if(m_showSpiral)
		{
			if(m_drawSpiralConnector)
			{

				Vector2 p0 = m_spiral[0];
				Vector2 p2 = m_spiralSymmetry[0];

				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { 0, -m_halfHeight }, p0, color, m_thickness);
				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { m_halfWidth, 0 }, p2, color, m_thickness);

				p0.x = -p0.x;
				p2.x = -p2.x;

				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { 0, -m_halfHeight }, p0, color, m_thickness);
				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { -m_halfWidth, 0 }, p2, color, m_thickness);

				p0.y = -p0.y;
				p2.y = -p2.y;

				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { 0, m_halfHeight }, p0, color, m_thickness);
				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { -m_halfWidth, 0 }, p2, color, m_thickness);

				p0.x = -p0.x;
				p2.x = -p2.x;

				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { 0, m_halfHeight }, p0, color, m_thickness);
				RenderSFMLImpl::renderThickLine(window, *m_settings.camera, { m_halfWidth, 0 }, p2, color, m_thickness);
			}

			//std::vector<Vector2> curve;
			//curve.reserve(m_spiral.size() + m_spiralSymmetry.size() + m_spiralRoundCurvatureStart.size() );
			//for(auto&& p : m_spiral)
			//	curve.push_back(p);

			//for (auto&& p : m_spiralRoundCurvatureStart)
			//	curve.push_back(p);

			//for(auto iter = m_spiralSymmetry.rbegin(); iter != m_spiralSymmetry.rend(); ++iter)
			//	curve.push_back(*iter);

			//RenderSFMLImpl::renderPolyDashedThickLine(window, *m_settings.camera, curve, color, m_thickness, 0.01f, 0.02f);



			drawCurve(window, m_spiral, color);
			drawCurve(window, m_spiralSymmetry, color);
			drawCurve(window, m_spiralRoundCurvatureStart, color);

		}


		if (m_showSpiralCurvature)
		{
			drawCurvature(window, m_spiralCurvatureStart, m_spiralCurvatureEnd, color, false);
			drawCurvature(window, m_spiralSymmetryCurvatureStart, m_spiralSymmetryCurvatureEnd, color, false);

			Vector2 p0 = m_spiralCircleCenter;
			Vector2 p1 = m_spiralStartRoundedPos;
			Vector2 p2 = m_spiralEndRoundedPos;

			if(m_curvatureFlip)
			{
				p0 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p0);
				p1 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p1);
				p2 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p2);
			}
			 
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, p0, RenderConstant::Gray, 3);

			//RenderSFMLImpl::renderArrow(window, *m_settings.camera, m_spiralStartRoundedPos, m_spiralStartRoundedPos + (m_spiralStartRoundedPos - m_spiralCircleCenter).normal().perpendicular(), RenderConstant::Red, 0.1);
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, p0, p1, RenderConstant::Gray, 0.01, 0.01);
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, p0, p2, RenderConstant::Gray, 0.01, 0.01);

			drawCurvature(window, m_spiralRoundCurvatureStart, m_spiralRoundCurvatureEnd, color, false);
		}
	}

	void SpiralScene::drawReference(sf::RenderWindow& window)
	{
		if (m_showReferenceLine)
		{
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_roundCenter, m_roundCorner, RenderConstant::LightGray);
			Vector2 ref1(m_p01.x, (m_halfHeight - m_currentRadius) * m_innerHeightFactor);
			Vector2 ref2((m_halfWidth - m_currentRadius) * m_innerWidthFactor, m_p10.y);
			Vector2 ref3, ref4;

			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p01, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p10, ref2, RenderConstant::LightGray);


			ref1 = Vector2(m_halfWidth, m_halfHeight - m_currentRadius);
			ref2 = Vector2(m_p01.x, m_halfHeight - m_currentRadius);
			ref3 = Vector2(m_halfWidth - m_currentRadius, m_halfHeight);
			ref4 = Vector2(m_halfWidth - m_currentRadius, m_p10.y);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref1, ref2, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, ref3, ref4, RenderConstant::LightGray);

			ref1 = Vector2((m_halfWidth - m_currentRadius) * m_innerWidthFactor, m_roundCorner.y);
			ref2 = Vector2(m_roundCorner.x, (m_halfHeight - m_currentRadius) * m_innerHeightFactor);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_roundCorner, ref1, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_roundCorner, ref2, RenderConstant::LightGray);

			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_roundCenter, m_startRoundedPos, RenderConstant::LightGray);
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_roundCenter, m_endRoundedPos, RenderConstant::LightGray);

		}
	}

	void SpiralScene::drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start,
		const std::vector<Vector2>& end, const sf::Color& color, bool flip)
	{
		if (start.size() != end.size())
			return;
		std::vector<Vector2> scaleEnd;
		Vector2 lastEnd;
		for (size_t i = 0; i < start.size(); ++i)
		{
			Vector2 n = end[i] - start[i];
			n = n * m_curvatureScaleFactor;

			if (flip)
				n = -n;

			Vector2 newEnd = n + start[i];
			RenderSFMLImpl::renderLine(window, *m_settings.camera, start[i], newEnd, color);


			if (i != 0)
			{
				RenderSFMLImpl::renderLine(window, *m_settings.camera, lastEnd
				                           , newEnd, color);
			}
			scaleEnd.push_back(newEnd);

			lastEnd = newEnd;
		}

	}

	void SpiralScene::drawCurve(sf::RenderWindow& window, const std::vector<Vector2>& curve,
		const sf::Color& color) const
	{
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
}


