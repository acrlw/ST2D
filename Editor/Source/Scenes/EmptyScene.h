#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class EmptyScene : public AbstractScene
	{
	public:
		explicit EmptyScene()
			: AbstractScene("EmptyScene")
		{
		}

		~EmptyScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;


	private:
	};
}