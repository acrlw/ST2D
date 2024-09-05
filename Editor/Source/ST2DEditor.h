#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scenes/CurveScene.h"
#include "Scenes/NarrowphaseScene.h"
#include "Scenes/BroadphaseScene.h"
#include "Scenes/SpiralScene.h"
#include "Scenes/HelloWorldScene.h"
#include "Scenes/EmptyScene.h"
#include "Scenes/SplineScene.h"


#include "ReferenceLayer.h"

namespace STEditor
{
	using namespace ST;

	class ST2DEditor
	{
	public:
		ST2DEditor();
		~ST2DEditor() = default;
		void exec();

	private:
		//events
		//void onResized(sf::Event& event);
		//void onClosed(sf::Event& event);
		//void onKeyReleased(sf::Event& event);
		//void onKeyPressed(sf::Event& event);
		//void onMouseReleased(sf::Event& event);
		//void onMouseMoved(sf::Event& event);
		//void onMousePressed(sf::Event& event);
		//void onWheelScrolled(sf::Event& event);

		void onFrameBufferResize(GLFWwindow* window, int width, int height);
		void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods);
		void onMouseButton(GLFWwindow* window, int button, int action, int mods);
		void onMouseMoved(GLFWwindow* window, double xpos, double ypos);
		void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

		//render
		void onRenderUI();
		void onRender();

		//running
		void onUpdate(float deltaTime) const;

		//destroy
		void onDestroy();

		//misc
		void styleUI();
		void restart();
		void switchScene(int index);
		void clearAll();

		bool m_userDrawVisible = true;
		bool m_cameraViewportMovement = false;
		bool m_onDistanceCheck = false;

		int m_currentSceneIndex = 2;

		std::array<const char*, 7> m_sceneName = { "HelloWorld", "Curve", "Broadphase", "Narrowphase", "Empty", "Spline", "Spiral" };

		std::array<std::function<std::unique_ptr<AbstractScene>()>, 7> m_sceneList;
		

		GLFWwindow* m_window;


		float m_zoomFactor = 0.3f;
		bool m_enableDistanceCheck = true;

		Vector2 m_mousePos;
		Vector2 m_screenMousePos;

		std::unique_ptr<AbstractScene> m_currentScene;
		std::array<Vector2, 2> m_mouseArray;
		std::unique_ptr<Renderer2D> m_renderer2D;

		double m_previousTime;

		ReferenceLayer m_referenceLayer;
	};
}