#pragma once

#include "Renderer2D.h"

namespace STEditor
{
	using namespace ST;

	class ReferenceLayer
	{
	public:

		void onRender(Renderer2D& renderer);
		void onUpdate(float deltaTime);
		void onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods);
		void onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods);
		void onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos);
		void onMouseScroll(GLFWwindow* window, Renderer2D& renderer, double xoffset, double yoffset);

		bool& coordsVisible() { return m_coordsVisible; }
		bool& gridVisible() { return m_gridVisible; }
		bool& distanceCheck() { return m_distanceCheck; }

	private:

		bool m_distanceCheck = true;
		bool m_coordsVisible = true;
		bool m_gridVisible = true;

		int m_gridMaxPoint = 100;
		Color axisColor = Palette::Green;
		Color thin = Palette::DarkGreen;
		Color thick = Palette::DarkGreen;
		float numberPixelOffset = 6.0f;

	};
}