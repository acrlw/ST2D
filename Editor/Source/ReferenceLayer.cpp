#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D* renderer)
	{
		if (renderer == nullptr)
			return;
		
		renderer->line(100, 100, 500, 500, DarkPalette::Green);
	}
}