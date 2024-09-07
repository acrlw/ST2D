#pragma once

#include "ShaderProgram.h"

namespace STEditor
{
	using namespace ST;

	struct Color
	{
		float r, g, b, a;
		constexpr Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
			: r(red), g(green), b(blue), a(alpha) {}

		constexpr Color(int red, int green, int blue, int alpha = 255)
			: r(static_cast<float>(red) / 255.0f), g(static_cast<float>(green) / 255.0f), b(static_cast<float>(blue) / 255.0f), a(static_cast<float>(alpha) / 255.0f) {}
	};

	namespace DarkPalette
	{
		constexpr Color Background(0.16f, 0.16f, 0.16f, 1.0f);
		constexpr Color Red(244, 67, 54);
		constexpr Color Green(76, 175, 80);
		constexpr Color Blue(3, 169, 244);
		constexpr Color Yellow(255, 235, 59);
		constexpr Color Cyan(0, 188, 212);
		constexpr Color Pink(233, 30, 99);
		constexpr Color Gray(189, 189, 189);
		constexpr Color Orange(255, 138, 101);
		constexpr Color Teal(29, 233, 182);
		constexpr Color Purple(156, 39, 176);

		constexpr Color LightRed(255, 205, 210);
		constexpr Color LightGreen(139, 195, 74);
		constexpr Color LightBlue(0, 188, 212);
		constexpr Color LightCyan(178, 235, 242);
		constexpr Color LightGray(158, 158, 158);

		constexpr Color DarkRed(211, 47, 47);
		constexpr Color DarkGreen(44, 113, 48);
		constexpr Color DarkBlue(2, 136, 209);

	}

	namespace LightPalette
	{
		constexpr Color Background(1.0f, 1.0f, 1.0f, 1.0f);
		constexpr Color Red(183, 28, 28);
		constexpr Color Green(76, 175, 80);
		constexpr Color Blue(13, 71, 161);
		constexpr Color Yellow(245, 127, 23);
		constexpr Color Cyan(0, 131, 143);
		constexpr Color Pink(194, 24, 91);
		constexpr Color Gray(15, 15, 15);
		constexpr Color Orange(230, 81, 0);
		constexpr Color Teal(0, 105, 92);
		constexpr Color Purple(106, 27, 154);


	}

	namespace Palette
	{
		enum class ThemeMode {
			Dark,
			Light
		};

		static ThemeMode currentThemeMode = ThemeMode::Light;

		static Color Background = DarkPalette::Background;

		static Color Red = DarkPalette::Red;
		static Color Green = DarkPalette::Green;
		static Color Blue = DarkPalette::Blue;
		static Color Yellow = DarkPalette::Yellow;
		static Color Cyan = DarkPalette::Cyan;
		static Color Pink = DarkPalette::Pink;
		static Color Gray = DarkPalette::Gray;
		static Color Orange = DarkPalette::Orange;
		static Color Teal = DarkPalette::Teal;
		static Color Purple = DarkPalette::Purple;

		static Color LightRed = DarkPalette::LightRed;
		static Color LightGreen = DarkPalette::LightGreen;
		static Color LightBlue = DarkPalette::LightBlue;
		static Color LightCyan = DarkPalette::LightCyan;
		static Color LightGray = DarkPalette::LightGray;

		static Color DarkRed = DarkPalette::DarkRed;
		static Color DarkGreen = DarkPalette::DarkGreen;
		static Color DarkBlue = DarkPalette::DarkBlue;

		static void setThemeMode(ThemeMode mode)
		{
			currentThemeMode = mode;
			if (currentThemeMode == ThemeMode::Dark)
			{
				Background = DarkPalette::Background;

				Red = DarkPalette::Red;
				Green = DarkPalette::Green;
				Blue = DarkPalette::Blue;
				Yellow = DarkPalette::Yellow;
				Cyan = DarkPalette::Cyan;
				Pink = DarkPalette::Pink;
				Gray = DarkPalette::Gray;
				Orange = DarkPalette::Orange;
				Teal = DarkPalette::Teal;
				Purple = DarkPalette::Purple;

				LightRed = DarkPalette::LightRed;
				LightGreen = DarkPalette::LightGreen;
				LightBlue = DarkPalette::LightBlue;
				LightCyan = DarkPalette::LightCyan;
				LightGray = DarkPalette::LightGray;

				DarkRed = DarkPalette::DarkRed;
				DarkGreen = DarkPalette::DarkGreen;
				DarkBlue = DarkPalette::DarkBlue;
			}
			else
			{
				Background = LightPalette::Background;

				Red = LightPalette::Red;
				Green = LightPalette::Green;
				Blue = LightPalette::Blue;
				Yellow = LightPalette::Yellow;
				Cyan = LightPalette::Cyan;
				Pink = LightPalette::Pink;
				Gray = LightPalette::Gray;
				Orange = LightPalette::Orange;
				Teal = LightPalette::Teal;
				Purple = LightPalette::Purple;
			}
		}

		
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

