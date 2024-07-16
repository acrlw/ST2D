#include "CurveScene.h"
#include "Render.h"

namespace STEditor
{
	CurveScene::~CurveScene()
	{
		
	}

	void CurveScene::onLoad()
	{

	}

	void CurveScene::onUnLoad()
	{
	}

	void CurveScene::onUpdate(float deltaTime)
	{
	}

	void CurveScene::onRender(sf::RenderWindow& window)
	{
		RenderSFMLImpl::renderPoint(window, *m_settings.camera, Vector2(0, 0), RenderConstant::Red, 10);
	}

	void CurveScene::onRenderUI()
	{
		AbstractScene::onRenderUI();
	}

	void CurveScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void CurveScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void CurveScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void CurveScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void CurveScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void CurveScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}
}
