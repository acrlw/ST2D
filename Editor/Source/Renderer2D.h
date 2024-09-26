#pragma once

#include "ShaderProgram.h"
#include "Palette.h"

namespace STEditor
{
	using namespace ST;

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
		void arrow(int x1, int y1, int x2, int y2, const Color& color, const float& size = 0.2f, const float& degree = 45);
		void text(int screenX, int screenY, const Color& color, const std::string& text, const float& scale = 1.0f, bool centered = true);

		//world space
		void point(const Vector2& position, const Color& color, float size = 4.0f);
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

		float m_maxMeterToPixel = 20000.0f;
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
		int m_fontHeight = 14;

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
