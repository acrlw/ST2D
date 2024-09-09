#include "Renderer2D.h"

namespace STEditor
{

	Renderer2D::Renderer2D(GLFWwindow* window) : m_window(window)
	{
		initShaders();

		initRenderSettings();

		initFont();

		buildViewProjectionMatrix();

		m_easingMeterToPixel.setEasingFunction(EasingFunction::easeOutExpo);

		updateScreenAABB();
	}

	Renderer2D::~Renderer2D()
	{
		APP_INFO("Delete VAO ({},{},{}) and VBO ({},{},{})", m_graphicsVAO, m_pointVAO, m_fontVAO, m_graphicsVBO, m_pointVBO, m_fontVBO);

		glDeleteVertexArrays(1, &m_graphicsVAO);
		glDeleteBuffers(1, &m_graphicsVBO);

		glDeleteVertexArrays(1, &m_pointVAO);
		glDeleteBuffers(1, &m_pointVBO);

		glDeleteVertexArrays(1, &m_fontVAO);
		glDeleteBuffers(1, &m_fontVBO);

		for(auto& value : m_characters | std::views::values)
			glDeleteTextures(1, &value.textureID);

		m_graphicsProgram.destroy();
		m_pointProgram.destroy();
		m_fontProgram.destroy();

		FT_Done_Face(m_ftFace);
		FT_Done_FreeType(m_ftLibrary);
	}

	void Renderer2D::initShaders()
	{
		ZoneScopedN("[Renderer2D] Init Shaders");

		m_graphicsProgram.addVertexShader("./Resource/Shaders/vert-graphics.glsl");
		m_graphicsProgram.addFragmentShader("./Resource/Shaders/frag-graphics.glsl");
		m_graphicsProgram.compileShader();
		m_graphicsProgram.link();

		m_pointProgram.addVertexShader("./Resource/Shaders/vert-round-point.glsl");
		m_pointProgram.addFragmentShader("./Resource/Shaders/frag-round-point.glsl");
		m_pointProgram.compileShader();
		m_pointProgram.link();

		m_fontProgram.addVertexShader("./Resource/Shaders/vert-font.glsl");
		m_fontProgram.addFragmentShader("./Resource/Shaders/frag-font.glsl");
		m_fontProgram.compileShader();
		m_fontProgram.link();
	}

