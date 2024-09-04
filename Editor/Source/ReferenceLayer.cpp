#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D& renderer)
	{
		renderer.point({ 1.0f, 1.0f }, DarkPalette::Pink);
		//renderer.thickLine({ -1.0f, -1.0f }, { 1.0f, 1.0f }, DarkPalette::Teal, 2.0f);
		//renderer.thickLine({ -2.0f, -1.0f }, { 0.0f, 1.0f }, DarkPalette::Purple, 8.0f);
		//renderer.line(100, 500, 500, 800, DarkPalette::Purple);
		//renderer.closeLines({ {2.0f, 1.0f}, {1.0f, -1.0f}, {-1.0f, -1.0f} }, DarkPalette::Green);
		//renderer.line({ -10.0f, 0.0f }, { 10.0f, 0.0f }, DarkPalette::Green);
		//renderer.line({ 0.0f, -10.0f }, { 0.0f, 10.0f }, DarkPalette::Green);

		//Vector2 center = renderer->worldToScreen({ 0.0f, 0.0f });
		//int i = 0;

		//center = renderer->screenToWorld({ 1920.0f, 1080.0f });
		//i = 1;
	}
}