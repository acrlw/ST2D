#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include "ST2DCore.h"
#include "Camera2D.h"

namespace STEditor
{

	namespace RenderConstant
	{
		constexpr ST::real PointSize = 2.0f;
		constexpr int BorderSize = 1;
		constexpr int FillAlpha = 38;
		constexpr int BasicCirclePointCount = 60;
		constexpr real BasicDashLength = 0.02f;
		constexpr real BasicDashGap = 0.02f;
		const sf::Color Yellow = sf::Color(255, 235, 59);
		const sf::Color Red = sf::Color(244, 67, 54);
		const sf::Color LightRed = sf::Color(255, 205, 210);
		const sf::Color DarkRed = sf::Color(211, 47, 47);
		const sf::Color Blue = sf::Color(3, 169, 244);
		const sf::Color LightBlue = sf::Color(0, 188, 212);
		const sf::Color DarkBlue = sf::Color(2, 136, 209);
		const sf::Color Cyan = sf::Color(0, 188, 212);
		const sf::Color LightCyan = sf::Color(178, 235, 242);
		const sf::Color Green = sf::Color(76, 175, 80);
		const sf::Color LightGreen = sf::Color(139, 195, 74);
		const sf::Color Pink = sf::Color(233, 30, 99);
		const sf::Color DarkGreen = sf::Color(44, 113, 48);
		const sf::Color LightGray = sf::Color(158, 158, 158);
		const sf::Color Gray = sf::Color(189, 189, 189);
		const sf::Color Orange = sf::Color(255, 138, 101);
		const sf::Color Teal = sf::Color(29, 233, 182);
		static real ScaleFactor = 1.0f;
	}

	class RenderSFMLImpl
	{
	public:
		static sf::Vector2f toVector2f(const Vector2& vector);
		static void renderPoint(sf::RenderWindow& window, Camera2D& camera, const Vector2& point, const sf::Color& color,
			real pointSize = RenderConstant::PointSize);

		static void renderPolyLine(sf::RenderWindow& window, Camera2D& camera, const std::vector<Vector2>& points,
			const sf::Color& color);

		static void renderPolyThickLine(sf::RenderWindow& window, Camera2D& camera, const std::vector<Vector2>& points,
			const sf::Color& color, const real& thickness = 2.0f);

		static void renderLine(sf::RenderWindow& window, Camera2D& camera, const Vector2& p1, const Vector2& p2,
			const sf::Color& color);

		static void renderPoints(sf::RenderWindow& window, Camera2D& camera, const std::vector<Vector2>& points,
			const sf::Color& color);

		static void renderLines(sf::RenderWindow& window, Camera2D& camera,
			const std::vector<std::pair<Vector2, Vector2>>& lines, const sf::Color& color);

		static void renderShape(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderPolygon(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderEdge(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderRectangle(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderCircle(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderCapsule(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderEllipse(sf::RenderWindow& window, Camera2D& camera, const Transform& transform,
			Shape* shape, const sf::Color& color);
		static void renderAngleLine(sf::RenderWindow& window, Camera2D& camera, const Transform& transform);
		
		static void renderAABB(sf::RenderWindow& window, Camera2D& camera, const AABB& aabb, const sf::Color& color);


		static void renderPolyDashedLine(sf::RenderWindow& window, Camera2D& camera, const std::vector<Vector2>& points,
			const sf::Color& color, const real& dashLength = RenderConstant::BasicDashLength,
			const real& dashGap = RenderConstant::BasicDashGap);

		static void renderDashedLine(sf::RenderWindow& window, Camera2D& camera, const Vector2& p1, const Vector2& p2,
			const sf::Color& color, const real& dashLength = RenderConstant::BasicDashLength,
			const real& dashGap = RenderConstant::BasicDashGap);
		
		static void renderArrow(sf::RenderWindow& window, Camera2D& camera, const Vector2& start, const Vector2& end,
			const sf::Color& color,
			const real& size = 0.30f, const real& degree = 20);

		static void renderText(sf::RenderWindow& window, Camera2D& camera, const Vector2& position, const sf::Font& font,
			const std::string& text, const sf::Color& color, const unsigned int& size = 18,
			const Vector2& screenOffset = { 0, 0 });

		static void renderFloat(sf::RenderWindow& window, Camera2D& camera, const Vector2& position, const sf::Font& font,
			const real& value, const sf::Color& color, const unsigned int& size = 18,
			const Vector2& screenOffset = Vector2(-0.2f, -0.2f));

		static void renderInt(sf::RenderWindow& window, Camera2D& camera, const Vector2& position, const sf::Font& font,
			const int& value, const sf::Color& color, const unsigned int& size = 18,
			const Vector2& screenOffset = Vector2(-0.2f, -0.2f));

		static void renderUInt(sf::RenderWindow& window, Camera2D& camera, const Vector2& position, const sf::Font& font,
			const uint32_t& value, const sf::Color& color, const unsigned int& size = 18,
			const Vector2& offset = Vector2(0.0f, -0.2f));

		static void renderSimplex(sf::RenderWindow& window, Camera2D& camera, const Simplex& simplex,
			const sf::Color& color, const sf::Font& font, bool showIndex = true, const unsigned int& indexSize = 18);

		static void renderThickLine(sf::RenderWindow& window, Camera2D& camera, const Vector2& p1, const Vector2& p2,
			const sf::Color& color, const real& thickness = 2.0f);


		static void renderPolytope(sf::RenderWindow& window, Camera2D& camera, const std::vector<Vector2>& polytope,
			const sf::Color& color, const sf::Font& font,
			real pointSize = RenderConstant::PointSize, const unsigned int& indexSize = 18,
			bool showIndex = true);

		static void renderPosition(sf::RenderWindow& window, Camera2D& camera, const Vector2& position,
			const sf::Color& color, const sf::Font& font, const unsigned int& size = 18,
			const Vector2& screenOffset = Vector2(0.6f, 0.6f));
	};
}
