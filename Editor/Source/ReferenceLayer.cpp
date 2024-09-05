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

		
		renderer.line({}, { 1.0f, 0.0f }, DarkPalette::Red);
		renderer.line({}, { 0.0f, 1.0f }, DarkPalette::Green);

		Color thick = DarkPalette::DarkGreen;
		thick.a = 150.0f / 255.0f;

		Color thin = DarkPalette::DarkGreen;


		float meterToPixel = renderer.meterToPixel();

		AABB viewport = renderer.screenAABB();
		Vector2 bottomLeft = viewport.bottomLeft();
		Vector2 topRight = viewport.topRight();

		float maxPoint = static_cast<float>(m_gridMaxPoint);

		int xMin = std::max(static_cast<int>(std::floor(bottomLeft.x)), -m_gridMaxPoint);
		int yMin = std::max(static_cast<int>(std::floor(bottomLeft.y)), -m_gridMaxPoint);
		int xMax = std::min(static_cast<int>(std::ceil(topRight.x)), m_gridMaxPoint);
		int yMax = std::min(static_cast<int>(std::ceil(topRight.y)), m_gridMaxPoint);


		for (int i = xMin; i <= xMax; ++i)
		{
			if (i == 0)
				thin = DarkPalette::Green;
			else
			{
				thin = DarkPalette::DarkGreen;
				thin.a = 100.0f / 255.0f;
			}

			Vector2 start = { static_cast<real>(i), static_cast<real>(yMax) };
			Vector2 end = { static_cast<real>(i), static_cast<real>(yMin) };
			renderer.line(start, end, thin);
		}

		for (int i = yMin; i <= yMax; ++i)
		{
			if (i == 0)
				thin = DarkPalette::Green;
			else
			{
				thin = DarkPalette::DarkGreen;
				thin.a = 100.0f / 255.0f;
			}

			Vector2 start = { static_cast<real>(xMax), static_cast<real>(i) };
			Vector2 end = { static_cast<real>(xMin), static_cast<real>(i) };
			renderer.line(start, end, thin);
		}
	}
}