	struct MultiCommandsDraw
	{
		std::vector<float> vertices;
		std::vector<int> commands;
		float thickness = 1.0f;
		float pointSize = 10.0f;
		Color fillColor;
	};

	struct Char
	{
		GLuint textureID;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
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
		void point(const Vector2& position, const Color& color, float size = 6.0f);
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

		void arrow(const Vector2& start, const Vector2& end, const Color& color, const float& size = 0.2f, const float& degree = 45);

		void text(const Vector2& position, const Color& color, const std::string& text, const float& scale = 1.0f, bool centered = true);

		void simplex(const Simplex& simplex, const Color& color, bool showIndex = true);
		void polytope(const std::vector<Vector2>& points, const Color& color, float pointSize = 6, bool showIndex = true);

		Vector2 screenToNDC(const Vector2& pos)const;
		Vector2 ndcToScreen(const Vector2& pos)const;
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

		AABB screenAABB() const;

	private:
		void drawGraphicsProgram();
		void drawPointsProgram();
		void drawTextProgram();

		void onZoomView(float xoffset, float yoffset);
		void onTranslateView(float x, float y);

		void updateScreenAABB();
		void buildViewProjectionMatrix();

		void pushVector(std::vector<float>& lines, const Vector2& vec);
		void pushColor(std::vector<float>& lines, const Color& color);

		void initShaders();
		void initRenderSettings();
		void initFont();

		bool m_smoothZooming = false;

		int m_frameBufferWidth = 1920;
		int m_frameBufferHeight = 1080;

		// camera
		float m_orthoSize = 4.0f;
		float m_orthoSizeScaleRatio = 0.15f;
		Vector2 m_scrollMouseStart;
		Vector2 m_scrollMouseScreenStart;

		bool m_isTranslateView = false;
		bool m_translationStart = false;

		Vector2 m_mouseScreenStart;
		float m_zNear = 0.1f;
		float m_zFar = 1000.0f;
		float m_aspectRatio = 16.0f / 9.0f;

		EasingObject<float> m_easingMeterToPixel = EasingObject(100.0f);
		GLFWwindow* m_window = nullptr;

		float m_maxMeterToPixel = 10000.0f;
		float m_meterToPixel = 100.0f;
		float m_pixelToMeter = 1.0f / m_meterToPixel;
		float m_scaleRatio = 0.1f;
		float m_zoomingDuration = 0.3f;

		glm::vec3 m_translationStartPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 m_view;
		glm::mat4 m_projection;

		int m_roundPointSampleCount = 12;

		AABB m_screenAABB;

		//graphics shader
		std::vector<float> m_lines;
		std::vector<ThickLine> m_thickLines;
		std::vector<PolyLines> m_polyLines;
		std::vector<Fill> m_fills;
		std::vector<FillStroke> m_fillStrokes;
		std::vector<MultiCommandsDraw> m_multiCommandsDraws;

		std::vector<float> m_ndcLines;
		std::vector<MultiCommandsDraw> m_ndcMultiCommandsDraws;

		size_t m_graphicsDataSize = 0;
		size_t m_graphicsDataCapacity = 7;

		//round point shader
		std::vector<float> m_points;
		std::vector<float> m_ndcPoints;

		size_t m_pointDataSize = 0;
		size_t m_pointDataCapacity = 8;

		//font shader
		std::vector<float> m_text;
		std::vector<GLuint> m_textureIDs;
		std::vector<Color> m_textColors;
		std::map<GLchar, Char> m_characters;

		size_t m_textDataSize = 0;
		size_t m_textDataCapacity = 4 * 6;
		int m_fontHeight = 18;

		ShaderProgram m_graphicsProgram;
		ShaderProgram m_fontProgram;
		ShaderProgram m_pointProgram;

		unsigned int m_graphicsVAO;
		unsigned int m_graphicsVBO;

		unsigned int m_fontVAO;
		unsigned int m_fontVBO;

		unsigned int m_pointVAO;
		unsigned int m_pointVBO;

		FT_Library m_ftLibrary;
		FT_Face m_ftFace;

		float m_simplexIndexOffset = 10.0f;
	};

	
}