	void Renderer2D::initRenderSettings()
	{
		ZoneScopedN("[Renderer2D] Init Render Settings");

		m_graphicsProgram.use();

		glGenVertexArrays(1, &m_graphicsVAO);
		glBindVertexArray(m_graphicsVAO);

		glGenBuffers(1, &m_graphicsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_graphicsVBO);

		glBufferData(GL_ARRAY_BUFFER, m_graphicsDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		m_pointProgram.use();

		glGenVertexArrays(1, &m_pointVAO);
		glBindVertexArray(m_pointVAO);

		glGenBuffers(1, &m_pointVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_pointVBO);
		glBufferData(GL_ARRAY_BUFFER, m_pointDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
		glEnableVertexAttribArray(2);

		m_fontProgram.use();

		glGenVertexArrays(1, &m_fontVAO);
		glBindVertexArray(m_fontVAO);

		glGenBuffers(1, &m_fontVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
		glBufferData(GL_ARRAY_BUFFER, m_textDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		APP_INFO("Created VAO({},{},{}) and VBO ({},{},{})", m_graphicsVAO, m_pointVAO, m_fontVAO, m_graphicsVBO, m_pointVBO, m_fontVBO);
	}

	void Renderer2D::initFont()
	{
		ZoneScopedN("[Renderer2D] Init Font");

		if(FT_Init_FreeType(&m_ftLibrary))
		{
			APP_ERROR("[FreeType] Failed to init FreeType library");
		}

		if(FT_New_Face(m_ftLibrary, "Resource/Fonts/MiSans-Medium.ttf", 0, &m_ftFace))
		{
			APP_ERROR("[FreeType] Failed to load font");
		}

		FT_Set_Pixel_Sizes(m_ftFace, 0, m_fontHeight);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (GLubyte c = 0; c < 128; c++)
		{

			if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER))
			{
				APP_ERROR("[FreeType] Failed to load Glyph");
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				m_ftFace->glyph->bitmap.width,
				m_ftFace->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				m_ftFace->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			Char character = {
				texture,
				glm::ivec2(m_ftFace->glyph->bitmap.width, m_ftFace->glyph->bitmap.rows),
				glm::ivec2(m_ftFace->glyph->bitmap_left, m_ftFace->glyph->bitmap_top),
				m_ftFace->glyph->advance.x
			};
			m_characters.insert(std::pair<GLchar, Char>(c, character));
		}

	}

	void Renderer2D::onRenderStart()
	{
		ZoneScopedN("[Renderer2D] On Render Start");

		std::vector<float> vertices = m_lines;

		for (auto&& elem : m_thickLines)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for (auto&& elem : m_polyLines)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for(auto&& elem: m_fills)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for (auto&& elem : m_fillStrokes)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		for(auto&& elem: m_multiCommandsDraws)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		vertices.insert(vertices.end(), m_ndcLines.begin(), m_ndcLines.end());


		for (auto&& elem : m_ndcMultiCommandsDraws)
			vertices.insert(vertices.end(), elem.vertices.begin(), elem.vertices.end());

		m_graphicsProgram.use();

		glBindVertexArray(m_graphicsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_graphicsVBO);
		m_graphicsDataSize = vertices.size();
		if (m_graphicsDataSize > m_graphicsDataCapacity)
		{
			m_graphicsDataCapacity = m_graphicsDataSize * 2;
			glBufferData(GL_ARRAY_BUFFER, m_graphicsDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_graphicsDataSize * sizeof(float), vertices.data());

		vertices.clear();
		vertices.insert(vertices.end(), m_points.begin(), m_points.end());
		vertices.insert(vertices.end(), m_ndcPoints.begin(), m_ndcPoints.end());

		m_pointProgram.use();

		glBindVertexArray(m_pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_pointVBO);
		m_pointDataSize = vertices.size();
		if (m_pointDataSize > m_pointDataCapacity)
		{
			m_pointDataCapacity = m_pointDataSize * 2;
			glBufferData(GL_ARRAY_BUFFER, m_pointDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_pointDataSize * sizeof(float), vertices.data());

		vertices.clear();
		vertices.insert(vertices.end(), m_text.begin(), m_text.end());

		m_fontProgram.use();

		glBindVertexArray(m_fontVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);

		m_textDataSize = vertices.size();
		if (m_textDataSize > m_textDataCapacity)
		{
			m_textDataCapacity = m_textDataSize * 2;
			glBufferData(GL_ARRAY_BUFFER, m_textDataCapacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_textDataSize * sizeof(float), vertices.data());

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Renderer2D::onRender()
	{
		ZoneScopedN("[Renderer2D] On Render");

		GLint previousDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
		glDepthFunc(GL_ALWAYS);

		buildViewProjectionMatrix();

		onRenderStart();

		drawGraphicsProgram();
		drawPointsProgram();
		drawTextProgram();

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
			m_orthoSize = 0.25f * (m_zFar - m_zNear) / m_zFar * (static_cast<float>(m_frameBufferWidth) / (m_aspectRatio * m_easingMeterToPixel.value()));
			m_pixelToMeter = 1.0f / m_easingMeterToPixel.value();

			float h = m_orthoSize;
			float w = m_aspectRatio * h;

			m_projection = glm::ortho(-w, w, -h, h, m_zNear, m_zFar);

			Vector2 currentMouse = screenToWorld(m_scrollMouseScreenStart);

			Vector2 delta = m_scrollMouseStart - currentMouse;
			m_cameraPosition += glm::vec3(delta.x, delta.y, 0.0f);

			m_view = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);

			updateScreenAABB();
		}
	}

	void Renderer2D::onRenderEnd()
	{
		ZoneScopedN("[Renderer2D] On Render End");

		m_lines.clear();
		m_thickLines.clear();
		m_polyLines.clear();
		m_fills.clear();
		m_fillStrokes.clear();
		m_multiCommandsDraws.clear();
		m_ndcLines.clear();
		m_ndcMultiCommandsDraws.clear();

		m_points.clear();
		m_ndcPoints.clear();

		m_text.clear();
		m_textureIDs.clear();
		m_textColors.clear();

		//m_lines.shrink_to_fit();
		//m_thickLines.shrink_to_fit();
		//m_polyLines.shrink_to_fit();
		//m_fills.shrink_to_fit();
		//m_fillStrokes.shrink_to_fit();
		//m_multiCommandsDraws.shrink_to_fit();
		//m_ndcLines.shrink_to_fit();
		//m_ndcMultiCommandsDraws.shrink_to_fit();

		//m_points.shrink_to_fit();
		//m_ndcPoints.shrink_to_fit();

		//m_text.shrink_to_fit();
		//m_textureIDs.shrink_to_fit();
		//m_textColors.shrink_to_fit();
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, const Color& color)
	{
		Vector2 p1 = screenToNDC({ static_cast<float>(x1), static_cast<float>(y1) });
		Vector2 p2 = screenToNDC({ static_cast<float>(x2), static_cast<float>(y2) });

		float minX = p1.x;
		float maxX = p2.x;
		float minY = p1.y;
		float maxY = p2.y;

		if (minX > maxX)
			std::swap(minX, maxX);
		if (minY > maxY)
			std::swap(minY, maxY);

		bool xOut = maxX < -1.0f || minX > 1.0f;
		bool yOut = maxY < -1.0f || minY > 1.0f;

		bool collide = !(xOut || yOut);
		if (!collide)
			return;

		pushVector(m_ndcLines, p1);
		pushColor(m_ndcLines, color);
		pushVector(m_ndcLines, p2);
		pushColor(m_ndcLines, color);
	}

	void Renderer2D::line(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
	{
		Vector2 p1 = screenToNDC({ static_cast<float>(x1), static_cast<float>(y1) });
		Vector2 p2 = screenToNDC({ static_cast<float>(x2), static_cast<float>(y2) });

		float minX = p1.x;
		float maxX = p2.x;
		float minY = p1.y;
		float maxY = p2.y;

		if (minX > maxX)
			std::swap(minX, maxX);
		if (minY > maxY)
			std::swap(minY, maxY);

		bool xOut = maxX < -1.0f || minX > 1.0f;
		bool yOut = maxY < -1.0f || minY > 1.0f;

		bool collide = !(xOut || yOut);
		if (!collide)
			return;

		Color color(r, g, b, a);


		pushVector(m_ndcLines, p1);
		pushColor(m_ndcLines, color);
		pushVector(m_ndcLines, p2);
		pushColor(m_ndcLines, color);
	}

	void Renderer2D::point(int x, int y, const Color& color, float size)
	{
		Vector2 p = screenToNDC({ static_cast<float>(x), static_cast<float>(y) });

		if (!(Math::isInRange(p.x, -1.0f, 1.0f) &&
			Math::isInRange(p.y, -1.0f, 1.0f)))
			return;

		pushVector(m_ndcPoints, p);
		pushColor(m_ndcPoints, color);
		m_ndcPoints.push_back(size);
	}

	void Renderer2D::point(const Vector2& position, const Color& color, float size)
	{
		AABB aabb = m_screenAABB;
		aabb.expand(2.0f * size);

		if (!AABB::collide(aabb, position))
			return;

		pushVector(m_points, position);
		pushColor(m_points, color);
		m_points.push_back(size);
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
		if (!AABB::collide(m_screenAABB, start, end))
			return;

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

		AABB aabb = AABB::fromShape(transform, shape);

		if (!aabb.collide(m_screenAABB))
			return;

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
		line(transform.position, yP, DarkPalette::Blue);
	}

	void Renderer2D::aabb(const AABB& aabb, const Color& color)
	{
		closedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft() }, color);
	}

	void Renderer2D::dashedAABB(const AABB& aabb, const Color& color, float dashLength, float gapLength)
	{
		polyDashedLines({ aabb.topLeft(), aabb.topRight(), aabb.bottomRight(), aabb.bottomLeft(),aabb.topLeft() }, color, dashLength, gapLength);
	}

	void Renderer2D::arrow(const Vector2& start, const Vector2& end, const Color& color, const float& size, const float& degree)
	{
		Vector2 tf = start - end;
		real length = tf.length();
		real scale = size;
		if (length < 1.0f)
			scale = length * size;
		if (realEqual(length, 0))
			return;

		Vector2 n = tf.normal();
		Vector2 normal = n * scale;
		Complex c(Math::radians(degree * 0.5f));
		Vector2 d1 = c.multiply(normal);
		Vector2 d2 = c.set(-Math::radians(degree * 0.5f)).multiply(normal);

		Vector2 p1 = d1 + end;
		Vector2 p2 = d2 + end;
		real p = d1.dot(n);
		real l = (d1 - p * n).length();
		real x = l / Math::tanx(Math::radians(degree));
		real f = p - x;
		Vector2 p3 = f * n + end;

		fill({ end, p1, p3, p2 }, color);
		line(start, p3, color);
	}

	void Renderer2D::text(const Vector2& position, const Color& color, const std::string& text, const float& scale,
		bool centered)
	{
		if (text.empty())
			return;

		Vector2 screenPos = worldToScreen(position);
		screenPos.y = m_frameBufferHeight - screenPos.y;

		float x = screenPos.x;
		float y = screenPos.y;

		if(centered)
		{
			float tx = x;
			float textWidth = 0.0f;
			float minHeight = m_fontHeight;
			float maxHeight = 0.0f;
			float firstCharWidth = 0.0f;

			for (auto iter = text.begin(); iter != text.end(); ++iter)
			{
				Char ch = m_characters[*iter];

				float xpos = tx + ch.bearing.x * scale;

				float w = ch.size.x * scale;

				char c = *iter;

				if (c != '.' && c != ',' && c != ' ' && c != '+' && c != '-' && c != '*')
					minHeight = Math::min(minHeight, ch.size.y * scale);

				maxHeight = Math::max(maxHeight, ch.size.y * scale);

				if (iter == text.begin())
				{
					textWidth = xpos;
					firstCharWidth = ch.size.x * scale;
				}
				else if (iter == text.end() - 1)
				{
					textWidth = xpos + w - textWidth;
				}

				tx += (ch.advance >> 6) * scale;
			}

			if(text.size() == 1)
				textWidth = firstCharWidth;

			textWidth *= 0.5f;

			if (minHeight == m_fontHeight)
				minHeight = maxHeight;

			minHeight *= 0.5f;

			if (x < -textWidth || x > m_frameBufferWidth + textWidth || y < -minHeight || y > m_frameBufferHeight + minHeight)
				return;

			for (auto&& elem : text)
			{
				Char ch = m_characters[elem];

				float xpos = x + ch.bearing.x * scale;
				float ypos = y - (ch.size.y - ch.bearing.y) * scale;

				xpos -= textWidth;
				ypos -= minHeight;

				int px = xpos;
				int py = m_frameBufferHeight - ypos;

				float w = ch.size.x * scale;
				float h = ch.size.y * scale;

				std::array vertices =
				{
					xpos, ypos + h, 0.0f, 0.0f,
					xpos, ypos, 0.0f, 1.0f,
					xpos + w, ypos, 1.0f, 1.0f,

					xpos, ypos + h, 0.0f, 0.0f,
					xpos + w, ypos, 1.0f, 1.0f,
					xpos + w, ypos + h, 1.0f, 0.0f
				};

				m_text.insert(m_text.end(), vertices.begin(), vertices.end());

				m_textureIDs.push_back(ch.textureID);

				m_textColors.push_back(color);

				x += (ch.advance >> 6) * scale;
			}
		}
		else
		{
			float maxHeight = 0.0f;

			for (auto&& elem : text)
			{
				Char ch = m_characters[elem];

				if (ch.size.y - ch.bearing.y > 0)
					maxHeight = Math::max(maxHeight, ch.size.y * scale);
			}

			if (x < -m_fontHeight || x > m_frameBufferWidth + m_fontHeight || y < -maxHeight || y > m_frameBufferHeight + maxHeight)
				return;

			for (auto&& elem : text)
			{
				Char ch = m_characters[elem];

				float xpos = x + ch.bearing.x * scale;
				float ypos = y - (ch.size.y - ch.bearing.y) * scale;

				ypos -= maxHeight;

				float w = ch.size.x * scale;
				float h = ch.size.y * scale;

				std::array vertices =
				{
					xpos, ypos + h, 0.0f, 0.0f,
					xpos, ypos, 0.0f, 1.0f,
					xpos + w, ypos, 1.0f, 1.0f,
					xpos, ypos + h, 0.0f, 0.0f,
					xpos + w, ypos, 1.0f, 1.0f,
					xpos + w, ypos + h, 1.0f, 0.0f
				};

				m_text.insert(m_text.end(), vertices.begin(), vertices.end());

				m_textureIDs.push_back(ch.textureID);

				m_textColors.push_back(color);

				x += (ch.advance >> 6) * scale;
			}
		}
		


	}


	void Renderer2D::simplex(const Simplex& simplex, const Color& color, bool showIndex)
	{
		Color lineColor = color;
		lineColor.a = 150.0f / 255.0f;

		switch (simplex.count)
		{
		case 0:
			break;
		case 1:
			point(simplex.vertices[0].result, color);
			break;
		case 2:
			point(simplex.vertices[0].result, color);
			point(simplex.vertices[1].result, color);
			line(simplex.vertices[0].result, simplex.vertices[1].result, lineColor);

			if (showIndex)
			{
				Vector2 offset = simplex.vertices[1].result - simplex.vertices[0].result;
				offset = -offset.perpendicular().normal() * m_simplexIndexOffset * m_pixelToMeter;
				text(simplex.vertices[0].result + offset, color, "0");
				text(simplex.vertices[1].result + offset, color, "1");
			}
			break;
		case 3:
		{
			closedLines({ simplex.vertices[0].result, simplex.vertices[1].result, simplex.vertices[2].result }, lineColor);
			point(simplex.vertices[0].result, color);
			point(simplex.vertices[1].result, color);
			point(simplex.vertices[2].result, color);

			if (showIndex)
			{
				Vector2 center = 1.0f / 3.0f * (simplex.vertices[0].result + simplex.vertices[1].result + simplex.vertices[2].result);

				Vector2 offset = simplex.vertices[0].result - center;
				offset = offset.normal() * m_simplexIndexOffset * m_pixelToMeter;
				text(simplex.vertices[0].result + offset, color, "0");

				offset = simplex.vertices[1].result - center;
				offset = offset.normal() * m_simplexIndexOffset * m_pixelToMeter;
				text(simplex.vertices[1].result + offset, color, "1");

				offset = simplex.vertices[2].result - center;
				offset = offset.normal() * m_simplexIndexOffset * m_pixelToMeter;
				text(simplex.vertices[2].result + offset, color, "2");

			}
			break;
		}
		default:
			assert(false && "Simplex count must be less than 3");
			break;
		}
	}

	void Renderer2D::polytope(const std::vector<Vector2>& points, const Color& color, float pointSize, bool showIndex)
	{
		Vector2 center = GeometryAlgorithm2D::computeCenter(points);
		std::vector<Vector2> offsets;

		for (const auto& p : points)
		{
			point(p, color, pointSize);
			offsets.emplace_back((p - center).normal() * m_simplexIndexOffset * m_pixelToMeter);
		}
		closedLines(points, color);

		//draw text
		if(showIndex)
		{
			for (size_t i = 0; i < points.size(); ++i)
			{
				text(points[i] + offsets[i], color, std::to_string(i));
			}
		}
	}

	Vector2 Renderer2D::screenToNDC(const Vector2& pos) const
	{
		float ndcX = (2.0f * static_cast<float>(pos.x) / static_cast<float>(m_frameBufferWidth)) - 1.0f;
		float ndcY = 1.0f - (2.0f * static_cast<float>(pos.y) / static_cast<float>(m_frameBufferHeight));
		return { ndcX, ndcY };
	}

	Vector2 Renderer2D::ndcToScreen(const Vector2& pos) const
	{
		return {};
	}

	Vector2 Renderer2D::worldToScreen(const Vector2& worldPos) const
	{
		glm::vec4 pos = m_projection * m_view * glm::vec4(worldPos.x, worldPos.y, 0.0f, 1.0f);
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

		glm::vec4 pos = glm::inverse(m_projection * m_view) * glm::vec4(ndcX, ndcY, ndcZ, 1.0f);
		pos /= pos.w;

		return { pos.x, pos.y };
	}

	void Renderer2D::drawGraphicsProgram()
	{
		ZoneScopedN("[Renderer2D] On Draw Graphics Program");
		glm::mat4 identity = glm::mat4(1.0f);

		m_graphicsProgram.use();
		glBindVertexArray(m_graphicsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_graphicsVBO);

		m_graphicsProgram.setUniformMat4f("view", m_view);
		m_graphicsProgram.setUniformMat4f("projection", m_projection);

		size_t lineSize = m_lines.size() / 7;
		size_t offset = 0;
		if (!m_lines.empty())
		{
			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, offset, lineSize);
			offset += lineSize;
		}


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
				m_graphicsProgram.setUniform1i("isFillMode", 0);

				glLineWidth(elem.thickness);
				glDrawArrays(GL_LINE_LOOP, offset, elem.vertices.size() / 7);

				m_graphicsProgram.setUniform1i("isFillMode", 1);
				m_graphicsProgram.setUniform4f("fillColor", elem.fillColor.r, elem.fillColor.g, elem.fillColor.b, elem.fillColor.a);

				glDrawArrays(GL_TRIANGLE_FAN, offset, elem.vertices.size() / 7);

				offset += elem.vertices.size() / 7;
			}
			m_graphicsProgram.setUniform1i("isFillMode", 0);
		}

		if (!m_multiCommandsDraws.empty())
		{
			for (auto&& elem : m_multiCommandsDraws)
			{
				glLineWidth(elem.thickness);
				for (auto&& command : elem.commands)
				{
					if (command == GL_TRIANGLE_FAN)
					{
						m_graphicsProgram.setUniform1i("isFillMode", 1);
						m_graphicsProgram.setUniform4f("fillColor", elem.fillColor.r, elem.fillColor.g, elem.fillColor.b, elem.fillColor.a);

						glDrawArrays(GL_TRIANGLE_FAN, offset, elem.vertices.size() / 7);

						m_graphicsProgram.setUniform1i("isFillMode", 0);
					}
					else if (command == GL_POINTS)
					{
						glPointSize(elem.pointSize);
						glDrawArrays(GL_POINTS, offset, elem.vertices.size() / 7);
					}
					else
					{
						glLineWidth(elem.thickness);
						glDrawArrays(command, offset, elem.vertices.size() / 7);
					}
				}
				offset += elem.vertices.size() / 7;
			}
		}

		m_graphicsProgram.setUniformMat4f("view", identity);
		m_graphicsProgram.setUniformMat4f("projection", identity);

		if (!m_ndcLines.empty())
		{
			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, offset, m_ndcLines.size() / 7);
			offset += m_ndcLines.size() / 7;
		}

		if (!m_ndcMultiCommandsDraws.empty())
		{
			for (auto&& elem : m_ndcMultiCommandsDraws)
			{
				glLineWidth(elem.thickness);
				for (auto&& command : elem.commands)
				{
					if (command == GL_TRIANGLE_FAN)
					{
						m_graphicsProgram.setUniform1i("isFillMode", 1);
						m_graphicsProgram.setUniform4f("fillColor", elem.fillColor.r, elem.fillColor.g, elem.fillColor.b, elem.fillColor.a);

						glDrawArrays(GL_TRIANGLE_FAN, offset, elem.vertices.size() / 7);

						m_graphicsProgram.setUniform1i("isFillMode", 0);
					}
					else if (command == GL_POINTS)
					{
						glPointSize(elem.pointSize);
						glDrawArrays(GL_POINTS, offset, elem.vertices.size() / 7);
					}
					else
					{
						glLineWidth(elem.thickness);
						glDrawArrays(command, offset, elem.vertices.size() / 7);
					}
				}
				offset += elem.vertices.size() / 7;
			}
		}
	}

