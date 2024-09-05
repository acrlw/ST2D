#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H 

#include "ShaderProgram.h"
#include "glm/glm.hpp"

namespace STEditor
{
	using namespace ST;

	struct Color
	{
		float r, g, b, a;
		Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
			: r(red), g(green), b(blue), a(alpha) {}

		Color(int red, int green, int blue, int alpha = 255)
			: r(static_cast<float>(red) / 255.0f), g(static_cast<float>(green) / 255.0f), b(static_cast<float>(blue) / 255.0f), a(static_cast<float>(alpha) / 255.0f) {}
	};

	namespace DarkPalette
	{
		const Color Red = Color(244, 67, 54);
		const Color Green = Color(76, 175, 80);
		const Color Blue = Color(3, 169, 244);
		const Color Yellow = Color(255, 235, 59);
		const Color Cyan = Color(0, 188, 212);
		const Color Pink = Color(233, 30, 99);
		const Color Gray = Color(189, 189, 189);
		const Color Orange = Color(255, 138, 101);
		const Color Teal = Color(29, 233, 182);
		const Color Purple = Color(156, 39, 176);

		const Color LightRed = Color(255, 205, 210);
		const Color LightGreen = Color(139, 195, 74);
		const Color LightBlue = Color(0, 188, 212);
		const Color LightCyan = Color(178, 235, 242);
		const Color LightGray = Color(158, 158, 158);

		const Color DarkRed = Color(211, 47, 47);
		const Color DarkGreen = Color(44, 113, 48);
		const Color DarkBlue = Color(2, 136, 209);

	}

	namespace LightPalette
	{
		const Color Red = Color(183, 28, 28);
		const Color Green = Color(76, 175, 80);
		const Color Blue = Color(13, 71, 161);
		const Color Yellow = Color(249, 168, 37);
		const Color Cyan = Color(0, 131, 143);
		const Color Pink = Color(194, 24, 91);
		const Color Gray = Color(66, 66, 66);
		const Color Orange = Color(230, 81, 0);
		const Color Teal = Color(0, 105, 92);
		const Color Purple = Color(106, 27, 154);
	}

	struct PolyLines
	{
		std::vector<float> vertices;
		float thickness = 1.0f;
		bool closed = false;
	};

	struct Fill
	{
		std::vector<float> vertices;
	};

	struct FillStroke
	{
		std::vector<float> vertices;
		Color fillColor;
		float thickness = 1.0f;
	};

	struct ThickLine
	{
		std::vector<float> vertices;
		float thickness = 1.0f;
	};

	struct Points
	{
		std::vector<float> vertices;
		float size = 1.0f;
	};




	class Renderer2D
	{
	public:

		Renderer2D(GLFWwindow* window);
		~Renderer2D();

		void onRenderStart();
		void onRenderEnd();

		//screen space
		void line(int x1, int y1, int x2, int y2, const Color& color);
		void line(int x1, int y1, int x2, int y2, int r, int g, int b, int a);
		void point(int x, int y, const Color& color, float size = 1.0f);

		//world space
		void point(const Vector2& position, const Color& color, float size = 1.0f);
		void line(const Vector2& start, const Vector2& end, int r, int g, int b, int a);
		void line(const Vector2& start, const Vector2& end, float r, float g, float b, float a);
		void line(const Vector2& start, const Vector2& end, const Color& color);

		void fill(const std::vector<Vector2>& points, const Color& color);
		void fillAndStroke(const std::vector<Vector2>& points, const Color& fillColor, const Color& strokeColor, float thickness = 1.0f);

		void thickLine(const Vector2& start, const Vector2& end, const Color& color, float thickness = 2.0f);
		void dashedLine(const Vector2& start, const Vector2& end, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);

