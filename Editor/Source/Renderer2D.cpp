#include "Renderer2D.h"

namespace STEditor
{
	Renderer2D::Renderer2D(GLFWwindow* window) : m_window(window)
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

		m_model = glm::mat4(1.0f);
		buildMVPMatrix();

		m_easingMeterToPixel.setEasingFunction(EasingFunction::easeOutCubic);
		
	}

	Renderer2D::~Renderer2D()
	{
		APP_INFO("Delete VAO ({}) and VBO ({}) ", m_verticesVao, m_verticesVbo);

		glDeleteVertexArrays(1, &m_verticesVao);
		glDeleteBuffers(1, &m_verticesVbo);

		m_shaderProgram.destroy();
	}

	void Renderer2D::initRenderSettings()
	{
		m_shaderProgram.use();

		glGenVertexArrays(1, &m_verticesVao);
		glBindVertexArray(m_verticesVao);

		glGenBuffers(1, &m_verticesVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_verticesVbo);
		m_size = 0;
		m_capacity = 7;
		glBufferData(GL_ARRAY_BUFFER, m_capacity * sizeof(float), nullptr, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		APP_INFO("Created Line VAO({}) and VBO({})", m_verticesVao, m_verticesVbo);
	}

	void Renderer2D::onRenderStart()
	{
		std::vector<float> vertices = m_lines;

		for (auto&& elem : m_thickLines)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for (auto&& elem : m_polyLines)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		vertices.insert(vertices.end(), m_points.begin(), m_points.end());

		for(auto&& elem: m_fills)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for (auto&& elem : m_fillStrokes)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		vertices.insert(vertices.end(), m_ndcLines.begin(), m_ndcLines.end());

		vertices.insert(vertices.end(), m_ndcPoints.begin(), m_ndcPoints.end());

		glBindVertexArray(m_verticesVao);
		glBindBuffer(GL_ARRAY_BUFFER, m_verticesVbo);
		m_size = vertices.size();
		if (m_size > m_capacity)
		{
			m_capacity = m_size * 2;
			glBufferData(GL_ARRAY_BUFFER, m_capacity * sizeof(float), nullptr, GL_STATIC_DRAW);
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_size * sizeof(float), vertices.data());
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Renderer2D::onRender()
	{
		buildMVPMatrix();

		onRenderStart();

		GLint previousDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
		glDepthFunc(GL_ALWAYS);

		glBindVertexArray(m_verticesVao);
		glBindBuffer(GL_ARRAY_BUFFER, m_verticesVbo);

		m_shaderProgram.setUniformMat4f("model", m_model);
		m_shaderProgram.setUniformMat4f("view", m_view);
		m_shaderProgram.setUniformMat4f("projection", m_projection);

		size_t lineSize = m_lines.size() / 7;
		size_t offset = lineSize;
		if (!m_lines.empty())
			glDrawArrays(GL_LINES, 0, lineSize);


		if (!m_thickLines.empty())
		{
			for (auto&& elem : m_thickLines)
			{
				glLineWidth(elem.thickness);
				glDrawArrays(GL_LINES, offset, elem.vertices.size() / 7);
				offset += elem.vertices.size() / 7;
			}
		}

		if (!m_polyLines.empty())
		{
			for (auto&& elem : m_polyLines)
			{
				glLineWidth(elem.thickness);
				if (elem.closed)
					glDrawArrays(GL_LINE_LOOP, offset, elem.vertices.size() / 7);
				else
					glDrawArrays(GL_LINE_STRIP, offset, elem.vertices.size() / 7);
				offset += elem.vertices.size() / 7;
			}
		}

		if (!m_points.empty())
		{
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, offset, m_points.size() / 7);
			offset += m_points.size() / 7;
		}

		if (!m_fills.empty())
		{
			for (auto&& elem : m_fills)
			{
				glDrawArrays(GL_TRIANGLE_FAN, offset, elem.vertices.size() / 7);
				offset += elem.vertices.size() / 7;
			}
		}

		if (!m_fillStrokes.empty())
		{
			for (auto&& elem : m_fillStrokes)
			{
				m_shaderProgram.setUniform1i("isFillMode", 0);

				glLineWidth(elem.thickness);
				glDrawArrays(GL_LINE_LOOP, offset, elem.vertices.size() / 7);

				m_shaderProgram.setUniform1i("isFillMode", 1);
				m_shaderProgram.setUniform4f("fillColor", elem.fillColor.r, elem.fillColor.g, elem.fillColor.b, elem.fillColor.a);

				glDrawArrays(GL_TRIANGLE_FAN, offset, elem.vertices.size() / 7);

				offset += elem.vertices.size() / 7;
			}
			m_shaderProgram.setUniform1i("isFillMode", 0);
		}

		if (!m_ndcLines.empty())
		{
			glm::mat4 identity = glm::mat4(1.0f);

			glLineWidth(1.0f);

			m_shaderProgram.setUniformMat4f("model", identity);
			m_shaderProgram.setUniformMat4f("view", identity);
			m_shaderProgram.setUniformMat4f("projection", identity);

			glDrawArrays(GL_LINES, offset, m_ndcLines.size() / 7);
			offset += m_ndcLines.size() / 7;
		}

		if (!m_ndcPoints.empty())
		{
			glm::mat4 identity = glm::mat4(1.0f);

			glPointSize(8.0f);
			m_shaderProgram.setUniformMat4f("model", identity);
			m_shaderProgram.setUniformMat4f("view", identity);
			m_shaderProgram.setUniformMat4f("projection", identity);

			glDrawArrays(GL_POINTS, offset, m_ndcPoints.size() / 7);
			offset += m_ndcPoints.size() / 7;
		}

		glDepthFunc(previousDepthFunc);

		onRenderEnd();
	}

	void Renderer2D::onUpdate(float deltaTime)
	{
		if(m_smoothZooming)
			m_easingMeterToPixel.update(deltaTime);
		else
			m_easingMeterToPixel.finish();

		bool finished = m_easingMeterToPixel.isFinished();
		if (!finished)
		{
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			Vector2 currentMouse = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

			Vector2 delta = m_scrollMouseStart - currentMouse;
			m_cameraPosition += glm::vec3(delta.x, delta.y, 0.0f);
		}
		else
		{
			m_scrollMouseStart.clear();
		}
	}

	void Renderer2D::onRenderEnd()
	{
		glLineWidth(1.0f);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_lines.clear();
		m_points.clear();
		m_thickLines.clear();
		m_polyLines.clear();
		m_fills.clear();
		m_fillStrokes.clear();
		m_ndcLines.clear();
		m_ndcPoints.clear();

		m_lines.shrink_to_fit();
		m_points.shrink_to_fit();
		m_thickLines.shrink_to_fit();
		m_polyLines.shrink_to_fit();
		m_fills.shrink_to_fit();
		m_fillStrokes.shrink_to_fit();
		m_ndcLines.shrink_to_fit();
		m_ndcPoints.shrink_to_fit();
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, const Color& color)
	{
		float ndcX1 = (2.0f * static_cast<float>(x1) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY1 = 1.0f - (2.0f * static_cast<float>(y1) / static_cast<float>(m_frameBufferHeight));
		float ndcX2 = (2.0f * static_cast<float>(x2) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY2 = 1.0f - (2.0f * static_cast<float>(y2) / static_cast<float>(m_frameBufferHeight));

		pushVector(m_ndcLines, { ndcX1, ndcY1 });
		pushColor(m_ndcLines, color);
		pushVector(m_ndcLines, { ndcX2, ndcY2 });
		pushColor(m_ndcLines, color);
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
	{
		float ndcX1 = (2.0f * static_cast<float>(x1) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY1 = 1.0f - (2.0f * static_cast<float>(y1) / static_cast<float>(m_frameBufferHeight));
		float ndcX2 = (2.0f * static_cast<float>(x2) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY2 = 1.0f - (2.0f * static_cast<float>(y2) / static_cast<float>(m_frameBufferHeight));
		Color color(r, g, b, a);

		pushVector(m_ndcLines, { ndcX1, ndcY1 });
		pushColor(m_ndcLines, color);
		pushVector(m_ndcLines, { ndcX2, ndcY2 });
		pushColor(m_ndcLines, color);
	}

	void Renderer2D::point(int x, int y, const Color& color, float size)
	{
		float ndcX = (2.0f * static_cast<float>(x) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY = 1.0f - (2.0f * static_cast<float>(y) / static_cast<float>(m_frameBufferHeight));

		pushVector(m_ndcPoints, { ndcX, ndcY });
		pushColor(m_ndcPoints, color);
	}

	void Renderer2D::point(const Vector2& position, const Color& color, float size)
	{
		pushVector(m_points, position);
		pushColor(m_points, color);
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
		pushVector(m_lines, start);
		pushColor(m_lines, color);
		pushVector(m_lines, end);
		pushColor(m_lines, color);
	}

	void Renderer2D::fill(const std::vector<Vector2>& points, const Color& color)
	{
		if (points.size() < 2)
			return;

		Fill shape;
		for(auto&& elem: points)
		{
			pushVector(shape.vertices, elem);
			pushColor(shape.vertices, color);
		}
		m_fills.push_back(shape);
	}

	void Renderer2D::fillAndStroke(const std::vector<Vector2>& points, const Color& fillColor, const Color& strokeColor,
		float thickness)
	{
		if (points.size() < 2)
			return;

		FillStroke shape;
		for (auto&& elem : points)
		{
			pushVector(shape.vertices, elem);
			pushColor(shape.vertices, strokeColor);
		}
		shape.fillColor = fillColor;
		shape.thickness = thickness;
		m_fillStrokes.emplace_back(shape);
	}

	void Renderer2D::thickLine(const Vector2& start, const Vector2& end, const Color& color, float thickness)
	{
		ThickLine line;
		pushVector(line.vertices, start);
		pushColor(line.vertices, color);
		pushVector(line.vertices, end);
		pushColor(line.vertices, color);
		line.thickness = thickness;
		m_thickLines.push_back(line);
	}

	void Renderer2D::dashedLine(const Vector2& start, const Vector2& end, const Color& color, float dashLength,
		float gapLength)
	{
		Vector2 direction = end - start;
		real length = direction.length();
		real lengthStep = dashLength / length;
		real step = (dashLength + gapLength) / length;
		for (real i = 0; i <= 1; i += step)
		{
			Vector2 dashStart = start + direction * i;
			Vector2 dashEnd = start + direction * Math::min(i + lengthStep, 1.0f);
			line(dashStart, dashEnd, color);
		}
	}

	void Renderer2D::polyDashedLines(const std::vector<Vector2>& points, const Color& color, float dashLength,
		float gapLength)
	{
		if (points.size() < 2)
			return;

		real dashResidual = 0.0f;
		real gapResidual = 0.0f;
		real dashedAndGap = dashLength + gapLength;
		for (size_t i = 1; i < points.size(); ++i)
		{
			Vector2 p1 = points[i - 1];
			Vector2 p2 = points[i];
			Vector2 direction = p2 - p1;
			real length = direction.length();
			if (realEqual(length, 0.0f))
				continue;

			direction /= length;

			Vector2 start = p1;

			if (dashResidual > 0.0f)
			{
				if (dashResidual > length)
				{
					dashResidual -= length;
					line(p1, p2, color);
					continue;
				}

				Vector2 end = start + direction * dashResidual;
				line(start, end, color);
				length -= dashResidual;
				dashResidual = 0.0f;

				if (length > gapLength)
				{
					length -= gapLength;
					start = end + direction * gapLength;
				}
				else
				{
					gapResidual = gapLength - length;
					continue;
				}
			}

			if (gapResidual > 0.0f)
			{
				if (gapResidual > length)
				{
					gapResidual -= length;
					continue;
				}
				start += direction * gapResidual;
				length -= gapResidual;
				gapResidual = 0.0f;
			}


			bool finished = false;

			while (!finished)
			{
				if (length < dashLength)
				{
					line(start, p2, color);
					dashResidual = dashLength - length;
					gapResidual = 0.0f;
					finished = true;
				}
				else if (length >= dashLength && length < dashedAndGap)
				{
					Vector2 end = start + direction * dashLength;
					line(start, end, color);
					dashResidual = 0.0f;
					gapResidual = dashedAndGap - length;
					finished = true;
				}
				else
				{
					Vector2 end = start + direction * dashLength;
					line(start, end, color);
					start = end + direction * gapLength;
					dashResidual = 0.0f;
					gapResidual = 0.0f;
					length -= dashedAndGap;
				}

			}
		}
	}

	void Renderer2D::polyThickLine(const std::vector<Vector2>& points, const Color& color, float thickness)
	{
		if (points.size() < 2)
			return;

		PolyLines lines;
		for (auto&& elem : points)
		{
			pushVector(lines.vertices, elem);
			pushColor(lines.vertices, color);
		}
		lines.thickness = thickness;
		m_polyLines.emplace_back(lines);
	}

	void Renderer2D::polyDashedThickLine(const std::vector<Vector2>& points, const Color& color, float thickness,
		float dashLength, float gapLength)
	{
		if (points.size() < 2)
			return;

		real dashResidual = 0.0f;
		real gapResidual = 0.0f;
		real dashedAndGap = dashLength + gapLength;
		for (size_t i = 1; i < points.size(); ++i)
		{
			Vector2 p1 = points[i - 1];
			Vector2 p2 = points[i];
			Vector2 direction = p2 - p1;
			real length = direction.length();
			if (realEqual(length, 0.0f))
				continue;

			direction /= length;

			Vector2 start = p1;

			if (dashResidual > 0.0f)
			{
				if (dashResidual > length)
				{
					dashResidual -= length;
					thickLine(p1, p2, color, thickness);
					continue;
				}

				Vector2 end = start + direction * dashResidual;
				thickLine(start, end, color, thickness);
				length -= dashResidual;
				dashResidual = 0.0f;

				if (length > gapLength)
				{
					length -= gapLength;
					start = end + direction * gapLength;
				}
				else
				{
					gapResidual = gapLength - length;
					continue;
				}
			}

			if (gapResidual > 0.0f)
			{
				if (gapResidual > length)
				{
					gapResidual -= length;
					continue;
				}
				start += direction * gapResidual;
				length -= gapResidual;
				gapResidual = 0.0f;
			}


			bool finished = false;

			while (!finished)
			{
				if (length < dashLength)
				{
					thickLine(start, p2, color, thickness);
					dashResidual = dashLength - length;
					gapResidual = 0.0f;
					finished = true;
				}
				else if (length >= dashLength && length < dashedAndGap)
				{
					Vector2 end = start + direction * dashLength;
					thickLine(start, end, color, thickness);
					dashResidual = 0.0f;
					gapResidual = dashedAndGap - length;
					finished = true;
				}
				else
				{
					Vector2 end = start + direction * dashLength;
					thickLine(start, end, color, thickness);
					start = end + direction * gapLength;
					dashResidual = 0.0f;
					gapResidual = 0.0f;
					length -= dashedAndGap;
				}

			}
		}
	}

	void Renderer2D::polyClosedThickLines(const std::vector<Vector2>& points, const Color& color, float thickness)
	{
		if (points.size() < 2)
			return;

		PolyLines lines;
		for (auto&& elem : points)
		{
			pushVector(lines.vertices, elem);
			pushColor(lines.vertices, color);
		}
		lines.closed = true;
		lines.thickness = thickness;
		m_polyLines.emplace_back(lines);
	}

	void Renderer2D::shape(const Transform& transform, Shape* shape, const Color& color)
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

	void Renderer2D::polygon(const Transform& transform, Shape* shape, const Color& color)
	{
		assert(shape != nullptr && shape->type() == ShapeType::Polygon);
		auto polygon = static_cast<ST::Polygon*>(shape);
		std::vector<Vector2> vertices;
		vertices.reserve(polygon->vertices().size());
		for (auto&& elem : polygon->vertices())
			vertices.push_back(transform.translatePoint(elem));
		Color fillColor = color;
		fillColor.a = 38.0f / 255.0f;
		fillAndStroke(vertices, fillColor, color);
	}

	void Renderer2D::edge(const Transform& transform, Shape* shape, const Color& color)
	{
		assert(shape != nullptr && shape->type() == ShapeType::Edge);
		auto edge = static_cast<Edge*>(shape);
		Vector2 p1 = edge->startPoint() + transform.position;
		Vector2 p2 = edge->endPoint() + transform.position;
		Vector2 center = (edge->startPoint() + edge->endPoint()) / 2.0f;
		center += transform.position;

		point(p1, color);
		point(p2, color);
		line(p1, p2, color);
		line(center, center + 0.1f * edge->normal(), DarkPalette::Yellow);
	}

	void Renderer2D::circle(const Transform& transform, Shape* shape, const Color& color)
	{
		assert(shape != nullptr && shape->type() == ShapeType::Circle);
		const Circle* circle = static_cast<Circle*>(shape);
		std::vector<Vector2> vertices;
		int pointCount = 60 + static_cast<int>(meterToPixel());
		vertices.reserve(pointCount);
		for (int i = 0; i < pointCount; ++i)
		{
			real radian = Constant::DoublePi * static_cast<float>(i) / static_cast<float>(pointCount);
			Complex rot(radian);
			Vector2 point = rot.multiply(Vector2(1.0f, 0.0f)) * circle->radius();
			vertices.emplace_back(transform.translatePoint(point));
		}
		Color fillColor = color;
		fillColor.a = 38.0f / 255.0f;
		fillAndStroke(vertices, fillColor, color);
	}

	void Renderer2D::capsule(const Transform& transform, Shape* shape, const Color& color)
	{
		assert(shape != nullptr && shape->type() == ShapeType::Capsule);
		const Capsule* capsule = static_cast<Capsule*>(shape);
		std::vector<Vector2> vertices;
		int pointCount = 60 + static_cast<int>(meterToPixel());
		vertices.reserve(pointCount);
		pointCount /= 2;
		float halfWidth = capsule->halfWidth();
		float halfHeight = capsule->halfHeight();
		bool isHorizontal = halfWidth > halfHeight;
		float radius = halfWidth > halfHeight ? halfHeight : halfWidth;

		auto sampling = [&](const Vector2& center, const real& startRadians, const real& endRadians)
			{
				real step = (endRadians - startRadians) / static_cast<float>(pointCount);
				for (real radian = startRadians; radian <= endRadians; radian += step)
				{
					Vector2 point(radius * Math::cosx(radian), radius * Math::sinx(radian));
					point += center;
					const Vector2 worldPos = Complex(transform.rotation).multiply(point) + transform.position;
					vertices.emplace_back(worldPos);
				}
			};
		if (isHorizontal)
		{
			sampling((capsule->bottomLeft() + capsule->topLeft()) / 2.0f, Math::radians(90),
				Math::radians(270));
			sampling((capsule->topRight() + capsule->bottomRight()) / 2.0f, Math::radians(270),
				Math::radians(450));
		}
		else
		{
			sampling((capsule->topLeft() + capsule->topRight()) / 2.0f, Math::radians(0),
				Math::radians(180));
			sampling((capsule->bottomLeft() + capsule->bottomRight()) / 2.0f, Math::radians(180),
				Math::radians(360));
		}

		Color fillColor = color;
		fillColor.a = 38.0f / 255.0f;
		fillAndStroke(vertices, fillColor, color);
	}

	void Renderer2D::ellipse(const Transform& transform, Shape* shape, const Color& color)
	{
		assert(shape != nullptr && shape->type() == ShapeType::Ellipse);
		const ST::Ellipse* ellipse = static_cast<ST::Ellipse*>(shape);

		std::vector<Vector2> vertices;
		int pointCount = 60 + static_cast<int>(meterToPixel());
		vertices.reserve(pointCount);

		real step = Constant::DoublePi / static_cast<float>(pointCount);
		real innerRadius = ellipse->A();
		real outerRadius = ellipse->B();
		if (ellipse->A() > ellipse->B())
		{
			innerRadius = ellipse->B();
			outerRadius = ellipse->A();
		}
		for (real radian = 0; radian <= Constant::DoublePi; radian += step)
		{
			Vector2 point(outerRadius * Math::cosx(radian), innerRadius * Math::sinx(radian));
			const Vector2 worldPos = transform.translatePoint(point);
			vertices.emplace_back(worldPos);
		}

		Color fillColor = color;
		fillColor.a = 38.0f / 255.0f;
		fillAndStroke(vertices, fillColor, color);
	}

	void Renderer2D::orientation(const Transform& transform)
	{
		Vector2 xP(0.15f, 0);
		Vector2 yP(0, 0.15f);
		xP = transform.translatePoint(xP);
		yP = transform.translatePoint(yP);
		line(transform.position, xP, DarkPalette::Red);
		line(transform.position, yP, DarkPalette::Green);
	}

	void Renderer2D::aabb(const AABB& aabb, const Color& color)
	{
		closedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft() }, color);
	}

	void Renderer2D::dashedAABB(const AABB& aabb, const Color& color, float dashLength, float gapLength)
	{
		polyDashedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft(),aabb.topLeft() }, color, dashLength, gapLength);
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

	void Renderer2D::simplex(const Simplex& simplex, const Color& color, bool showIndex,
		const unsigned int& fontSize)
	{
		Color lineColor = color;
		lineColor.a = 150.0f / 255.0f;

		switch (simplex.count)
		{
		case 0:
			break;
		case 1:
			point(simplex.vertices[0].result, color);
			if (showIndex)
				text(simplex.vertices[0].result, color, 0, fontSize);
			break;
		case 2:
			point(simplex.vertices[0].result, color);
			point(simplex.vertices[1].result, color);
			line(simplex.vertices[0].result, simplex.vertices[1].result, lineColor);

			if (showIndex)
			{
				Vector2 offset = simplex.vertices[1].result - simplex.vertices[0].result;
				offset = -offset.perpendicular().normal() * 0.3f;
				text(simplex.vertices[0].result, color, 0, fontSize, offset);
				text(simplex.vertices[1].result, color, 1, fontSize, offset);
			}
			break;
		case 3:
			closedLines({ simplex.vertices[0].result, simplex.vertices[1].result, simplex.vertices[2].result }, lineColor);
			point(simplex.vertices[0].result, color);
			point(simplex.vertices[1].result, color);
			point(simplex.vertices[2].result, color);

			if (showIndex)
			{
				Vector2 center = (simplex.vertices[0].result + simplex.vertices[1].result + simplex.vertices[2].result)
					/ 3.0f;

				Vector2 offset = simplex.vertices[0].result - center;
				offset = offset.normal() * 0.3f;
				text(simplex.vertices[0].result, color, 0, fontSize, offset);
				
				offset = simplex.vertices[1].result - center;
				offset = offset.normal() * 0.3f;
				text(simplex.vertices[1].result, color, 1, fontSize, offset);
				
				offset = simplex.vertices[2].result - center;
				offset = offset.normal() * 0.3f;
				text(simplex.vertices[2].result, color, 2, fontSize, offset);
				
			}
			break;
		default:
			assert(false && "Simplex count must be less than 3");
			break;
		}
	}

	void Renderer2D::polytope(const std::vector<Vector2>& points, const Color& color, float pointSize, const unsigned int& indexSize, bool showIndex)
	{

		Vector2 center = GeometryAlgorithm2D::computeCenter(points);
		for (int i = 0; i < points.size(); ++i)
		{
			int j = (i + 1) % points.size();
			Vector2 offset = (points[i] - center).normal() * 0.3f;

			point(points[i], color, pointSize);
			point(points[j], color, pointSize);
			line(points[i], points[j], color);

			if (showIndex)
				text(points[i], color, i, indexSize, offset);
		}
	}

	Vector2 Renderer2D::worldToScreen(const Vector2& worldPos) const
	{
		glm::vec4 pos = m_projection * m_view * m_model * glm::vec4(worldPos.x, worldPos.y, 0.0f, 1.0f);
		float ndcX = pos.x / pos.w;
		float ndcY = pos.y / pos.w;

		float x = (ndcX + 1.0f) * 0.5f * static_cast<float>(m_frameBufferWidth);
		float y = (1.0f - ndcY) * 0.5f * static_cast<float>(m_frameBufferHeight);

		x = std::floor(x);
		y = std::floor(y);

		return { x, y };
	}

	Vector2 Renderer2D::screenToWorld(const Vector2& screenPos) const
	{
		float ndcX = (2.0f * static_cast<float>(screenPos.x) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY = 1.0f - (2.0f * static_cast<float>(screenPos.y) / static_cast<float>(m_frameBufferHeight));
		float ndcZ = 0.0f;

		glm::vec4 pos = glm::inverse(m_projection * m_view * m_model) * glm::vec4(ndcX, ndcY, ndcZ, 1.0f);
		pos /= pos.w;

		return { pos.x, pos.y };
	}

	void Renderer2D::polyLines(const std::vector<Vector2>& points, const Color& color)
	{
		if (points.size() < 2)
			return;

		PolyLines lines;
		for(auto&& elem: points)
		{
			pushVector(lines.vertices, elem);
			pushColor(lines.vertices, color);
		}
		m_polyLines.emplace_back(lines);
	}

	void Renderer2D::closedLines(const std::vector<Vector2>& points, const Color& color)
	{
		if (points.size() < 2)
			return;

		PolyLines lines;
		for (auto&& elem : points)
		{
			pushVector(lines.vertices, elem);
			pushColor(lines.vertices, color);
		}
		lines.closed = true;
		m_polyLines.emplace_back(lines);
	}

	void Renderer2D::onFrameBufferResize(int width, int height)
	{
		m_frameBufferWidth = width;
		m_frameBufferHeight = height;

		glViewport(0, 0, width, height);
	}

	void Renderer2D::onKeyButton(int key, int scancode, int action, int mods)
	{

	}

	void Renderer2D::onMouseButton(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (action == GLFW_PRESS)
			{
				m_isTranslateView = true;
			}
			else if (action == GLFW_RELEASE)
			{
				m_isTranslateView = false;
				m_translationStart = false;
				m_translationStartPos = m_cameraPosition;
				m_mouseStart.clear();
			}
		}
	}

	void Renderer2D::onMouseMoved(double xpos, double ypos)
	{
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);

		if (m_isTranslateView)
			onTranslateView(x, y);
	}

	void Renderer2D::onMouseScroll(double xoffset, double yoffset)
	{
		onZoomView(static_cast<float>(xoffset), static_cast<float>(yoffset));
	}

	float Renderer2D::meterToPixel() const
	{
		return m_meterToPixel;
	}

	float Renderer2D::pixelToMeter() const
	{
		return 1.0f / m_meterToPixel;
	}


	void Renderer2D::onZoomView(float xoffset, float yoffset)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		m_scrollMouseStart = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

		m_meterToPixel *= 1.0f + yoffset * m_scaleRatio;
		m_meterToPixel = std::clamp(m_meterToPixel, 0.1f, 5000.0f);

		m_easingMeterToPixel.continueTo(m_meterToPixel, 0.5f);

		if (!m_smoothZooming)
		{
			m_easingMeterToPixel.finish();

			buildMVPMatrix();

			Vector2 after = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

			Vector2 scrollDelta = m_scrollMouseStart - after;

			m_cameraPosition += glm::vec3(scrollDelta.x, scrollDelta.y, 0.0f);

			m_scrollMouseStart.clear();
		}
	}

	void Renderer2D::onTranslateView(float x, float y)
	{
		if (!m_translationStart)
		{
			m_mouseStart = screenToWorld({ x, y });
			m_translationStart = true;
			m_translationStartPos = m_cameraPosition;
			return;
		}

		Vector2 current = screenToWorld({ x, y });
		float dx = current.x - m_mouseStart.x;
		float dy = current.y - m_mouseStart.y;

		dx *= -m_translateSensitivity;
		dy *= -m_translateSensitivity;

		glm::vec3 delta = glm::vec3(dx, dy, 0.0f);
		glm::vec3 pos = m_translationStartPos + delta;

		m_cameraPosition = pos;

	}

	void Renderer2D::buildMVPMatrix()
	{
		m_view = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);

		m_orthoSize = 0.25f * (m_zFar - m_zNear) / m_zFar * (static_cast<float>(m_frameBufferWidth) / (m_aspectRatio * m_easingMeterToPixel.value()));

		float h = m_orthoSize;
		float w = m_aspectRatio * h;

		m_projection = glm::ortho(-w, w, -h, h, m_zNear, m_zFar);
	}

	void Renderer2D::pushVector(std::vector<float>& lines, const Vector2& vec)
	{
		lines.push_back(vec.x);
		lines.push_back(vec.y);
		lines.push_back(0.0f);
	}

	void Renderer2D::pushColor(std::vector<float>& lines, const Color& color)
	{
		lines.push_back(color.r);
		lines.push_back(color.g);
		lines.push_back(color.b);
		lines.push_back(color.a);
	}
}
