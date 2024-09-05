#pragma once

#include "ST2DCore.h"
#include "Camera2D.h"
#include "RenderSFMLImpl.h"
#include "Renderer2D.h"

namespace STEditor
{
	class AbstractScene
	{
	public:
		AbstractScene(const std::string& name) :
			m_name(name)
		{
		}

		virtual ~AbstractScene() = default;

		virtual void onLoad() = 0;
		virtual void onUnLoad() = 0;

		virtual void onUpdate(float deltaTime) = 0;

		virtual void onRender(Renderer2D& renderer) {};
		virtual void onRenderUI() {}

		virtual void onFrameBufferResize(GLFWwindow* window, Renderer2D& renderer, int width, int height) {};
		virtual void onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods) {};
		virtual void onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods) {};
		virtual void onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos) {};
		virtual void onMouseScroll(GLFWwindow* window, Renderer2D& renderer, double xoffset, double yoffset) {};

	protected:
		std::string m_name;
	};

}