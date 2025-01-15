#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class SplineScene : public AbstractScene
	{

	public:
		explicit SplineScene()
			: AbstractScene("SplineScene")
		{
		}

		~SplineScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;
		void onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods) override;
		void onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos) override;
		
		void onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods) override;


	private:
		real deBoor(real u, int i, int k);
		Vector2 samplePoint(real u);
		void updateKnots();
		void updateSamplePoints();
		void updateCurvaturePoints();

		void nurbsDerivative(real u, int order = 1);
		void nurbsCurvature(real u);

		Vector2* m_targetPoint = nullptr;
		bool m_addPoint = false;
		bool m_movePoint = false;
		bool m_snap = false;
		Vector2 m_mousePoint;
		std::vector<Vector3> m_controlPoints;
		std::vector<real> m_knots;
		std::vector<Vector2> m_samplePoints;
		std::vector<Vector2> m_curvatureStart;
		std::vector<Vector2> m_curvatureEnd;
		int m_selectedIndex = -1;
		int m_degree = 3;
		int m_sampleCount = 100;

	};
}