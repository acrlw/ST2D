#pragma once

#include "AbstractScene.h"
#include <deque>

namespace STEditor
{
	class HelloWorldScene : public AbstractScene
	{
	public:
		explicit HelloWorldScene()
			: AbstractScene("HelloWorldScene")
		{
		}

		~HelloWorldScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;

	private:

		Vector2 position;
		float theta = 0.0f;
		float r = 1.0f;
		EasingObject<float> thetaEasing = EasingObject(0.0f);
		std::deque<Vector2> pList;
		bool flip = false;
		EasingObject<Complex> matEasing = EasingObject(Complex(Math::radians(180)));
		Vector2 arrow;
	};
}