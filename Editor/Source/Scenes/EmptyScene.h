#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class EmptyScene : public AbstractScene
	{
	public:
		explicit EmptyScene(const SceneSettings& settings)
			: AbstractScene(settings, "EmptyScene")
		{
		}

		~EmptyScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(sf::RenderWindow& window) override;
		void onRenderUI() override;
		void onMousePress(sf::Event& event) override;
		void onMouseRelease(sf::Event& event) override;
		void onMouseMove(sf::Event& event) override;
		void onMouseDoubleClick(sf::Event& event) override;
		void onKeyRelease(sf::Event& event) override;
		void onKeyPressed(sf::Event& event) override;

	private:
	};
}