	void Renderer2D::drawPointsProgram()
	{
		ZoneScopedN("[Renderer2D] On Draw Points Program");

		glm::mat4 identity = glm::mat4(1.0f);

		m_pointProgram.use();

		glBindVertexArray(m_pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_pointVBO);

		m_pointProgram.setUniformMat4f("view", m_view);
		m_pointProgram.setUniformMat4f("projection", m_projection);


		if (!m_points.empty())
			glDrawArrays(GL_POINTS, 0, m_points.size() / 8);

		m_pointProgram.setUniformMat4f("view", identity);
		m_pointProgram.setUniformMat4f("projection", identity);

		if (!m_ndcPoints.empty())
		{
			glDrawArrays(GL_POINTS, 0, m_ndcPoints.size() / 8);
		}
	}

	void Renderer2D::drawTextProgram()
	{
		ZoneScopedN("[Renderer2D] On Draw Text Program");

		m_fontProgram.use();

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_frameBufferWidth), 0.0f, static_cast<float>(m_frameBufferHeight));

		m_fontProgram.setUniformMat4f("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_fontVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);

		size_t offset = 0;

		if(!m_textureIDs.empty())
		{
			for (int i = 0; i < m_textureIDs.size(); ++i)
			{
				m_fontProgram.setUniform4f("fontColor", m_textColors[i].r, m_textColors[i].g, m_textColors[i].b, m_textColors[i].a);
				glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]);
				glDrawArrays(GL_TRIANGLES, offset, 6);
				offset += 6;
			}
		}
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
		if (width == 0 || height == 0)
			return;

		m_frameBufferWidth = width;
		m_frameBufferHeight = height;

		glViewport(0, 0, width, height);

		buildViewProjectionMatrix();

		updateScreenAABB();
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
		return m_pixelToMeter;
	}

	AABB Renderer2D::screenAABB() const
	{
		return m_screenAABB;
	}

	void Renderer2D::updateScreenAABB()
	{
		Vector2 screenTopLeft = screenToWorld({ 0.0f, 0.0f });
		Vector2 screenBottomRight = screenToWorld({ static_cast<real>(m_frameBufferWidth), static_cast<real>(m_frameBufferHeight) });

		m_screenAABB = AABB::fromBox(screenTopLeft, screenBottomRight);
	}

	void Renderer2D::onZoomView(float xoffset, float yoffset)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		m_scrollMouseScreenStart = { static_cast<float>(xpos), static_cast<float>(ypos) };
		m_scrollMouseStart = screenToWorld(m_scrollMouseScreenStart);

		m_meterToPixel *= 1.0f + yoffset * m_scaleRatio;
		m_meterToPixel = std::clamp(m_meterToPixel, 1.0f / m_maxMeterToPixel, m_maxMeterToPixel);

		m_easingMeterToPixel.continueTo(m_meterToPixel, m_zoomingDuration);

		if (!m_smoothZooming)
		{
			m_easingMeterToPixel.finish();
			m_pixelToMeter = 1.0f / m_meterToPixel;

			m_orthoSize = 0.25f * (m_zFar - m_zNear) / m_zFar * (static_cast<float>(m_frameBufferWidth) / (m_aspectRatio * m_easingMeterToPixel.value()));

			float h = m_orthoSize;
			float w = m_aspectRatio * h;

			m_projection = glm::ortho(-w, w, -h, h, m_zNear, m_zFar);

			Vector2 after = screenToWorld({ static_cast<float>(xpos), static_cast<float>(ypos) });

			Vector2 scrollDelta = m_scrollMouseStart - after;

			m_cameraPosition += glm::vec3(scrollDelta.x, scrollDelta.y, 0.0f);

			m_scrollMouseStart.clear();

			m_view = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);

			updateScreenAABB();
		}
	}

	void Renderer2D::onTranslateView(float x, float y)
	{
		if (!m_translationStart)
		{
			m_mouseScreenStart.set(x, y);
			m_translationStart = true;
			m_translationStartPos = m_cameraPosition;
			return;
		}

		Vector2 currentMouse(x, y);
		currentMouse = screenToWorld(currentMouse);

		Vector2 oldMouse = m_mouseScreenStart;
		oldMouse = screenToWorld(oldMouse);

		Vector2 dMouse = currentMouse - oldMouse;

		glm::vec3 delta = glm::vec3(dMouse.x, dMouse.y, 0.0f);

		glm::vec3 pos = m_translationStartPos - delta;

		m_cameraPosition = pos;

		m_view = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);

		updateScreenAABB();
	}

	void Renderer2D::buildViewProjectionMatrix()
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
