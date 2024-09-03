#pragma once

#include "AbstractScene.h"
#include "Curve.h"

namespace STEditor
{
	class SpiralScene : public AbstractScene
	{
	public:
		explicit SpiralScene(const SceneSettings& settings)
			: AbstractScene(settings, "SpiralScene")
		{
		}

		~SpiralScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onDraw(sf::RenderWindow& window) override;
		void onRenderUI() override;

	private:
		void computeReference();
		void computeSpiral();
		void computeG1();
		void computeG2();
		void drawG1(sf::RenderWindow& window);
		void drawG2(sf::RenderWindow& window);
		void drawSpiral(sf::RenderWindow& window);
		void drawReference(sf::RenderWindow& window);
		void drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start,
			const std::vector<Vector2>& end, const sf::Color& color, bool flip);

		void drawCurve(sf::RenderWindow& window, const std::vector<Vector2>& curve, const sf::Color& color) const;

		bool m_lockRelativeRatio = false;
		bool m_lockRadius = false;
		bool m_lockCornerStart = false;
		bool m_drawSpiralConnector = true;
		float m_lockWidthSize = 0.0f;
		float m_lockHeightSize = 0.0f;
		bool m_showCurvatureBezier = false;
		bool m_connectInner = false;
		bool m_showRoundedCurvature = false;
		bool m_showReferenceLine = false;
		bool m_showG1Continuity = false;
		bool m_showG2Continuity = false;
		bool m_showSpiral = true;
		bool m_showSpiralCurvature = true;
		bool m_showG2Curvature = false;
		bool m_curvatureFlip = true;

		float m_innerWidthFactor = 0.55f;
		float m_innerHeightFactor = 0.55f;
		float m_curvatureScaleFactor = 0.1f;
		float m_halfWidth = 1.0f;
		float m_halfHeight = 1.0f;
		float m_percentage = 0.638f;
		float m_cornerPercentage = 0.301f;

		float m_relativeRatio1 = 0.0f;
		float m_relativeRatio2 = 0.0f;

		int m_circleSegment = 100;

		Vector2 m_p00, m_p01, m_p10, m_p11;
		Vector2 m_b1P01, m_b1P10, m_b2P01, m_b2P10;
		Vector2 m_roundCenter, m_roundCorner;
		Vector2 m_startRoundedPos, m_endRoundedPos;
		float m_currentRadius = 0.0f;
		float m_roundStartRadians = 0.0f;
		float m_roundEndRadians = 0.0f;


		float m_scaleK = 1.0f;
		float m_thickness = 2.0f;

		int N = 600;

		std::vector<Vector2> m_spiral, m_spiralSymmetry;
		std::vector<Vector2> m_spiralCurvatureStart, m_spiralCurvatureEnd, m_spiralSymmetryCurvatureStart, m_spiralSymmetryCurvatureEnd;

		std::vector<Vector2> m_g1Vertices;
		std::vector<Vector2> m_roundCurvatureStart, m_roundCurvatureEnd;

		std::vector<Vector2> m_g2Vertices;
		std::vector<Vector2> m_g2CurvatureStart, m_g2CurvatureEnd;

		std::vector<Vector2> m_cubicBezierPoints1, m_cubicBezierPoints2;
		std::vector<Vector2> m_cubicBezierCurvatureStart1, m_cubicBezierCurvatureEnd1;
		std::vector<Vector2> m_cubicBezierCurvatureStart2, m_cubicBezierCurvatureEnd2;

		CubicBezier m_g2CubicBezier1, m_g2CubicBezier2;

		float m_spiralStartRadians = 0.0f;
		float m_spiralEndRadians = 0.0f;
		std::vector<Vector2> m_spiralRoundCurvatureStart, m_spiralRoundCurvatureEnd;
		Vector2 m_spiralCircleCenter, m_spiralStartRoundedPos, m_spiralEndRoundedPos;
		Vector2 m_spiralAxialSymmetryDir;

		float m_L = 1.0f;
		float m_weight = 0.5f;
		float m_weight1 = 0.392f;
		float m_weight2 = 0.339f;
		CubicBezier m_bezier;

		Spiral m_spiralCurve;
		TwoWeightCubicBezierSpiral m_twoWeight;
		OneWeightCubicBezierSpiral m_oneWeight;
		G2Spiral m_g2Spiral;
		G3Spiral m_g3Spiral;
		G4Spiral m_g4Spiral;
		G5Spiral m_g5Spiral;
		G6Spiral m_g6Spiral;
		G7Spiral m_g7Spiral;

		int m_currentShapeIndex = 1;
	};
}