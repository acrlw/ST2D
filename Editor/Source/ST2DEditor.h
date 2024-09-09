#pragma once

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
		void onFrameBufferResize(GLFWwindow* window, int width, int height);
		void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods);
		void onMouseButton(GLFWwindow* window, int button, int action, int mods);
		void onMouseMoved(GLFWwindow* window, double xpos, double ypos);
		void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

		//render
		void onRenderUI();
		void onRender();

		//running
		void onUpdate(float deltaTime);

		//destroy
		void onDestroy();

		//misc
		void styleDarkUI();
		void styleLightUI();
		void restart();
		void switchScene(int index);
		void clearAll();

		ImVec4 colorToImVec4(unsigned int hexValue);
		ImVec4 colorToImVec4(int r, int g, int b, int a = 255);

		bool m_userDrawVisible = true;

		int m_currentSceneIndex = 0;

		std::array<const char*, 7> m_sceneName = { "HelloWorld", "Curve", "Broadphase", "Narrowphase", "Empty", "Spline", "Spiral" };

		std::array<std::function<std::unique_ptr<AbstractScene>()>, 7> m_sceneList;
		

		GLFWwindow* m_window;

		Vector2 m_mousePos;
		Vector2 m_screenMousePos;

		std::unique_ptr<AbstractScene> m_currentScene;
		std::array<Vector2, 2> m_mouseArray;
		std::unique_ptr<Renderer2D> m_renderer2D;

		double m_previousTime;

		ReferenceLayer m_referenceLayer;

		bool m_nightMode = true;
		bool m_currentNightMode = true;
	};
}