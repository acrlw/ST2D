#include "Camera2D.h"

#include "Render.h"

namespace STEditor
{
	real Viewport::width()
	{
		return bottomRight.x - topLeft.x;
	}

	real Viewport::height()
	{
		return bottomRight.y - topLeft.y;
	}

	Vector2 Viewport::center() const
	{
		return (bottomRight - topLeft) * 0.5f;
	}

	void Viewport::setWidth(const real& width)
	{
		bottomRight.x = topLeft.x + width;
	}

	void Viewport::setHeight(const real& height)
	{
		bottomRight.y = topLeft.y + height;
	}

	void Viewport::set(const real& width, const real& height)
	{
		setWidth(width);
		setHeight(height);
	}

	Camera2D::Camera2D()
	{
		m_meterToPixelEasing.setEasingFunction(EasingFunction::easeOutExpo);
		m_meterToPixelEasing.restart(m_defaultMeterToPixel * 3.0f , m_defaultMeterToPixel * 4.0f, m_easingDuration * 0.2f);
		m_origin = m_viewport.center();
	}

	void Camera2D::onUpdate(float deltaTime)
	{
		if (!m_smoothZoom)
		{
			m_meterToPixelEasing.finish();
			if(!m_preScreenMousePos.isOrigin())
			{
				Vector2 deltaTransform = (screenToWorld(m_preScreenMousePos) - m_preWorldMousePos) * m_meterToPixelEasing.value();
				m_transform += deltaTransform;
			}
		}
		else
			m_meterToPixelEasing.update(deltaTime);

		m_pixelToMeter = 1.0f / m_meterToPixelEasing.value();

		bool isZooming = !m_meterToPixelEasing.isFinished();

		if (isZooming && !m_preScreenMousePos.isOrigin())
		{
			m_transform += (screenToWorld(m_preScreenMousePos) - m_preWorldMousePos) * m_meterToPixelEasing.value();
		}
	}

	void Camera2D::onRender(sf::RenderWindow& window)
	{
		if (m_visible)
		{

			//draw background
			window.clear(sf::Color(50, 50, 50));

			
			if (m_treeVisible)
			{
				if(m_tree != nullptr)
					drawTree(m_tree->rootIndex(), window);
			}
			if (m_uniformGridVisible && m_grid != nullptr)
			{
				for (auto&& elem : m_grid->m_cellsToBodies)
				{
					Vector2 topLeft(static_cast<real>(elem.first.x) * m_grid->cellWidth() - m_grid->width() * 0.5f,
						static_cast<real>(elem.first.y) * m_grid->cellHeight() - m_grid->height() * 0.5f);
					AABB cell(topLeft, m_grid->cellWidth(), m_grid->cellHeight());
					//cell.expand(-0.05f);

					RenderSFMLImpl::renderAABB(window, *this, cell, sf::Color::Cyan);
				}
			}
			if (m_gridScaleLineVisible)
			{
				//draw axis

				sf::Color color = sf::Color::Green;
				color.a = 250;

				std::vector<Vector2> axisPoints;
				axisPoints.reserve(m_axisPointCount * 2 + 1);

				for (int i = -m_axisPointCount; i <= m_axisPointCount; ++i)
				{
					axisPoints.emplace_back(Vector2(0, static_cast<real>(i)));
					axisPoints.emplace_back(Vector2(static_cast<real>(i), 0));
				}


				//draw grid
				drawGridScaleLine(window);
				RenderSFMLImpl::renderPoints(window, *this, axisPoints, color);
				color.a = 140;
				RenderSFMLImpl::renderLine(window, *this, Vector2(0.0f, static_cast<real>(-m_axisPointCount)),
					Vector2(0.0f, static_cast<real>(m_axisPointCount)), color);
				RenderSFMLImpl::renderLine(window, *this, Vector2(static_cast<real>(-m_axisPointCount), 0.0f),
					Vector2(static_cast<real>(m_axisPointCount), 0.0f), color);
			}

		}
	}
	bool& Camera2D::gridScaleLineVisible()
	{
		return m_gridScaleLineVisible;
	}

