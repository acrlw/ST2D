#include "ReferenceLayer.h"

namespace STEditor
{
	void ReferenceLayer::onRender(Renderer2D& renderer)
	{
		if(m_distanceCheck && m_distanceCheckMoved)
		{
			int x1 = static_cast<int>(m_mouseStart.x);
			int y1 = static_cast<int>(m_mouseStart.y);
			int x2 = static_cast<int>(m_currentMouse.x);
			int y2 = static_cast<int>(m_currentMouse.y);

			renderer.arrow(x1, y1, x2, y2, Palette::Teal, 0.1f);
			Vector2 start = Vector2(x1, y1);
			Vector2 end = Vector2(x2, y2);

			int offsetX = x2;
			int offsetY = y2;
			offsetY -= 25;
			start = renderer.screenToWorld(start);
			end = renderer.screenToWorld(end);
			Vector2 diff = end - start;

			if(diff.x >= 0.0f)
				offsetX += 50;
			else
				offsetX -= 50;

			std::string text = std::format("{:.3f}", diff.length());
			renderer.text(offsetX, offsetY, Palette::Teal, text, 1.0f, true);
		}

		if (m_gridVisible)
		{
			m_axisColor = Palette::Green;
			m_thinColor = Palette::DarkGreen;
			m_thickColor = Palette::DarkGreen;

			m_thinColor.a = 60.0f / 255.0f;
			m_thickColor.a = 120.0f / 255.0f;

			float meterToPixel = renderer.meterToPixel();

			int h = 1;
			if (meterToPixel <= 5)
				h = 20;
			else if (meterToPixel <= 20)
				h = 10;
			else if (meterToPixel <= 40)
				h = 5;
			else if (meterToPixel <= 60)
				h = 2;

			bool isFine = meterToPixel > 60;

			int f = 5;
			if (isFine)
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

			Color color = m_axisColor;
			Vector2 offset = Vector2(-1.0f, -1.0f) * m_coordsOffsetPixel * renderer.pixelToMeter();
			for (int i = xMin; i <= xMax; ++i)
			{
				if (i % h != 0)
					continue;

				if (i == 0)
					color = m_axisColor;
				else
					color = m_thickColor;


				Vector2 start = { static_cast<real>(i), static_cast<real>(yMax) };
				Vector2 end = { static_cast<real>(i), static_cast<real>(yMin) };
				renderer.line(start, end, color);
				if (m_coordsVisible)
					renderer.text(Vector2(static_cast<real>(i), 0.0f) + offset, Palette::DarkGreen, std::to_string(i), 1.0f, true);

				if (isFine)
				{
					for (int j = 0; j < 10; ++j)
					{
						if (j % f != 0)
							continue;
						float fi = static_cast<real>(i);
						float fj = static_cast<real>(j);
						start = { fi + fj * 0.1f, static_cast<real>(yMax) };
						end = { fi + fj * 0.1f, static_cast<real>(yMin) };
						renderer.line(start, end, m_thinColor);
					}
				}
			}

			for (int i = yMin; i <= yMax; ++i)
			{
				if (i % h != 0)
					continue;

				if (i == 0)
					color = m_axisColor;
				else
					color = m_thickColor;

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
						renderer.line(start, end, m_thinColor);
					}
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
		bool isLCtrlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
		bool isDPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		
		if(m_enableDistanceCheck && isLCtrlPressed && isDPressed && !m_distanceCheck)
		{
			m_distanceCheck = true;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			m_mouseStart.set(xpos, ypos);
		}
		else if (m_distanceCheck && (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_D) && action == GLFW_RELEASE)
		{
			m_distanceCheck = false;
			m_distanceCheckMoved = false;
			m_mouseStart.clear();
		}
	}

	void ReferenceLayer::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{

	}

	void ReferenceLayer::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{
		if(m_distanceCheck)
		{
			m_distanceCheckMoved = true;
			m_currentMouse.set(xpos, ypos);
		}
	}

	void ReferenceLayer::onMouseScroll(GLFWwindow* window, Renderer2D& renderer, double xoffset, double yoffset)
	{

	}
}
