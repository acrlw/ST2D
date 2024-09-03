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
		void onDraw(sf::RenderWindow& window) override;
		void onRenderUI() override;

	private:

		// The single-variable function for which derivatives are needed
		

		void drawCurve(sf::RenderWindow& window, const std::vector<Vector2>& curve, const sf::Color& color) const;

		void drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start, 
			const std::vector<Vector2>& end, const sf::Color& color, bool flip = false);

		void computeBasicParam();

		void computeG1Vertices();

		void computeG2Vertices();

		void computeG3Vertices();

		bool m_lockCornerStart = false;

		bool m_lockRelativeRatio = false;

		float m_lockWidthSize = 0.0f;
		float m_lockHeightSize = 0.0f;

		bool m_lockRadius = false;
		bool m_connectInner = false;
		bool m_connectWeights = false;
		bool m_enableCornerSmoothing = false;
		bool m_numOptimize = false;
		bool once = false;
		bool m_showG1 = false;
		bool m_showG2 = true;
		bool m_showG3 = true;
		bool m_showG3Curvature = false;
		bool m_showRoundedCurvature = true;
		bool m_showReferenceLine = true;
		bool m_showBezierCurvature = false;
		bool m_showQuinticBezier = false;
		bool m_showQuinticBezierCurvature = false;

		int m_currentIndex = -1;
		bool m_isMoving = false;
		Vector2 m_mousePos;

		RationalCubicBezier m_rationalCubicBezier1;
		RationalCubicBezier m_rationalCubicBezier2;

		CubicBezier m_bezier1;
		CubicBezier m_bezier2;

		//std::array<Vector2, 4> m_bezierPoints1;
		//std::array<Vector2, 4> m_bezierPoints2;

		float m_cornerSmoothing = 0.6f;
		float m_residualResolution = 1e-5f;
		float m_optimizeSpeed = 1e-5f;
		float m_innerWidthFactor = 0.678f;
		float m_innerHeightFactor = 0.678f;
		float m_curvatureScaleFactor = 0.1f;
		int m_bezierCount = 8000;
		int m_count = 100;
		float m_halfWidth = 1.0f;
		float m_halfHeight = 1.0f;
		float m_percentage = 0.670f;
		float m_cornerPercentage = 0.367f;
		float m_thickness = 2;

		float m_relativeRatio1 = 0.0f;
		float m_relativeRatio2 = 0.0f;


		std::vector<Vector2> g1Vertices, g2Vertices, g3Vertices;
		std::vector<Vector2> roundCurvatureStart, roundCurvatureEnd;
		std::vector<Vector2> bezierCurve1, bezierCurve2;
		std::vector<Vector2> bezierCurvature1,bezierCurvature2;
		std::vector<Vector2> rationalBezierCurve1, rationalBezierCurve2;
		std::vector<Vector2> rationalBezierCurvature1, rationalBezierCurvature2;

		std::array<float, 4> rationalBezierWeight1 = { 0.205959990620613, 0.274320006370544, 0.136367648839951, 0.067795760929585 };
		std::array<float, 4> rationalBezierWeight2 = { 0.205959990620613, 0.274320006370544, 0.136367648839951, 0.067795760929585 };
		// shared param for generating curves
		Vector2 p00, p01, p10, p11;
		Vector2 b1P01, b1P10, b2P01, b2P10;
		Vector2 roundCenter, roundCorner;
		Vector2 startRoundedPos, endRoundedPos;
		float currentRadius = 0.0f;
		float roundStartRadians = 0.0f;
		float roundEndRadians = 0.0f;

		float scaleK = 1.0f;
	};
}