		void polyLines(const std::vector<Vector2>& points, const Color& color);
		void closedLines(const std::vector<Vector2>& points, const Color& color);
		void polyDashedLines(const std::vector<Vector2>& points, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);
		void polyThickLine(const std::vector<Vector2>& points, const Color& color, float thickness = 2.0f);
		void polyDashedThickLine(const std::vector<Vector2>& points, const Color& color, float thickness = 2.0f, float dashLength = 0.1f, float gapLength = 0.1f);
		void polyClosedThickLines(const std::vector<Vector2>& points, const Color& color, float thickness = 2.0f);

		void shape(const Transform& transform, Shape* shape, const Color& color);
		void polygon(const Transform& transform, Shape* shape, const Color& color);
		void edge(const Transform& transform, Shape* shape, const Color& color);
		void circle(const Transform& transform, Shape* shape, const Color& color);
		void capsule(const Transform& transform, Shape* shape, const Color& color);
		void ellipse(const Transform& transform, Shape* shape, const Color& color);
		void orientation(const Transform& transform);

		void aabb(const AABB& aabb, const Color& color);
		void dashedAABB(const AABB& aabb, const Color& color, float dashLength = 0.1f, float gapLength = 0.1f);

		void arrow(const Color& color, const float& size = 0.2f, const float& degree = 45);

		void text(const Vector2& position, const Color& color, const std::string& text, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, int value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, float value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);
		void text(const Vector2& position, const Color& color, unsigned int value, const unsigned int& size = 16, const Vector2& offset = { 0.0f, 0.0f }, bool centered = true);

		void simplex(const Simplex& simplex, const Color& color, bool showIndex = true, const unsigned int& fontSize = 18);
		void polytope(const std::vector<Vector2>& points,
			const Color& color, float pointSize = 16, const unsigned int& indexSize = 18,
			bool showIndex = true);

		Vector2 worldToScreen(const Vector2& worldPos) const;
		Vector2 screenToWorld(const Vector2& screenPos) const;

		void onRender();
		void onUpdate(float deltaTime);

		void onFrameBufferResize(int width, int height);
		void onKeyButton(int key, int scancode, int action, int mods);
		void onMouseButton(int button, int action, int mods);
		void onMouseMoved(double xpos, double ypos);
		void onMouseScroll(double xoffset, double yoffset);

		float meterToPixel() const;
		float pixelToMeter() const;

		bool& smoothZooming() { return m_smoothZooming; }
		float& scaleRatio() { return m_scaleRatio; }

	private:
		void onZoomView(float xoffset, float yoffset);
		void onTranslateView(float x, float y);
		void buildMVPMatrix();

		void pushVector(std::vector<float>& lines, const Vector2& vec);
		void pushColor(std::vector<float>& lines, const Color& color);

		void initRenderSettings();

		bool m_smoothZooming = false;

		int m_frameBufferWidth = 1920;
		int m_frameBufferHeight = 1080;

		// camera
		float m_orthoSize = 4.0f;
		float m_orthoSizeScaleRatio = 0.15f;
		Vector2 m_scrollMouseStart;

		bool m_isTranslateView = false;
		bool m_translationStart = false;
		float m_translateSensitivity = 0.5f;
		Vector2 m_mouseStart;
		float m_zNear = 0.1f;
		float m_zFar = 1000.0f;
		float m_aspectRatio = 16.0f / 9.0f;

		EasingObject<float> m_easingMeterToPixel = EasingObject(100.0f);
		GLFWwindow* m_window = nullptr;

		float m_meterToPixel = 100.0f;
		float m_scaleRatio = 0.1f;

		glm::vec3 m_translationStartPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 m_model;
		glm::mat4 m_view;
		glm::mat4 m_projection;

		//render
		std::vector<float> m_lines;
		std::vector<float> m_points;
		std::vector<ThickLine> m_thickLines;
		std::vector<PolyLines> m_polyLines;
		std::vector<Fill> m_fills;
		std::vector<FillStroke> m_fillStrokes;

		std::vector<float> m_ndcLines;
		std::vector<float> m_ndcPoints;


		ShaderProgram m_shaderProgram;

		unsigned int m_verticesVao;
		unsigned int m_verticesVbo;

		size_t m_capacity = 7;
		size_t m_size = 0;
	};

	
}