	bool& Camera2D::visible()
	{
		return m_visible;
	}

	bool& Camera2D::treeVisible()
	{
		return m_treeVisible;
	}

	bool& Camera2D::uniformGridVisible()
	{
		return m_uniformGridVisible;
	}

	bool& Camera2D::coordinateScale()
	{
		return m_drawCoordinateScale;
	}

	bool& Camera2D::smoothZoom()
	{
		return m_smoothZoom;
	}

	int Camera2D::axisPointCount() const
	{
		return m_axisPointCount;
	}

	void Camera2D::setAxisPointCount(int count)
	{
		m_axisPointCount = count;
	}

	void Camera2D::setMeterToPixel(real value)
	{
		m_meterToPixelEasing.continueTo(value, m_easingDuration);
	}

	real Camera2D::meterToPixel() const
	{
		return m_meterToPixelEasing.value();
	}

	real Camera2D::pixelToMeter() const
	{
		return 1.0f / m_meterToPixelEasing.value();
	}

	Vector2 Camera2D::transform() const
	{
		return m_transform;
	}

	void Camera2D::setTransform(const Vector2& transform)
	{
		m_transform = transform;
	}

	Viewport Camera2D::viewport() const
	{
		return m_viewport;
	}

	void Camera2D::setViewport(const Viewport& viewport)
	{
		m_viewport = viewport;
		m_origin.set((m_viewport.topLeft.x + m_viewport.bottomRight.x) * (0.5),
			(m_viewport.topLeft.y + m_viewport.bottomRight.y) * (0.5));
	}

	Vector2 Camera2D::worldToScreen(const Vector2& pos) const
	{
		Vector2 real_origin(m_origin.x + m_transform.x, m_origin.y - m_transform.y);
		return Vector2(real_origin.x + pos.x * m_meterToPixelEasing.value() , real_origin.y - pos.y * m_meterToPixelEasing.value());
	}

	Vector2 Camera2D::screenToWorld(const Vector2& pos) const
	{
		Vector2 real_origin(m_origin.x + m_transform.x, m_origin.y - m_transform.y);
		Vector2 result = pos - real_origin;
		result.y = -result.y;
		result *= pixelToMeter();
		return result;
	}

	Tree* Camera2D::tree() const
	{
		return m_tree;
	}

	void Camera2D::setTree(Tree* tree)
	{
		m_tree = tree;
	}

	UniformGrid* Camera2D::uniformGrid() const
	{
		return m_grid;
	}

	void Camera2D::setUniformGrid(UniformGrid* grid)
	{
		m_grid = grid;
	}

	real Camera2D::defaultMeterToPixel() const
	{
		return m_defaultMeterToPixel;
	}

	void Camera2D::setDefaultMeterToPixel(const real& number)
	{
		m_defaultMeterToPixel = number;
	}

	void Camera2D::setPreScreenMousePos(const Vector2& pos)
	{
		m_preScreenMousePos = pos;
		m_preWorldMousePos = screenToWorld(pos);
	}

	void Camera2D::setFont(sf::Font* font)
	{
		m_font = font;
	}

	sf::Font* Camera2D::font()
	{
		return m_font;
	}

	bool Camera2D::checkPointInViewport(const Vector2& pos)
	{
		Vector2 topLeft = screenToWorld(m_viewport.topLeft);
		Vector2 bottomRight = screenToWorld(m_viewport.bottomRight);
		return GeometryAlgorithm2D::isPointInsideAABB(pos, topLeft, bottomRight);
	}

