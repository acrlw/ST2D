#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class NarrowphaseScene : public AbstractScene
	{
	public:
		explicit NarrowphaseScene()
			: AbstractScene("NarrowphaseScene")
		{
		}

		~NarrowphaseScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;
		void onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods) override;
		void onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods) override;
		void onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos) override;

	private:
		ST::Rectangle rect;
		ST::Ellipse ellipse;
		ST::Capsule capsule;
		ST::Circle circle;
		Transform tf1, tf2;
		Transform* selectedTransform = nullptr;
		Transform oldTransform;
		Vector2 mouseStart;

		//int m_currentPolytopeIndex = 0;
		//int m_maxPolytopeIndex = 0;
		bool m_showSimplex = false;
		bool m_showPolytope = false;
	};
}