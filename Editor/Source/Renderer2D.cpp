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
	}

	Renderer2D::~Renderer2D()
	{
		m_shaderProgram.destroy();
	}

	void Renderer2D::onRenderStart()
	{

	}

	void Renderer2D::onRenderEnd()
	{

	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
	{

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
	}

	void Renderer2D::flush()
	{

	}
}
