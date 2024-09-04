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

		m_model = glm::mat4(1.0f);
		buildMVPMatrix();
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
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	}

	void Renderer2D::polyClosedThickLines(const std::vector<Vector2>& points, const Color& color, float thickness)
	{
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
	}

	void Renderer2D::edge(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::rectangle(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::circle(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::capsule(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::ellipse(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::orientation(const Transform& transform)
	{
	}

	void Renderer2D::aabb(const AABB& aabb, const Color& color)
	{
		closedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft() }, color);
	}

	void Renderer2D::dashedAABB(const AABB& aabb, const Color& color, float dashLength, float gapLength)
	{
		polyDashedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft(),aabb.topLeft() }, color, dashLength, gapLength);
	}

	void Renderer2D::dashedShape(const Transform& transform, Shape* shape, const Color& color)
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

	void Renderer2D::dashedPolygon(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::dashedEdge(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::dashedRectangle(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::dashedCircle(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::dashedCapsule(const Transform& transform, Shape* shape, const Color& color)
	{
	}

	void Renderer2D::dashedEllipse(const Transform& transform, Shape* shape, const Color& color)
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

	void Renderer2D::simplex(const Simplex& simplex, const Color& color, bool showIndex,
		const unsigned int& fontSize)
	{
	}

	void Renderer2D::polytope(const std::vector<Vector2>& points, const Color& color, float pointSize, const unsigned int& indexSize, bool showIndex)
	{

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

	void Renderer2D::onRender()
	{
		buildMVPMatrix();

		drawGridScaleLines();

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
		if(!m_lines.empty())
			glDrawArrays(GL_LINES, 0, lineSize);
		

		if (!m_thickLines.empty())
		{
			for(auto&& elem: m_thickLines)
			{
				glLineWidth(elem.thickness);
				glDrawArrays(GL_LINES, offset, elem.vertices.size() / 7);
				offset += elem.vertices.size() / 7;
			}
		}

		if(!m_polyLines.empty())
		{
			for(auto&& elem: m_polyLines)
			{
				glLineWidth(elem.thickness);
				if (elem.closed)
					glDrawArrays(GL_LINE_LOOP, offset, elem.vertices.size() / 7);
				else
					glDrawArrays(GL_LINE_STRIP, offset, elem.vertices.size() / 7);
				offset += elem.vertices.size() / 7;
			}
		}

		if(!m_points.empty())
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

		if(!m_ndcPoints.empty())
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

	void Renderer2D::onFrameBufferResize(GLFWwindow* window, int width, int height)
	{
		m_frameBufferWidth = width;
		m_frameBufferHeight = height;

		glViewport(0, 0, width, height);
	}

	void Renderer2D::onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

	}

	void Renderer2D::onMouseButton(GLFWwindow* window, int button, int action, int mods)
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

	void Renderer2D::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
	{
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);

		if (m_isTranslateView)
			onTranslateView(window, x, y);
	}

	void Renderer2D::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_scrollMouseStart = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

		float y = static_cast<float>(yoffset);
		onScale(window, y);
	}

	void Renderer2D::drawGridScaleLines()
	{
		Color thin = DarkPalette::DarkGreen;
		for (int i = -10; i <= 10; ++i)
		{
			if (i == 0)
				thin = DarkPalette::Green;
			else
			{
				thin = DarkPalette::DarkGreen;
				thin.a = 100.0f / 255.0f;
			}

			Vector2 start = { -10.0f, static_cast<float>(i) };
			Vector2 end = { 10.0f, static_cast<float>(i) };
			line(start, end, thin);
			start = { static_cast<float>(i), -10.0f };
			end = { static_cast<float>(i), 10.0f };
			line(start, end, thin);
		}
	}

	void Renderer2D::onScale(GLFWwindow* window, float yOffset)
	{
		m_meterToPixel *= 1.0f + yOffset * m_scaleRatio;
		m_meterToPixel = std::clamp(m_meterToPixel, 0.1f, 5000.0f);

		m_orthoSize = 0.25f * (m_zFar - m_zNear) / m_zFar * (static_cast<float>(m_frameBufferWidth) / (m_aspectRatio * m_meterToPixel));

		//m_orthoSize *= 1.0f - yOffset * m_orthoSizeScaleRatio;


		//m_orthoSize -= yOffset * m_orthoSizeScaleRatio;
		//m_orthoSize = std::clamp(m_orthoSize, 0.1f, 100.0f);

		float h = m_orthoSize;
		float w = m_aspectRatio * h;

		m_projection = glm::ortho(-w, w, -h, h, m_zNear, m_zFar);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Vector2 after = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

		Vector2 scrollDelta = m_scrollMouseStart - after;

		m_cameraPosition += glm::vec3(scrollDelta.x, scrollDelta.y, 0.0f);

		m_scrollMouseStart.clear();
	}

	void Renderer2D::onTranslateView(GLFWwindow* window, float x, float y)
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

		m_orthoSize = 0.25f * (m_zFar - m_zNear) / m_zFar * (static_cast<float>(m_frameBufferWidth) / (m_aspectRatio * m_meterToPixel));

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
