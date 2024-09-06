#include "NarrowphaseScene.h"

namespace STEditor
{
	NarrowphaseScene::~NarrowphaseScene()
	{

	}

	void NarrowphaseScene::onLoad()
	{
		rect.set(1.0f, 1.0f);
		ellipse.set(1.0f, 2.0f);

		tf1.position.set(-0.561019301f, 0.842656434f);
		tf2.position.set(-1.0f, -1.0f);
		tf1.rotation = Math::radians(45.0f);
		tf2.rotation = Math::radians(100.0f);
	}

	void NarrowphaseScene::onUnLoad()
	{
	}

	void NarrowphaseScene::onUpdate(float deltaTime)
	{
		//tf1.rotation += Math::radians(45.0f) * deltaTime;
		//tf1.rotation = std::fmod(tf1.rotation, Constant::DoublePi);
	}

	void NarrowphaseScene::onRender(Renderer2D& renderer)
	{
		auto info = Narrowphase::gjkDistance(tf1, &rect, tf2, &ellipse);

		Color simplexColor = DarkPalette::Purple;

		Color polytopeColor = DarkPalette::Teal;
		polytopeColor.a = 50.0f / 255.0f;

		std::vector<Vector2> points;
		for(auto&& elem: info.polytope)
		{
			points.push_back(elem.vertex.result);
		}
		renderer.polytope(points, polytopeColor);

		renderer.simplex(info.simplex, simplexColor);

		renderer.shape(tf1, &rect, DarkPalette::Yellow);
		renderer.shape(tf2, &ellipse, DarkPalette::Cyan);

		renderer.point(info.pair.pointA, DarkPalette::Yellow);
		renderer.point(info.pair.pointB, DarkPalette::Cyan);

		renderer.dashedLine(info.pair.pointA, info.pair.pointB, DarkPalette::LightGray);

	}

	void NarrowphaseScene::onRenderUI()
	{

	}

	void NarrowphaseScene::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods)
	{
		

	}

	void NarrowphaseScene::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			Vector2 mousePos = renderer.screenToWorld({ static_cast<real>(xpos) , static_cast<real>(ypos)});
			Vector2 p1 = tf1.inverseTranslatePoint(mousePos);
			Vector2 p2 = tf2.inverseTranslatePoint(mousePos);
			if (rect.contains(p1))
			{
				selectedTransform = &tf1;
				oldTransform = tf1;
				mouseStart = mousePos;
			}
			else if (ellipse.contains(p2))
			{
				selectedTransform = &tf2;
				oldTransform = tf2;
				mouseStart = mousePos;
			}
		}
		if (action == GLFW_RELEASE)
			selectedTransform = nullptr;
	}

	void NarrowphaseScene::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{
		if (selectedTransform != nullptr)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			Vector2 currentMousePos = renderer.screenToWorld({ static_cast<real>(xpos) , static_cast<real>(ypos) });

			selectedTransform->position = oldTransform.position + (currentMousePos - mouseStart);
		}
	}
}