	void Camera2D::drawGridScaleLine(sf::RenderWindow& window)
	{
		Vector2 topLeft = screenToWorld(m_viewport.topLeft);
		Vector2 bottomRight = screenToWorld(m_viewport.bottomRight);

		sf::Color thick = RenderConstant::DarkGreen;
		thick.a = 160;
		sf::Color thin = RenderConstant::DarkGreen;
		thin.a = 60;

		const bool fineEnough = m_meterToPixelEasing.value() > 125;

		std::vector<std::pair<Vector2, Vector2>> lines;
		lines.reserve(m_axisPointCount * 2);

		int h = 1;

		if (m_meterToPixelEasing.value() < 30)
			h = 10;
		else if (m_meterToPixelEasing.value() < 60)
			h = 5;
		else if (m_meterToPixelEasing.value() < 120)
			h = 2;

		sf::Color color = sf::Color::Green;
		color.a = 80;
		for (int i = -m_axisPointCount; i <= m_axisPointCount; i += h)
		{
			Vector2 p1 = { static_cast<real>(i), static_cast<real>(m_axisPointCount) };
			Vector2 p2 = { static_cast<real>(i), static_cast<real>(-m_axisPointCount) };
			Vector2 p3 = { static_cast<real>(-m_axisPointCount), static_cast<real>(i) };
			Vector2 p4 = { static_cast<real>(m_axisPointCount), static_cast<real>(i) };

			bool checkOutOfX = p1.x > bottomRight.x || p1.x < topLeft.x;
			bool checkOutOfY = p3.y > topLeft.y || p3.y < bottomRight.y;

			if(!checkOutOfX)
				lines.emplace_back(p1, p2);
			if(!checkOutOfY)
				lines.emplace_back(p3, p4);

			//draw number
			if (!m_drawCoordinateScale)
				continue;

			std::string str = std::format("{}", i);
			if (!checkOutOfX)
				RenderSFMLImpl::renderText(window, *this, Vector2(static_cast<real>(i), 0.0f), *m_font, str, color, 16, { -0.25f, -0.25f });
			if (i == 0)
				continue;
			if (!checkOutOfY)
				RenderSFMLImpl::renderText(window, *this, Vector2(0.0f, static_cast<real>(i)), *m_font, str, color, 16, { -0.25f, -0.25f });

		}

		RenderSFMLImpl::renderLines(window, *this, lines, thick);


		if (fineEnough)
		{
			lines.clear();
			lines.reserve(m_axisPointCount);

			int slice = 50;
			if (m_meterToPixelEasing.value() < 250)
				slice = 2;
			else if (m_meterToPixelEasing.value() < 800)
				slice = 10;
			else if (m_meterToPixelEasing.value() < 2000)
				slice = 25;

			const real inv = 1.0f / static_cast<real>(slice);
			for (int i = -m_axisPointCount, j = -m_axisPointCount + h; i < m_axisPointCount; i += h, j += h)
			{
				for (int k = 1; k < slice; ++k)
				{
					real index = static_cast<real>(k) * inv;

					Vector2 p1 = { static_cast<real>(i) + index, static_cast<real>(m_axisPointCount) };
					Vector2 p2 = { static_cast<real>(i) + index, static_cast<real>(-m_axisPointCount) };
					Vector2 p3 = { static_cast<real>(-m_axisPointCount), static_cast<real>(i) + index };
					Vector2 p4 = { static_cast<real>(m_axisPointCount), static_cast<real>(i) + index };

					bool checkOutOfX = p1.x > bottomRight.x || p1.x < topLeft.x;
					bool checkOutOfY = p3.y > topLeft.y || p3.y < bottomRight.y;

					if (!checkOutOfX)
						lines.emplace_back(p1, p2);
					if (!checkOutOfY)
						lines.emplace_back(p3, p4);

				}
			}
			RenderSFMLImpl::renderLines(window, *this, lines, thin);
		}
	}

	void Camera2D::drawTree(int nodeIndex, sf::RenderWindow& window)
	{
		if (nodeIndex == -1)
			return;
		//std::cout << "tree size:" << m_tree->tree().size() << std::endl;
		drawTree(m_tree->tree()[nodeIndex].leftIndex, window);
		drawTree(m_tree->tree()[nodeIndex].rightIndex, window);

		AABB aabb = m_tree->tree()[nodeIndex].aabb;
		//aabb.expand(0.5);
		if (!m_tree->tree()[nodeIndex].isLeaf())
			RenderSFMLImpl::renderAABB(window, *this, aabb, sf::Color::Cyan);
	}
}
