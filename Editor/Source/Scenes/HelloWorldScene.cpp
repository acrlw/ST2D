#include "HelloWorldScene.h"


namespace STEditor
{
	HelloWorldScene::~HelloWorldScene()
	{

	}

	void HelloWorldScene::onLoad()
	{
		thetaEasing.setEasingFunction(EasingFunction::easeInOutSine);
		matEasing.restart(Complex(Math::radians(0)), Complex(Math::radians(-180)), 1.5f);
	}

	void HelloWorldScene::onUnLoad()
	{

	}

	void HelloWorldScene::onUpdate(float deltaTime)
	{
		thetaEasing.update(deltaTime);
		matEasing.update(deltaTime);
		if (thetaEasing.isFinished())
			thetaEasing.restart(0.0f, Constant::DoublePi, 3.0f);
		if(matEasing.isFinished())
		{
			if(!flip)
			{
				matEasing.continueTo(Complex(Math::radians(0)), 1.5f);
				flip = true;
			}
			else
			{
				matEasing.continueTo(Complex(Math::radians(-180)), 1.5f);
				flip = false;
			}
		}

		theta = thetaEasing.value();
		Matrix2x2 mat(theta);
		Vector2 p = mat.multiply(Vector2(1.0f, 0.0f));
		p *= r * (1.0f + Math::cosx(3.0f * theta) + 1.5f * Math::sinx(3.0f * theta) * Math::sinx(3.0f * theta));

		arrow = matEasing.value().multiply(Vector2(2.0f, 0.0f));

		if (pList.size() > 500)
			pList.pop_front();

		pList.push_back(p);
	}

	void HelloWorldScene::onRender(sf::RenderWindow& window)
	{
		sf::Color color = RenderConstant::Pink;
		color.a = 0;
		float step = 255.0f / static_cast<float>(pList.size());
		for(int i = 1;i < pList.size(); ++i)
		{
			color.a = i * step;
			RenderSFMLImpl::renderThickLine(window, *m_settings.camera, pList[i - 1], pList[i], color);
			/*RenderSFMLImpl::renderPoint(window, *m_settings.camera, pList[i], color, 3.0f);*/
		}
		RenderSFMLImpl::renderArrow(window, *m_settings.camera, Vector2(0.0f, 0.0f), arrow, RenderConstant::Green, 0.25f);
	}

	void HelloWorldScene::onRenderUI()
	{
		AbstractScene::onRenderUI();
	}

	void HelloWorldScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void HelloWorldScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void HelloWorldScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void HelloWorldScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void HelloWorldScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void HelloWorldScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}

}


