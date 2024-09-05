#include "SplineScene.h"


namespace STEditor
{
	SplineScene::~SplineScene()
	{

	}

	void SplineScene::onLoad()
	{

	}

	void SplineScene::onUnLoad()
	{

	}

	void SplineScene::onUpdate(float deltaTime)
	{

	}

	void SplineScene::onRender(Renderer2D& renderer)
	{

	}

	void SplineScene::onRenderUI()
	{
		ImGui::Begin("Spline");

		ImGui::DragInt("Degree", &m_degree, 1, 3, 10);

		ImGui::End();
	}

	void SplineScene::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods)
	{

	}

	void SplineScene::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{

	}


	void SplineScene::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{

	}
}


