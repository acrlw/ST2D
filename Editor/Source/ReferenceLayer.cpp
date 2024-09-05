#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D& renderer)
	{
		if (!m_gridVisible)
			return;

		Color thin = DarkPalette::DarkGreen;
		for (int i = -10; i <= 10; ++i)
		{
			if (i == 0)
				thin = DarkPalette::Green;
			else
			{
				thin = DarkPalette::DarkGreen;
				thin.a = 100.0f / 255.0f;
			}

			Vector2 start = { -10.0f, static_cast<float>(i) };
			Vector2 end = { 10.0f, static_cast<float>(i) };
			renderer.line(start, end, thin);
			start = { static_cast<float>(i), -10.0f };
			end = { static_cast<float>(i), 10.0f };
			renderer.line(start, end, thin);
		}

		//renderer.point({ 1.0f, 1.0f }, DarkPalette::Pink);
		//renderer.thickLine({ -1.0f, -1.0f }, { 1.0f, 1.0f }, DarkPalette::Teal, 2.0f);
		//renderer.thickLine({ -2.0f, -1.0f }, { 0.0f, 1.0f }, DarkPalette::Purple, 8.0f);
		//renderer.line(100, 500, 500, 800, DarkPalette::Purple);
		//auto color = DarkPalette::DarkGreen;
		//color.a = 40.0f / 255.0f;
		//renderer.fillAndStroke({ {2.0f, 1.0f}, {1.0f, -1.0f}, {-1.0f, -1.0f},{-1.0f, 2.0f} }, color, DarkPalette::Purple, 1.0f);


		//renderer.line({ -10.0f, 0.0f }, { 10.0f, 0.0f }, DarkPalette::Green);
		//renderer.line({ 0.0f, -10.0f }, { 0.0f, 10.0f }, DarkPalette::Green);

		//Vector2 center = renderer->worldToScreen({ 0.0f, 0.0f });
		//int i = 0;

		//center = renderer->screenToWorld({ 1920.0f, 1080.0f });
		//i = 1;
	}
}