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

	void NarrowphaseScene::onRender(sf::RenderWindow& window)
	{

		RenderSFMLImpl::renderShape(window, *m_settings.camera, tf1, &rect, RenderConstant::Yellow);

		RenderSFMLImpl::renderShape(window, *m_settings.camera, tf2, &ellipse, RenderConstant::Cyan);

		auto info = Narrowphase::gjkDistance(tf1, &rect, tf2, &ellipse);

		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointA, RenderConstant::Yellow);
		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointB, RenderConstant::Cyan);

		RenderSFMLImpl::renderDashedLine(window, *m_settings.camera, info.pair.pointA, info.pair.pointB, RenderConstant::LightGray);


	}

	void NarrowphaseScene::onRenderUI()
	{
		AbstractScene::onRenderUI();
	}

	void NarrowphaseScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void NarrowphaseScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void NarrowphaseScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void NarrowphaseScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void NarrowphaseScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void NarrowphaseScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}
}
