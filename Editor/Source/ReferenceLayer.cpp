#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D& renderer)
	{
		if (!m_gridVisible)
			return;

		/*
		 * 50: 1m
		 * 30: 2m
		 * 20: 5m
		 * 10: 10m
		 * 100: 0.5m
		 * 200: 0.1m
		 */

		Color thin = DarkPalette::DarkGreen;
		int gridSize = 50;
		float gridSizeF = static_cast<float>(gridSize);
		
		for (int i = -gridSize; i <= gridSize; ++i)
		{
			if (i == 0)
				thin = DarkPalette::Green;
			else
			{
				thin = DarkPalette::DarkGreen;
				thin.a = 100.0f / 255.0f;
			}

			Vector2 start = { -gridSizeF, static_cast<float>(i)};
			Vector2 end = { gridSizeF, static_cast<float>(i) };
			renderer.line(start, end, thin);
			start = { static_cast<float>(i), -gridSizeF };
			end = { static_cast<float>(i), gridSizeF };
			renderer.line(start, end, thin);
		}
	}
}