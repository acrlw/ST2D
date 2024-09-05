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

		tf1.position.set(1.0f, 1.5f);
		tf2.position.set(-1.0f, -1.0f);
		tf1.rotation = Math::radians(45.0f);
		tf2.rotation = Math::radians(100.0f);
	}

	void NarrowphaseScene::onUnLoad()
	{
	}

	void NarrowphaseScene::onUpdate(float deltaTime)
	{
	}

	void NarrowphaseScene::onRender(Renderer2D& renderer)
	{
		auto info = Narrowphase::gjkDistance(tf1, &rect, tf2, &ellipse);

		renderer.shape(tf1, &rect, DarkPalette::Yellow);
		renderer.shape(tf2, &ellipse, DarkPalette::Cyan);

		renderer.point(info.pair.pointA, DarkPalette::Yellow);
		renderer.point(info.pair.pointB, DarkPalette::Cyan);

		renderer.dashedLine(info.pair.pointA, info.pair.pointB, DarkPalette::LightGray);

	}

	void NarrowphaseScene::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods)
	{
		//if (event.mouseButton.button == sf::Mouse::Left)
		//{
		//	Vector2 mousePos = m_settings.camera->screenToWorld(Vector2(event.mouseButton.x, event.mouseButton.y));
		//	Vector2 p1 = tf1.inverseTranslatePoint(mousePos);
		//	Vector2 p2 = tf2.inverseTranslatePoint(mousePos);
		//	if (rect.contains(p1))
		//	{
		//		selectedTransform = &tf1;
		//		oldTransform = tf1;
		//		mouseStart = mousePos;
		//	}
		//	else if (ellipse.contains(p2))
		//	{
		//		selectedTransform = &tf2;
		//		oldTransform = tf2;
		//		mouseStart = mousePos;
		//	}
		//}
	}

	void NarrowphaseScene::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{
		//selectedTransform = nullptr;
	}

	void NarrowphaseScene::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{
		//if (selectedTransform != nullptr)
		//{
		//	Vector2 pos(event.mouseMove.x, event.mouseMove.y);
		//	Vector2 currentMousePos = m_settings.camera->screenToWorld(pos);

		//	selectedTransform->position = oldTransform.position + (currentMousePos - mouseStart);
		//}
	}
}
