#include "Renderer2D.h"

namespace STEditor
{
	Renderer2D::Renderer2D()
	{
		//read shader source from path res/shaders/vert.glsl
		auto readShader = [](const std::string& path) -> std::string
			{
				std::ifstream file(path);
				if (!file)
				{
					std::cout << std::format("[ERROR] Unable to read shader from {0}", path) << '\n';
					return "";
				}
				std::stringstream buffer;
				buffer << file.rdbuf();
				return buffer.str();
			};

		Shader vertexShader, fragmentShader, geometryShader;

		vertexShader.source = readShader("./Resource/Shaders/vert.glsl");
		fragmentShader.source = readShader("./Resource/Shaders/frag.glsl");
		geometryShader.source = readShader("./Resource/Shaders/geom.glsl");

		if (vertexShader.source.empty())
			APP_WARN("Vertex shader is empty");

		if (fragmentShader.source.empty())
			APP_WARN("Fragment shader is empty");

		if (geometryShader.source.empty())
			APP_WARN("Geometry shader is empty");


		m_shaderProgram.addVertexShader(vertexShader);
		m_shaderProgram.addFragmentShader(fragmentShader);
		m_shaderProgram.addGeometryShader(geometryShader);
		m_shaderProgram.compileShader();
		m_shaderProgram.link();

		initRenderSettings();
	}

	Renderer2D::~Renderer2D()
	{
		APP_INFO("Delete VAO ({}) and VBO ({}) ", m_vao, m_vbo);

		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);

