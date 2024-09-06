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


		Color thick = DarkPalette::DarkGreen;

		Color thin = DarkPalette::DarkGreen;
		thin.a = 55.0f / 255.0f;

		float meterToPixel = renderer.meterToPixel();

		int h = 1;
		if(meterToPixel <= 5)
			h = 20;
		else if (meterToPixel <= 20)
			h = 10;
		else if (meterToPixel <= 40)
			h = 5;
		else if (meterToPixel <= 60)
			h = 2;

		bool isFine = meterToPixel >= 100;

		int f = 5;
		if(isFine)
		{
			if (meterToPixel >= 180)
				f = 1;
		}


		AABB viewport = renderer.screenAABB();
		Vector2 bottomLeft = viewport.bottomLeft();
		Vector2 topRight = viewport.topRight();


		int xMin = std::max(static_cast<int>(std::floor(bottomLeft.x)), -m_gridMaxPoint);
		int yMin = std::max(static_cast<int>(std::floor(bottomLeft.y)), -m_gridMaxPoint);
		int xMax = std::min(static_cast<int>(std::ceil(topRight.x)), m_gridMaxPoint);
		int yMax = std::min(static_cast<int>(std::ceil(topRight.y)), m_gridMaxPoint);


		for (int i = xMin; i <= xMax; ++i)
		{
			if (i % h != 0)
				continue;

			if (i == 0)
				thick = DarkPalette::Green;
			else
			{
				thick = DarkPalette::DarkGreen;
				thick.a = 155.0f / 255.0f;
			}

			Vector2 start = { static_cast<real>(i), static_cast<real>(yMax) };
			Vector2 end = { static_cast<real>(i), static_cast<real>(yMin) };
			renderer.line(start, end, thick);

			if(isFine)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (j % f != 0)
						continue;
					float fi = static_cast<real>(i);
					float fj = static_cast<real>(j);
					start = { fi + fj * 0.1f, static_cast<real>(yMax) };
					end = { fi + fj * 0.1f, static_cast<real>(yMin) };
					renderer.line(start, end, thin);
				}
			}
		}

		for (int i = yMin; i <= yMax; ++i)
		{
			if (i % h != 0)
				continue;

			if (i == 0)
				thick = DarkPalette::Green;
			else
			{
				thick = DarkPalette::DarkGreen;
				thick.a = 155.0f / 255.0f;
			}

			Vector2 start = { static_cast<real>(xMax), static_cast<real>(i) };
			Vector2 end = { static_cast<real>(xMin), static_cast<real>(i) };
			renderer.line(start, end, thick);

			if (isFine)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (j % f != 0)
						continue;

					float fi = static_cast<real>(i);
					float fj = static_cast<real>(j);
					start = { static_cast<real>(xMax), fi + fj * 0.1f };
					end = { static_cast<real>(xMin), fi + fj * 0.1f };
					renderer.line(start, end, thin);
				}
			}
		}
	}
}