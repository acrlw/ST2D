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


		virtual void onDraw(sf::RenderWindow& window) = 0;
		virtual void onRender(GLFWwindow* window, Renderer2D* renderer) {};
		virtual void onRenderUI() {}

		virtual void onFrameBufferResize(int width, int height) {};
		virtual void onKeyButton(int key, int scancode, int action, int mods) {};
		virtual void onMouseButton(int button, int action, int mods) {};
		virtual void onMouseMoved(double xpos, double ypos) {};
		virtual void onMouseScroll(double xoffset, double yoffset) {};

		//duplicated
		virtual void onMousePress(sf::Event& event) {}
		virtual void onMouseRelease(sf::Event& event) {}
		virtual void onMouseMove(sf::Event& event) {}
		virtual void onMouseDoubleClick(sf::Event& event) {}
		virtual void onKeyRelease(sf::Event& event) {}
		virtual void onKeyPressed(sf::Event& event) {}

	protected:
		std::string m_name;
		SceneSettings m_settings;
	};

}