#pragma once

#include "ST2DCore.h"
#include <ft2build.h>
#include FT_FREETYPE_H 

namespace STEditor
{
	using namespace ST;

	struct Color
	{
		float r, g, b, a;
		Color(float red, float green, float blue, float alpha = 1.0f)
			: r(red), g(green), b(blue), a(alpha) {}
		Color(int red, int green, int blue, int alpha = 255)
			: r(static_cast<float>(red) / 255.0f), g(static_cast<float>(green) / 255.0f), b(static_cast<float>(blue) / 255.0f), a(static_cast<float>(alpha) / 255.0f) {}
	};

	namespace Palette
	{
		const Color Yellow = Color(255, 235, 59);
		const Color Red = Color(244, 67, 54);
		const Color LightRed = Color(255, 205, 210);
		const Color DarkRed = Color(211, 47, 47);
		const Color Blue = Color(3, 169, 244);
		const Color LightBlue = Color(0, 188, 212);
		const Color DarkBlue = Color(2, 136, 209);
		const Color Cyan = Color(0, 188, 212);
		const Color LightCyan = Color(178, 235, 242);
		const Color Green = Color(76, 175, 80);
		const Color LightGreen = Color(139, 195, 74);
		const Color Pink = Color(233, 30, 99);
		const Color DarkGreen = Color(44, 113, 48);
		const Color LightGray = Color(158, 158, 158);
		const Color Gray = Color(189, 189, 189);
		const Color Orange = Color(255, 138, 101);
		const Color Teal = Color(29, 233, 182);
	}



	class Renderer2D
	{
	public:

		Renderer2D();

		//screen space
		void line(int x1, int y1, int x2, int y2, int r, int g, int b, int a);

		//world space
		void line(const Vector2& start, const Vector2& end, int r, int g, int b, int a);
		void line(const Vector2& start, const Vector2& end, float r, float g, float b, float a);
		void line(const Vector2& start, const Vector2& end, const Color& color);

		void fill(const std::vector<Vector2>& points, const Color& color);

		void thickLine(const Vector2& start, const Vector2& end, const Color& color, float thickness = 2.0f);
		void dashedLine(const Vector2& start, const Vector2& end, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);

		void polyLines(const std::vector<Vector2>& points, const Color& color);
		void polyDashedLines(const std::vector<Vector2>& points, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);
		void polyThickLine(const std::vector<Vector2>& points, const Color& color, float thickness = 2.0f);
		void polyDashedThickLine(const std::vector<Vector2>& points, const Color& color, float thickness = 2.0f, float dashLength = 0.1f, float gapLength = 0.1f);

		void shape(const Transform& transform, Shape* shape, const sf::Color& color);
		void polygon(const Transform& transform, Shape* shape, const sf::Color& color);
		void edge(const Transform& transform, Shape* shape, const sf::Color& color);
		void rectangle(const Transform& transform, Shape* shape, const sf::Color& color);
		void circle(const Transform& transform, Shape* shape, const sf::Color& color);
		void capsule(const Transform& transform, Shape* shape, const sf::Color& color);
		void ellipse(const Transform& transform, Shape* shape, const sf::Color& color);
		void orientation(const Transform& transform);

		void aabb(const AABB& aabb, const Color& color);
		void dashedAABB(const AABB& aabb, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);

		void dashedShape(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedPolygon(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedEdge(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedRectangle(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedCircle(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedCapsule(const Transform& transform, Shape* shape, const sf::Color& color);
		void dashedEllipse(const Transform& transform, Shape* shape, const sf::Color& color);

		void arrow(const Color& color, const float& size = 0.2f, const float& degree = 45);

		void text(const Vector2& position, const Color& color, const std::string& text, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, int value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, float value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, unsigned int value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);

		void simplex(const Simplex& simplex, const sf::Color& color, bool showIndex = true, const unsigned int& fontSize = 18);
		void polytope(const std::vector<Vector2>& points,
			const sf::Color& color, float pointSize = 16, const unsigned int& indexSize = 18,
			bool showIndex = true);


		void flush();

	private:
		std::vector<float> m_lines;

	};

	
}
