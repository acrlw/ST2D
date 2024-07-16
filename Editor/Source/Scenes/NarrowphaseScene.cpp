#include "NarrowphaseScene.h"

namespace STEditor
{
	NarrowphaseScene::~NarrowphaseScene()
	{

	}

	void NarrowphaseScene::onLoad()
	{
		rect.set(1.0f, 1.0f);
		sp1.shape = &rect;
		sp2.shape = &rect;

		sp1.transform.position.set(1.0f, 1.5f);
		sp2.transform.position.set(-1.0f, -1.0f);
		sp1.transform.rotation = ST::Math::radians(45.0f);
		sp2.transform.rotation = Math::radians(62);
	}

	void NarrowphaseScene::onUnLoad()
	{
	}

	void NarrowphaseScene::onUpdate(float deltaTime)
	{
	}

	void NarrowphaseScene::onRender(sf::RenderWindow& window)
	{

		RenderSFMLImpl::renderShape(window, *m_settings.camera, sp1, RenderConstant::Yellow);

		RenderSFMLImpl::renderShape(window, *m_settings.camera, sp2, RenderConstant::Cyan);

		auto info = Narrowphase::gjkDistance(sp1, sp2);

		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointA, RenderConstant::Yellow);
		RenderSFMLImpl::renderPoint(window, *m_settings.camera, info.pair.pointB, RenderConstant::Cyan);

		RenderSFMLImpl::renderLine(window, *m_settings.camera, info.pair.pointA, info.pair.pointB, RenderConstant::Green);


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
