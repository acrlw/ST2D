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

	void NarrowphaseScene::onDraw(sf::RenderWindow& window)
	{

		RenderSFMLImpl::renderShape(window, *m_settings.camera, tf1, &rect, RenderConstant::Yellow);

		RenderSFMLImpl::renderShape(window, *m_settings.camera, tf2, &ellipse, RenderConstant::Cyan);

		auto info = Narrowphase::gjkDistance(tf1, &rect, tf2, &ellipse);

		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointA, RenderConstant::Yellow);
		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointB, RenderConstant::Cyan);

		RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, info.pair.pointA, info.pair.pointB, RenderConstant::LightGray);


	}

	void NarrowphaseScene::onMousePress(sf::Event& event)
	{
		if(event.mouseButton.button == sf::Mouse::Left)
		{
			Vector2 mousePos = m_settings.camera->screenToWorld(Vector2(event.mouseButton.x, event.mouseButton.y));
			Vector2 p1 = tf1.inverseTranslatePoint(mousePos);
			Vector2 p2 = tf2.inverseTranslatePoint(mousePos);
			if(rect.contains(p1))
			{
				selectedTransform = &tf1;
				oldTransform = tf1;
				mouseStart = mousePos;
			}
			else if(ellipse.contains(p2))
			{
				selectedTransform = &tf2;
				oldTransform = tf2;
				mouseStart = mousePos;
			}
		}
	}

	void NarrowphaseScene::onMouseRelease(sf::Event& event)
	{
		selectedTransform = nullptr;
	}

	void NarrowphaseScene::onMouseMove(sf::Event& event)
	{
		if (selectedTransform != nullptr)
		{
			Vector2 pos(event.mouseMove.x, event.mouseMove.y);
			Vector2 currentMousePos = m_settings.camera->screenToWorld(pos);

			selectedTransform->position = oldTransform.position + (currentMousePos - mouseStart);
		}
	}
}
