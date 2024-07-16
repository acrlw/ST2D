#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

namespace STEditor
{
	namespace RenderConstant
	{
		constexpr real PointSize = 2.0f;
		constexpr int BorderSize = 1;
		constexpr int FillAlpha = 38;
		constexpr int BasicCirclePointCount = 60;
		constexpr real BasicDashLength = 0.04f;
		constexpr real BasicDashGap = 0.04f;
		const sf::Color Yellow = sf::Color(255, 235, 59);
		const sf::Color Red = sf::Color(244, 67, 54);
		const sf::Color Blue = sf::Color(55, 133, 205);
		const sf::Color LightBlue = sf::Color(3, 169, 244);
		const sf::Color Cyan = sf::Color(78, 184, 210);
		const sf::Color LightCyan = sf::Color(0, 188, 212);
		const sf::Color Green = sf::Color(15, 250, 14);
		const sf::Color Pink = sf::Color(233, 30, 99);
		const sf::Color DarkGreen = sf::Color(44, 113, 48);
		const sf::Color Gray = sf::Color(189, 189, 189);
		const sf::Color Orange = sf::Color(255, 138, 101);
		const sf::Color Teal = sf::Color(29, 233, 182);
		static real ScaleFactor = 1.0f;
	}


}