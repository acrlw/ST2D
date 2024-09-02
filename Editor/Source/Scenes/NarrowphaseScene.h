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
		void onRender(sf::RenderWindow& window) override;
		void onMousePress(sf::Event& event) override;
		void onMouseRelease(sf::Event& event) override;
		void onMouseMove(sf::Event& event) override;

	private:
		ST::Rectangle rect;
		ST::Ellipse ellipse;
		Transform tf1, tf2;
		Transform* selectedTransform = nullptr;
		Transform oldTransform;
		Vector2 mouseStart;
	};
}