		m_shaderProgram.destroy();
	}

	void Renderer2D::initRenderSettings()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_shaderProgram.use();

		APP_INFO("Created VAO({}) and VBO({})", m_vao, m_vbo);
	}

	void Renderer2D::onRenderStart()
	{
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(float), m_lines.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Renderer2D::onRenderEnd()
	{
		m_lines.clear();
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, const Color& color)
	{
		float ndcX1 = (2.0f * static_cast<float>(x1) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY1 = 1.0f - (2.0f * static_cast<float>(y1) / static_cast<float>(m_frameBufferHeight));
		float ndcX2 = (2.0f * static_cast<float>(x2) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY2 = 1.0f - (2.0f * static_cast<float>(y2) / static_cast<float>(m_frameBufferHeight));

		line({ ndcX1, ndcY1 }, { ndcX2, ndcY2 }, color);
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
	{
		float ndcX1 = (2.0f * static_cast<float>(x1) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY1 = 1.0f - (2.0f * static_cast<float>(y1) / static_cast<float>(m_frameBufferHeight));
		float ndcX2 = (2.0f * static_cast<float>(x2) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY2 = 1.0f - (2.0f * static_cast<float>(y2) / static_cast<float>(m_frameBufferHeight));

		line({ ndcX1, ndcY1 }, { ndcX2, ndcY2 }, r, g, b, a);
	}

	void Renderer2D::line(const Vector2& start, const Vector2& end, int r, int g, int b, int a)
	{
		line(start, end, Color(r, g, b, a));
	}

	void Renderer2D::line(const Vector2& start, const Vector2& end, float r, float g, float b, float a)
	{
		line(start, end, Color(r, g, b, a));
	}

	void Renderer2D::line(const Vector2& start, const Vector2& end, const Color& color)
	{
		linePushVector(start);
		linePushColor(color);
		linePushVector(end);
		linePushColor(color);
	}

	void Renderer2D::fill(const std::vector<Vector2>& points, const Color& color)
	{

	}

	void Renderer2D::thickLine(const Vector2& start, const Vector2& end, const Color& color, float thickness)
	{

	}

	void Renderer2D::dashedLine(const Vector2& start, const Vector2& end, const Color& color, float dashLength,
		float gapLength)
	{
	}

	void Renderer2D::polyDashedLines(const std::vector<Vector2>& points, const Color& color, float dashLength,
		float gapLength)
	{
	}

	void Renderer2D::polyThickLine(const std::vector<Vector2>& points, const Color& color, float thickness)
	{
	}

	void Renderer2D::polyDashedThickLine(const std::vector<Vector2>& points, const Color& color, float thickness,
		float dashLength, float gapLength)
	{
	}

	void Renderer2D::shape(const Transform& transform, Shape* shape, const sf::Color& color)
	{
		CORE_ASSERT(shape != nullptr, "Null reference of shape");

		switch (shape->type())
		{
		case ShapeType::Polygon:
			polygon(transform, shape, color);
			break;
		case ShapeType::Edge:
			edge(transform, shape, color);
			break;
		case ShapeType::Circle:
			circle(transform, shape, color);
			break;
		case ShapeType::Capsule:
			capsule(transform, shape, color);
			break;
		case ShapeType::Ellipse:
			ellipse(transform, shape, color);
			break;
		}
	}

	void Renderer2D::polygon(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::edge(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::rectangle(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::circle(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::capsule(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::ellipse(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::orientation(const Transform& transform)
	{
	}

	void Renderer2D::aabb(const AABB& aabb, const Color& color)
	{
	}

	void Renderer2D::dashedAABB(const AABB& aabb, const Color& color, float dashLength, float gapLength)
	{
	}

	void Renderer2D::dashedShape(const Transform& transform, Shape* shape, const sf::Color& color)
	{
		CORE_ASSERT(shape != nullptr, "Null reference of shape");

		switch (shape->type())
		{
		case ShapeType::Polygon:
			dashedPolygon(transform, shape, color);
			break;
		case ShapeType::Edge:
			dashedEdge(transform, shape, color);
			break;
		case ShapeType::Circle:
			dashedCircle(transform, shape, color);
			break;
		case ShapeType::Capsule:
			dashedCapsule(transform, shape, color);
			break;
		case ShapeType::Ellipse:
			dashedEllipse(transform, shape, color);
			break;
		}
	}

	void Renderer2D::dashedPolygon(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::dashedEdge(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::dashedRectangle(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::dashedCircle(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::dashedCapsule(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::dashedEllipse(const Transform& transform, Shape* shape, const sf::Color& color)
	{
	}

	void Renderer2D::arrow(const Color& color, const float& size, const float& degree)
	{
	}

	void Renderer2D::text(const Vector2& position, const Color& color, const std::string& text,
		const unsigned int& size, const Vector2& offset, bool centered)
	{
	}

	void Renderer2D::text(const Vector2& position, const Color& color, int value, const unsigned int& size,
		const Vector2& offset, bool centered)
	{
	}

	void Renderer2D::text(const Vector2& position, const Color& color, float value, const unsigned int& size,
		const Vector2& offset, bool centered)
	{
	}

	void Renderer2D::text(const Vector2& position, const Color& color, unsigned int value, const unsigned int& size,
		const Vector2& offset, bool centered)
	{
	}

	void Renderer2D::simplex(const Simplex& simplex, const sf::Color& color, bool showIndex,
		const unsigned int& fontSize)
	{
	}

	void Renderer2D::polytope(const std::vector<Vector2>& points, const sf::Color& color, float pointSize, const unsigned int& indexSize, bool showIndex)
	{

	}

	void Renderer2D::polyLines(const std::vector<Vector2>& points, const Color& color)
	{
		if (points.size() < 2)
			return;

		for (size_t i = 1; i < points.size() - 1; i++)
			line(points[i - 1], points[i], color);


	}

	void Renderer2D::closeLines(const std::vector<Vector2>& points, const Color& color)
	{
		if (points.size() < 2)
			return;

		for (size_t i = 1; i < points.size(); i++)
			line(points[i - 1], points[i], color);

		line(points.back(), points.front(), color);
	}

	void Renderer2D::onRender()
	{
		onRenderStart();

		if(m_lines.empty())
			return;

		GLint previousDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);

		glDepthFunc(GL_ALWAYS);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glDrawArrays(GL_LINES, 0, m_lines.size() / 6);

		glBindVertexArray(0);

		glDepthFunc(previousDepthFunc);

		onRenderEnd();
	}

	void Renderer2D::resizeFrameBuffer(int width, int height)
	{
		m_frameBufferWidth = width;
		m_frameBufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void Renderer2D::linePushVector(const Vector2& vec)
	{
		m_lines.push_back(vec.x);
		m_lines.push_back(vec.y);
	}

	void Renderer2D::linePushColor(const Color& color)
	{
		m_lines.push_back(color.r);
		m_lines.push_back(color.g);
		m_lines.push_back(color.b);
		m_lines.push_back(color.a);
	}
}
