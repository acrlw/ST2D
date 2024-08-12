#pragma once


#include "Scenes/CurveScene.h"
#include "Scenes/NarrowphaseScene.h"
#include "Scenes/SpiralScene.h"
#include "Scenes/HelloWorldScene.h"
#include "Scenes/EmptyScene.h"
#include "Scenes/SplineScene.h"
#include <SFML/Graphics/Font.hpp>
#include "ST2DCore.h"

namespace STEditor
{
	using namespace ST;

	class ST2DEditor
	{
	public:
		ST2DEditor();
		~ST2DEditor();
		void exec();

	private:
		void styleUI();
		//events
		void onResized(sf::Event& event);
		void onClosed(sf::Event& event);
		void onKeyReleased(sf::Event& event);
		void onKeyPressed(sf::Event& event);
		void onMouseReleased(sf::Event& event);
		void onMouseMoved(sf::Event& event);
		void onMousePressed(sf::Event& event);
		void onWheelScrolled(sf::Event& event);
		//render
		void renderGUI(sf::RenderWindow& window, sf::Clock& clock);
		void render(sf::RenderWindow& window);
		//simulation

		void restart();

		void onUpdate(float deltaTime);

		void switchScene(int index);
		void clearAll();

		bool m_userDrawVisible = true;

		bool m_cameraViewportMovement = false;
		bool m_onDistanceCheck = false;


		int m_currentSceneIndex = 5;

		Camera2D m_camera;

		std::array<const char*, 6> m_sceneName = { "HelloWorldScene", "CurveScene", "NarrowphaseScene", "EmptyScene", "SplineScene", "SpiralScene" };

		std::array<std::function<std::unique_ptr<AbstractScene> (const SceneSettings& settings)>, 6> m_sceneList;
		std::unique_ptr<sf::RenderWindow> m_window;

		float m_zoomFactor = 0.3f;
		bool m_enableDistanceCheck = true;

		Vector2 m_mousePos;
		Vector2 m_screenMousePos;

		std::unique_ptr<AbstractScene> m_currentScene;
		sf::Font m_font;

		std::array<Vector2, 2> m_mouseArray;
	};
}