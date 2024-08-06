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
		void onRender(sf::RenderWindow& window) override;
		void onRenderUI() override;
		void onMousePress(sf::Event& event) override;
		void onMouseRelease(sf::Event& event) override;
		void onMouseMove(sf::Event& event) override;
		void onMouseDoubleClick(sf::Event& event) override;
		void onKeyRelease(sf::Event& event) override;
		void onKeyPressed(sf::Event& event) override;

	private:
		void computeReference();
		void computeSpiral();
		void computeG1();
		void drawG1(sf::RenderWindow& window);
		void drawSpiral(sf::RenderWindow& window);
		void drawReference(sf::RenderWindow& window);
		void drawCurvature(sf::RenderWindow& window, const std::vector<Vector2>& start,
			const std::vector<Vector2>& end, const sf::Color& color, bool flip);

		real arcLengthCurvature(real s);
		real arcLengthCurvatureInt(real s);

		bool m_lockCornerStart = false;
		float m_lockWidthSize = 0.0f;
		float m_lockHeightSize = 0.0f;
		bool m_showCurvatureBezier = false;
		bool m_connectInner = false;
		bool m_showRoundedCurvature = false;
		bool m_showReferenceLine = false;
		bool m_showG1Continuity = false;
		bool m_showCurvature = false;
		float m_innerWidthFactor = 0.55f;
		float m_innerHeightFactor = 0.55f;
		float m_curvatureScaleFactor = 0.1f;
		float m_halfWidth = 1.0f;
		float m_halfHeight = 1.0f;
		float m_percentage = 0.618f;
		float m_cornerPercentage = 0.382f;

		int m_circleSegment = 100;

		Vector2 m_p00, m_p01, m_p10, m_p11;
		Vector2 m_b1P01, m_b1P10, m_b2P01, m_b2P10;
		Vector2 m_roundCenter, m_roundCorner;
		Vector2 m_startRoundedPos, m_endRoundedPos;
		float m_currentRadius = 0.0f;
		float m_roundStartRadians = 0.0f;
		float m_roundEndRadians = 0.0f;


		float m_scaleK = 1.0f;


		int N = 200;
		float L = 1.0f;
		float alpha = 1.0f;

		std::vector<Vector2> m_spiral, m_spiralSymmetry;
		std::vector<Vector2> m_spiralCurvaturePoints, m_spiralSymmetryCurvaturePoints;

		std::vector<Vector2> m_g1Vertices;
		std::vector<Vector2> m_roundCurvatureStart, m_roundCurvatureEnd;

		float m_spiralStartRadians = 0.0f;
		float m_spiralEndRadians = 0.0f;
		std::vector<Vector2> m_spiralRoundCurvatureStart, m_spiralRoundCurvatureEnd;
		Vector2 m_spiralCircleCenter, m_spiralStartRoundedPos, m_spiralEndRoundedPos;
		Vector2 m_spiralAxialSymmetryDir;

		float m_L = 1.0f;
		float m_alpha = 1.0f;
		float m_weight = 0.5f;
		CubicBezier m_bezier;
	};
}