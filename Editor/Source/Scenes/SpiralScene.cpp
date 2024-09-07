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

	void SpiralScene::onRender(Renderer2D& renderer)
	{
		drawReference(renderer);
		if (m_showG1Continuity)
			drawG1(renderer);

		drawSpiral(renderer);

		if (m_showCurvatureBezier)
		{
			m_bezier.setCount(N);
			//RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(0), RenderConstant::Gray, 4.0);
			//RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(1), RenderConstant::Gray, 4.0);
			//RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(2), RenderConstant::Gray, 4.0);
			//RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_bezier.pointAt(3), RenderConstant::Gray, 4.0);

			//RenderSFMLImpl::renderLine(window, *m_settings.camera, m_bezier.pointAt(0), m_bezier.pointAt(1), RenderConstant::Gray);
			//RenderSFMLImpl::renderLine(window, *m_settings.camera, m_bezier.pointAt(2), m_bezier.pointAt(3), RenderConstant::Gray);
			//RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_bezier.curvePoints(), RenderConstant::Blue);

			Vector2 x = m_bezier.pointAt(3);
			Vector2 y = m_bezier.pointAt(3);
			x.y = 0;
			y.x = 0;
			//RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_bezier.pointAt(3), x, RenderConstant::Gray);
			//RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_bezier.pointAt(3), y, RenderConstant::Gray);
		}

		if (m_showG2Continuity)
			drawG2(renderer);
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
			m_bezier.setPoints({}, { m_weight * L, 0 }, { (1.0f - m_weight) * L, Cf }, { L, Cf });
		}
		else if(m_currentShapeIndex == 7)
		{
			m_spiralCurve.setSpiralShape(&m_twoWeight);
			real L = m_twoWeight.computeArcLength();
			real Cf = 1.0f / m_currentRadius;
			m_bezier.setPoints({}, { m_weight1 * L, 0 }, { (1.0f - m_weight2) * L, Cf }, { L, Cf });
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

		ImGui::Columns(2);
		ImGui::Checkbox("Connect Inner", &m_connectInner);
		ImGui::Checkbox("Lock Corner Start", &m_lockCornerStart);
		ImGui::NextColumn();
		ImGui::Checkbox("Lock Relative Ratio", &m_lockRelativeRatio);
		ImGui::Checkbox("Lock Radius", &m_lockRadius);
		ImGui::Columns(1);

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
		ImGui::DragFloat("Thickness", &m_thickness, 0.05f, 0.1f, 50.0f);

		if(ImGui::Button("Fit Inner") && !m_spiral.empty())
		{
			Vector2 p = m_spiral.front();
			m_innerWidthFactor = p.x / (m_halfWidth - m_currentRadius);
			m_innerHeightFactor = m_innerWidthFactor;
		}


		ImGui::End();
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

		if (m_lockRelativeRatio)
		{
			float l1 = m_relativeRatio1 * m_currentRadius;
			float l2 = m_relativeRatio2 * m_currentRadius;

			m_innerWidthFactor = 1.0f - l1 / (m_halfWidth - m_currentRadius);
			m_innerHeightFactor = 1.0f - l2 / (m_halfHeight - m_currentRadius);
		}
		else
		{
			float l1 = (1.0f - m_innerWidthFactor) * (m_halfWidth - m_currentRadius);
			m_relativeRatio1 = l1 / m_currentRadius;

			float l2 = (1.0f - m_innerHeightFactor) * (m_halfHeight - m_currentRadius);
			m_relativeRatio2 = l2 / m_currentRadius;
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

	void SpiralScene::drawG1(Renderer2D& renderer)
	{
		renderer.line(m_p00, m_p01, Palette::Green);
		renderer.line(m_p11, m_p10, Palette::Green);
		renderer.polyLines(m_roundCurvatureStart, Palette::Green);
		//RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p00, m_p01, RenderConstant::Green);
		//RenderSFMLImpl::renderLine(window, *m_settings.camera, m_p11, m_p10, RenderConstant::Green);

		//RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_roundCurvatureStart, RenderConstant::Green);


		if(m_showRoundedCurvature)
			drawCurvature(renderer, m_roundCurvatureStart, m_roundCurvatureEnd, Palette::Gray, false);
	}

	void SpiralScene::drawG2(Renderer2D& renderer)
	{
		drawCurve(renderer, m_g2Vertices, Palette::Green);
		if(m_showG2Curvature)
		{
			drawCurvature(renderer, m_cubicBezierCurvatureStart1, m_cubicBezierCurvatureEnd1, Palette::Green, false);
			drawCurvature(renderer, m_cubicBezierCurvatureStart2, m_cubicBezierCurvatureEnd2, Palette::Green, true);
			drawCurvature(renderer, m_roundCurvatureStart, m_roundCurvatureEnd, Palette::Green, false);
		}
	}

	void SpiralScene::drawSpiral(Renderer2D& renderer)
	{
		if(m_spiral.size() < 2)
			return;

		//RenderSFMLImpl::renderLine(window, *m_settings.camera, { 0, -1 }, m_spiral.front(), RenderConstant::Green);
		//RenderSFMLImpl::renderLine(window, *m_settings.camera, { 1, 0 }, m_spiralSymmetry.front(), RenderConstant::Green);


		//Vector2 v = -m_endRoundedPos.perpendicular();
		//RenderSFMLImpl::renderArrow(window, *m_settings.camera, {}, v, RenderConstant::Red);

		auto color = Palette::Yellow;

		if(m_showSpiral)
		{
			if(m_drawSpiralConnector)
			{

				Vector2 p0 = m_spiral[0];
				Vector2 p2 = m_spiralSymmetry[0];

				renderer.thickLine({ 0, -m_halfHeight }, p0, color, m_thickness);
				renderer.thickLine({ m_halfWidth, 0 }, p2, color, m_thickness);

				p0.x = -p0.x;
				p2.x = -p2.x;

				renderer.thickLine({ 0, -m_halfHeight }, p0, color, m_thickness);
				renderer.thickLine({ -m_halfWidth, 0 }, p2, color, m_thickness);

				p0.y = -p0.y;
				p2.y = -p2.y;

				renderer.thickLine({ 0, m_halfHeight }, p0, color, m_thickness);
				renderer.thickLine({ -m_halfWidth, 0 }, p2, color, m_thickness);

				p0.x = -p0.x;
				p2.x = -p2.x;

				renderer.thickLine({ 0, m_halfHeight }, p0, color, m_thickness);
				renderer.thickLine({ m_halfWidth, 0 }, p2, color, m_thickness);
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

			drawCurve(renderer, m_spiral, color);
			drawCurve(renderer, m_spiralSymmetry, color);
			drawCurve(renderer, m_spiralRoundCurvatureStart, color);

		}


		if (m_showSpiralCurvature)
		{
			drawCurvature(renderer, m_spiralCurvatureStart, m_spiralCurvatureEnd, color, false);
			drawCurvature(renderer, m_spiralSymmetryCurvatureStart, m_spiralSymmetryCurvatureEnd, color, false);

			Vector2 p0 = m_spiralCircleCenter;
			Vector2 p1 = m_spiralStartRoundedPos;
			Vector2 p2 = m_spiralEndRoundedPos;

			if(m_curvatureFlip)
			{
				p0 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p0);
				p1 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p1);
				p2 = GeometryAlgorithm2D::axialSymmetry({}, { 1.0, 0 }, p2);
			}

			renderer.point(p0, Palette::Gray);
			renderer.dashedLine(p0, p1, Palette::Gray, 0.01f, 0.01f);
			renderer.dashedLine(p0, p2, Palette::Gray, 0.01f, 0.01f);
			drawCurvature(renderer, m_spiralRoundCurvatureStart, m_spiralRoundCurvatureEnd, color, false);
		}
	}

	void SpiralScene::drawReference(Renderer2D& renderer)
	{
		if (m_showReferenceLine)
		{
			renderer.line(m_roundCenter, m_roundCorner, Palette::LightGray);
			
			Vector2 ref1(m_p01.x, (m_halfHeight - m_currentRadius) * m_innerHeightFactor);
			Vector2 ref2((m_halfWidth - m_currentRadius) * m_innerWidthFactor, m_p10.y);
			Vector2 ref3, ref4;

			renderer.line(m_p01, ref1, Palette::LightGray);
			renderer.line(m_p10, ref2, Palette::LightGray);

			ref1 = Vector2(m_halfWidth, m_halfHeight - m_currentRadius);
			ref2 = Vector2(m_p01.x, m_halfHeight - m_currentRadius);
			ref3 = Vector2(m_halfWidth - m_currentRadius, m_halfHeight);
			ref4 = Vector2(m_halfWidth - m_currentRadius, m_p10.y);

			renderer.line(ref1, ref2, Palette::LightGray);
			renderer.line(ref3, ref4, Palette::LightGray);

			ref1 = Vector2((m_halfWidth - m_currentRadius) * m_innerWidthFactor, m_roundCorner.y);
			ref2 = Vector2(m_roundCorner.x, (m_halfHeight - m_currentRadius) * m_innerHeightFactor);

			renderer.line(m_roundCenter, ref1, Palette::LightGray);
			renderer.line(m_roundCenter, ref2, Palette::LightGray);
			renderer.line(m_roundCenter, m_startRoundedPos, Palette::LightGray);
			renderer.line(m_roundCenter, m_endRoundedPos, Palette::LightGray);

		}
	}

	void SpiralScene::drawCurvature(Renderer2D& renderer, const std::vector<Vector2>& start,
		const std::vector<Vector2>& end, const Color& color, bool flip)
	{
		if (start.size() != end.size())
			return;
		std::vector<Vector2> scaleEnd;
		for (size_t i = 0; i < start.size(); ++i)
		{
			Vector2 n = end[i] - start[i];
			n = n * m_curvatureScaleFactor;

			if (flip)
				n = -n;

			Vector2 newEnd = n + start[i];

			renderer.line(start[i], newEnd, color);
			scaleEnd.push_back(newEnd);
		}
		renderer.polyLines(scaleEnd, color);
	}

	void SpiralScene::drawCurve(Renderer2D& renderer, const std::vector<Vector2>& curve, const Color& color) const
	{
		std::vector<Vector2> quadrant2 = curve;
		std::vector<Vector2> quadrant3 = curve;
		std::vector<Vector2> quadrant4 = curve;

		std::ranges::transform(quadrant2, quadrant2.begin(), [](const Vector2& v) { return Vector2(-v.x, v.y); });
		std::ranges::transform(quadrant3, quadrant3.begin(), [](const Vector2& v) { return Vector2(-v.x, -v.y); });
		std::ranges::transform(quadrant4, quadrant4.begin(), [](const Vector2& v) { return Vector2(v.x, -v.y); });

		renderer.polyThickLine(curve, color, m_thickness);
		renderer.polyThickLine(quadrant2, color, m_thickness);
		renderer.polyThickLine(quadrant3, color, m_thickness);
		renderer.polyThickLine(quadrant4, color, m_thickness);
	}
}


