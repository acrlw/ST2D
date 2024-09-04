#pragma once

#include "ST2DCore.h"
#include "Camera2D.h"
#include "RenderSFMLImpl.h"
#include "Renderer2D.h"

namespace STEditor
{
	struct SceneSettings
	{
		Camera2D* camera = nullptr;
		sf::Font* font = nullptr;
		Renderer2D* renderer = nullptr;
	};

	class AbstractScene
	{
	public:
		AbstractScene(const SceneSettings& settings, const std::string& name) :
			m_settings(settings), m_name(name)
		{
		}

		virtual ~AbstractScene() = default;

		virtual void onLoad() = 0;
		virtual void onUnLoad() = 0;

		virtual void onUpdate(float deltaTime) = 0;

		virtual void onRender(Renderer2D& renderer) {};
		virtual void onRenderUI() {}

		virtual void onFrameBufferResize(GLFWwindow* window, int width, int height) {};
		virtual void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods) {};
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) {};
		virtual void onMouseMoved(GLFWwindow* window, double xpos, double ypos) {};
		virtual void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {};

	protected:
		std::string m_name;
		SceneSettings m_settings;
	};

}