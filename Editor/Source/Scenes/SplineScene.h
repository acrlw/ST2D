#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class SplineScene : public AbstractScene
	{

	public:
		explicit SplineScene(const SceneSettings& settings)
			: AbstractScene(settings, "SplineScene")
		{
		}

		~SplineScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;
		void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods) override;
		void onMouseMoved(GLFWwindow* window, double xpos, double ypos) override;
		void onMouseButton(GLFWwindow* window, int button, int action, int mods) override;


	private:
		Vector2* m_targetPoint = nullptr;
		bool m_addPoint = false;
		bool m_movePoint = false;
		bool m_snap = false;
		Vector2 m_mousePoint;
		std::vector<Vector2> m_points;
		std::vector<std::vector<Vector2>> m_splineList;
		int m_degree = 3;

	};
}