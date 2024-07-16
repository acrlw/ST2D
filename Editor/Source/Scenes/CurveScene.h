#pragma once

#include "AbstractScene.h"
#include "Curve.h"

namespace STEditor
{
	class CurveScene : public AbstractScene
	{
	public:
		explicit CurveScene(const SceneSettings& settings)
			: AbstractScene(settings, "CurveScene")
		{
		}

		~CurveScene() override;
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
		void drawCurve(sf::RenderWindow& window, const std::vector<Vector2>& curve, const sf::Color& color) const;

		void drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start, const std::vector<Vector2>& end, const sf::Color& color);

		void computeBasicParam();

		void computeG1Vertices();

		void computeG2Vertices();

		void computeG3Vertices();

		bool m_connectWeights = false;
		bool m_enableCornerSmoothing = false;
		bool m_numOptimize = false;
		bool once = false;
		bool m_showG1 = false;
		bool m_showG2 = true;
		bool m_showG3 = true;
		bool m_showG3Curvature = true;
		bool m_showRoundedCurvature = true;
		bool m_showBezierCurvature = true;
		bool m_showQuinticBezier = false;
		bool m_showQuinticBezierCurvature = false;
		bool m_showReferenceLine = true;

		int m_currentIndex = -1;
		bool m_isMoving = false;
		Vector2 m_mousePos;

		RationalCubicBezier m_rationalCubicBezier2;
		RationalCubicBezier m_rationalCubicBezier1;

		CubicBezier m_bezier1;
		CubicBezier m_bezier2;

		//std::array<Vector2, 4> m_bezierPoints1;
		//std::array<Vector2, 4> m_bezierPoints2;

		float m_cornerSmoothing = 0.6f;
		float m_residualResolution = 1e-5f;
		float m_optimizeSpeed = 1e-5f;
		float m_innerWidthFactor = 0.55f;
		float m_innerHeightFactor = 0.55f;
		float m_curvatureScaleFactor = 0.1f;
		int m_bezierCount = 100;
		int m_count = 100;
		float m_halfWidth = 1.0f;
		float m_halfHeight = 1.0f;
		float m_percentage = 0.681f;
		float m_cornerPercentage = 0.4f;


		std::vector<Vector2> g1Vertices, g2Vertices, g3Vertices;
		std::vector<Vector2> roundCurvatureStart, roundCurvatureEnd;
		std::vector<Vector2> bezierCurve1, bezierCurve2;
		std::vector<Vector2> bezierCurvature1,bezierCurvature2;
		std::vector<Vector2> rationalBezierCurve1, rationalBezierCurve2;
		std::vector<Vector2> rationalBezierCurvature1, rationalBezierCurvature2;
		// shared param for generating curves
		Vector2 p00, p01, p10, p11;
		Vector2 roundCenter, roundCorner;
		Vector2 startRoundedPos, endRoundedPos;
		float currentRadius = 0.0f;
		float roundStartRadians = 0.0f;
		float roundEndRadians = 0.0f;

		float scaleK = 1.0f;
	};
}