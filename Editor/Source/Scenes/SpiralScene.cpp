#include "SpiralScene.h"


namespace STEditor
{
	SpiralScene::~SpiralScene()
	{

	}

	void SpiralScene::onLoad()
	{

	}

	void SpiralScene::onUnLoad()
	{

	}

	void SpiralScene::onUpdate(float deltaTime)
	{
		computeReference();
		computeG1();
		computeSpiral();

		Vector2 p0(0, 0);
		Vector2 p1(m_L * m_weight, 0);
		Vector2 p2((1.0f - m_weight) * m_L, m_alpha * m_L);
		Vector2 p3(m_L, m_alpha * m_L);

		m_bezier.setPoints(p0, p1, p2, p3);
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
	}

	void SpiralScene::onRenderUI()
	{
		ImGui::Begin("Spiral");

		ImGui::SeparatorText("Spiral Params");

		ImGui::DragInt("Spiral Count", &N, 1, 10, 10000);
		ImGui::DragFloat("Spiral Length", &L, 0.01f, 0.01f, 100.0f);
		ImGui::DragFloat("Spiral Alpha", &alpha, 0.01f, 0.01f, 100.0f);

		ImGui::Text("End Offset: (%f, %f)", m_spiral.back().x, m_spiral.back().y);
		ImGui::Text("Radius: %f", m_currentRadius);

		ImGui::SeparatorText("Rounded Params");
		ImGui::DragFloat("Half Width", &m_halfWidth, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Half Height", &m_halfHeight, 0.01f, 0.5f, 50.0f);
		ImGui::DragFloat("Rounded Radius Percentage", &m_percentage, 0.005f, 0.005f, 1.0f);
		ImGui::Checkbox("Connect Inner", &m_connectInner);
		ImGui::SameLine();
		ImGui::Checkbox("Lock Corner Start", &m_lockCornerStart);
		ImGui::DragFloat("Inner Width Percentage", &m_innerWidthFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Inner Height Percentage", &m_innerHeightFactor, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Corner Angle Percentage", &m_cornerPercentage, 0.005f, 0.0f, 0.999f);
		ImGui::DragInt("Circle Segment Count", &m_circleSegment, 2, 4, 500);
		ImGui::DragFloat("Curvature Scale Factor", &m_curvatureScaleFactor, 0.01f, 0.01f, 1.0f);


		ImGui::SeparatorText("Bezier Params");
		ImGui::DragFloat("Weight", &m_weight, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("Alpha", &m_alpha, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("Bezier Arc Length", &m_L, 0.01f, 0.01f, 100.0f);

		ImGui::SeparatorText("Visibility");
		ImGui::Checkbox("Show G1 Continuity", &m_showG1Continuity);
		ImGui::Checkbox("Show Reference Line", &m_showReferenceLine);
		ImGui::Checkbox("Show Rounded Curvature", &m_showRoundedCurvature);
		ImGui::Checkbox("Show Spiral Curvature", &m_showCurvature);
		ImGui::Checkbox("Show Curvature Bezier", &m_showCurvatureBezier);


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

		m_spiralCircleCenter.x = m_roundCenter.x;
		m_spiralCircleCenter.y = m_currentRadius;

		m_spiralStartRadians = Math::radians(-45.0f) - Math::radians(45.0f * m_cornerPercentage);
		m_spiralEndRadians = Math::radians(-45.0f) + Math::radians(45.0f * m_cornerPercentage);
		m_spiralStartRoundedPos = m_currentRadius * Vector2(std::cos(m_spiralStartRadians), std::sin(m_spiralStartRadians));
		m_spiralEndRoundedPos = m_currentRadius * Vector2(std::cos(m_spiralEndRadians), std::sin(m_spiralEndRadians));

		m_spiralStartRoundedPos += m_spiralCircleCenter;
		m_spiralEndRoundedPos += m_spiralCircleCenter;

		m_spiralAxialSymmetryDir.set(std::cos(Math::radians(-45.0f)), std::sin(Math::radians(-45.0f)));
		

	}

	void SpiralScene::computeSpiral()
	{

		//transform += m_p01;

		Vector2 tangent = (m_spiralStartRoundedPos - m_spiralCircleCenter).normal().perpendicular();
		real Cf = 1.0f / m_currentRadius;
		real ac = std::acos(tangent.x);
		L = 2.0f * ac / Cf;
		alpha = Cf / L;

		m_spiral.clear();
		m_spiralCurvaturePoints.clear();

		real ds = L / static_cast<real>(N);
		real lastC = 0.0f;
		real lastS = 0.0f;
		real lastCIntegral = 0.0f;
		real lastSIntegral = 0.0f;
		for(int i = 0;i < N; ++i)
		{
			real s = static_cast<real>(i) * ds; 
			real C = std::cos(arcLengthCurvatureInt(s));
			real S = std::sin(arcLengthCurvatureInt(s));
			//real C = std::cos(alpha * s * s / 2.0f);
			//real S = std::sin(alpha * s * s / 2.0f);
			if(i == 0)
			{
				lastC = C;
				lastS = S;
				m_spiral.emplace_back(Vector2(0,0));
				m_spiralCurvaturePoints.emplace_back(Vector2(0,0));
				continue;
			}
			real mid = s - 0.5f * ds;
			real midC = std::cos(arcLengthCurvatureInt(mid));
			real midS = std::sin(arcLengthCurvatureInt(mid));
			//real midC = std::cos(alpha * mid * mid / 2.0f);
			//real midS = std::sin(alpha * mid * mid / 2.0f);
			real CIntegral = 1.0f / 6.0f * ds * (lastC + 4.0f * midC + C) + lastCIntegral;
			real SIntegral = 1.0f / 6.0f * ds * (lastS + 4.0f * midS + S) + lastSIntegral;

			//real CIntegral = 0.5f * ds * (lastC + C) + lastCIntegral;
			//real SIntegral = 0.5f * ds * (lastS + S) + lastSIntegral;

			m_spiral.emplace_back(Vector2(CIntegral, SIntegral));

			Vector2 normal = Vector2(-S, C);
			real curvature = alpha * s;

			Vector2 curvaturePoint = Vector2(CIntegral, SIntegral) - curvature * normal;
			m_spiralCurvaturePoints.emplace_back(curvaturePoint);

			lastCIntegral = CIntegral;
			lastSIntegral = SIntegral;

			lastC = C;
			lastS = S;
		}

		Vector2 offset = m_spiralStartRoundedPos - m_spiral.back();
		m_spiralSymmetry.clear();
		m_spiralSymmetryCurvaturePoints.clear();
		for (int i = 0; i < N; ++i)
		{
			m_spiral[i] += offset;
			m_spiralCurvaturePoints[i] += offset;
			m_spiralSymmetry.emplace_back(GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, m_spiral[i]));
			m_spiralSymmetryCurvaturePoints.emplace_back(GeometryAlgorithm2D::axialSymmetry(m_spiralCircleCenter, m_spiralAxialSymmetryDir, m_spiralCurvaturePoints[i]));
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

			m_spiralRoundCurvatureStart.push_back(from);
			m_spiralRoundCurvatureEnd.push_back(point);
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

	void SpiralScene::drawSpiral(sf::RenderWindow& window)
	{
		if(m_spiral.size() < 2)
			return;

		

		RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_spiralRoundCurvatureStart, RenderConstant::Green);

		if (m_showRoundedCurvature)
		{
			RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_spiralCircleCenter, RenderConstant::Gray);

			//RenderSFMLImpl::renderArrow(window, *m_settings.camera, m_spiralStartRoundedPos, m_spiralStartRoundedPos + (m_spiralStartRoundedPos - m_spiralCircleCenter).normal().perpendicular(), RenderConstant::Red, 0.1);
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_spiralCircleCenter, m_spiralStartRoundedPos, RenderConstant::Gray, 0.01, 0.01);
			RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, m_spiralCircleCenter, m_spiralEndRoundedPos, RenderConstant::Gray, 0.01, 0.01);

			drawCurvature(window, m_spiralRoundCurvatureStart, m_spiralRoundCurvatureEnd, RenderConstant::Gray, false);
		}

		//Vector2 v = -m_endRoundedPos.perpendicular();
		//RenderSFMLImpl::renderArrow(window, *m_settings.camera, {}, v, RenderConstant::Red);


		RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_spiral, RenderConstant::Green);
		RenderSFMLImpl::renderPolyLine(window, *m_settings.camera, m_spiralSymmetry, RenderConstant::Green);


		if(m_showCurvature)
		{
			drawCurvature(window, m_spiral, m_spiralCurvaturePoints, RenderConstant::Gray, false);
			drawCurvature(window, m_spiralSymmetry, m_spiralSymmetryCurvaturePoints, RenderConstant::Gray, false);
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

			lastEnd = newEnd;
		}
	}

	real SpiralScene::arcLengthCurvature(real s)
	{
		return alpha * s;
		//if (s < 0.5f)
		//	return 4.0f * std::pow(s, 3.0f);

		//return 1.0f - 4.0f * std::pow(1.0f - s, 3.0f);
	}

	real SpiralScene::arcLengthCurvatureInt(real s)
	{
		return alpha * s * s / 2.0f;
		//if(s < 0.5f)
		//	return std::pow(s, 4.0f);

		//return std::pow(0.5, 4.0f) +
		//	s + std::pow(1.0f - s, 4.0f) - (0.5 + std::pow(1.0f - 0.5, 4.0f));
	}
}


