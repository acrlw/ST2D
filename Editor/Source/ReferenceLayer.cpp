#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D* renderer)
	{
		if (renderer == nullptr)
			return;

		renderer->line({ -1.0f, -1.0f }, { 1.0f, 1.0f }, DarkPalette::Red);
	}
}