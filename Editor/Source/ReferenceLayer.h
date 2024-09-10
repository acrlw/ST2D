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
		bool& enableDistanceCheck() { return m_enableDistanceCheck; }

	private:

		bool m_enableDistanceCheck = true;
		bool m_distanceCheck = false;
		bool m_distanceCheckMoved = false;
		bool m_coordsVisible = true;
		bool m_gridVisible = true;

		int m_gridMaxPoint = 100;
		Color m_axisColor = Palette::Green;
		Color m_thinColor = Palette::DarkGreen;
		Color m_thickColor = Palette::DarkGreen;
		float m_coordsOffsetPixel = 6.0f;

		Vector2 m_mouseStart;
		Vector2 m_currentMouse;
	};
}