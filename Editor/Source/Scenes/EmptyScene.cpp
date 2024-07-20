#include "EmptyScene.h"


namespace STEditor
{
	EmptyScene::~EmptyScene()
	{

	}

	void EmptyScene::onLoad()
	{

	}

	void EmptyScene::onUnLoad()
	{

	}

	void EmptyScene::onUpdate(float deltaTime)
	{

	}

	void EmptyScene::onRender(sf::RenderWindow& window)
	{

	}

	void EmptyScene::onRenderUI()
	{
		AbstractScene::onRenderUI();
	}

	void EmptyScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void EmptyScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void EmptyScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void EmptyScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void EmptyScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void EmptyScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}

}


