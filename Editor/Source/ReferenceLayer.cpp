#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D& renderer)
	{
		if (!m_gridVisible)
			return;

		axisColor = Palette::Green;
		thin = Palette::DarkGreen;
		thick = Palette::DarkGreen;

		thin.a = 60.0f / 255.0f;
		thick.a = 120.0f / 255.0f;

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

		bool isFine = meterToPixel > 60;

		int f = 5;
		if(isFine)
		{
			if (meterToPixel >= 150)
				f = 1;
		}


		AABB viewport = renderer.screenAABB();
		Vector2 bottomLeft = viewport.bottomLeft();
		Vector2 topRight = viewport.topRight();

		int xMin = std::max(static_cast<int>(std::floor(bottomLeft.x)), -m_gridMaxPoint);
		int yMin = std::max(static_cast<int>(std::floor(bottomLeft.y)), -m_gridMaxPoint);
		int xMax = std::min(static_cast<int>(std::ceil(topRight.x)), m_gridMaxPoint);
		int yMax = std::min(static_cast<int>(std::ceil(topRight.y)), m_gridMaxPoint);

		Color color = axisColor;
		Vector2 offset = Vector2(-1.0f, -1.0f) * numberPixelOffset * renderer.pixelToMeter();
		for (int i = xMin; i <= xMax; ++i)
		{
			if (i % h != 0)
				continue;

			if (i == 0)
				color = axisColor;
			else
				color = thick;
			

			Vector2 start = { static_cast<real>(i), static_cast<real>(yMax) };
			Vector2 end = { static_cast<real>(i), static_cast<real>(yMin) };
			renderer.line(start, end, color);
			if(m_coordsVisible)
				renderer.text(Vector2(static_cast<real>(i), 0.0f) + offset, Palette::DarkGreen, std::to_string(i), 1.0f, true);

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
				color = axisColor;
			else
				color = thick;

			Vector2 start = { static_cast<real>(xMax), static_cast<real>(i) };
			Vector2 end = { static_cast<real>(xMin), static_cast<real>(i) };
			renderer.line(start, end, color);
			if (m_coordsVisible && i != 0)
				renderer.text(Vector2(0.0f, static_cast<real>(i)) + offset, Palette::DarkGreen, std::to_string(i), 1.0f, true);

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

	void ReferenceLayer::onUpdate(float deltaTime)
	{

	}

	void ReferenceLayer::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action,
		int mods)
	{

	}

	void ReferenceLayer::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{

	}

	void ReferenceLayer::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{

	}

	void ReferenceLayer::onMouseScroll(GLFWwindow* window, Renderer2D& renderer, double xoffset, double yoffset)
	{

	}
}
