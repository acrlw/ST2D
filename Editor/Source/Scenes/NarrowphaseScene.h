#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class NarrowphaseScene : public AbstractScene
	{
	public:
		explicit NarrowphaseScene(const SceneSettings& settings)
			: AbstractScene(settings, "NarrowphaseScene")
		{
		}

		~NarrowphaseScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods) override;
		void onMouseButton(GLFWwindow* window, int button, int action, int mods) override;
		void onMouseMoved(GLFWwindow* window, double xpos, double ypos) override;

	private:
		ST::Rectangle rect;
		ST::Ellipse ellipse;
		Transform tf1, tf2;
		Transform* selectedTransform = nullptr;
		Transform oldTransform;
		Vector2 mouseStart;
	};
}