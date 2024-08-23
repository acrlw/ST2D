#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

#include "ST2DCore.h"


namespace STEditor
{

	using namespace ST;

	struct Viewport
	{
		Viewport() = default;

		Viewport(const Vector2& top_left, const Vector2& bottom_right) : topLeft(top_left),
			bottomRight(bottom_right)
		{
		}

		Vector2 topLeft = { 0, 0 };
		Vector2 bottomRight = { 1920, 1080 };
		real width();
		real height();
		Vector2 center()const;
		void setWidth(const real& width);
		void setHeight(const real& height);
		void set(const real& width, const real& height);
	};

	class Camera2D
	{
	public:
		Camera2D();

		void onUpdate(float deltaTime);
		void onRender(sf::RenderWindow& window);


		bool& gridScaleLineVisible();
		bool& visible();

		bool& coordinateScale();
		bool& smoothZoom();

		int axisPointCount() const;
		void setAxisPointCount(int count);

		void setMeterToPixel(real value);

		real meterToPixel() const;

		real pixelToMeter() const;

		Vector2 transform() const;
		void setTransform(const Vector2& transform);

		Viewport viewport() const;
		void setViewport(const Viewport& viewport);

		Vector2 worldToScreen(const Vector2& pos) const;
		Vector2 screenToWorld(const Vector2& pos) const;

		real defaultMeterToPixel()const;
		void setDefaultMeterToPixel(const real& number);

		void setPreScrollScreenMousePos(const Vector2& pos);

		void setFont(sf::Font* font);
		sf::Font* font();

		bool checkPointInViewport(const Vector2& worldPos);
		

	private:

		void drawGridScaleLine(sf::RenderWindow& window);

		bool m_visible = true;

		bool m_gridScaleLineVisible = true;
		bool m_centerVisible = false;

		bool m_drawCoordinateScale = true;
		bool m_smoothZoom = false;

		real m_defaultMeterToPixel = 80.0f;
		real m_pixelToMeter = 0.02f;
		real m_easingDuration = 0.5f;

		Vector2 m_transform;
		Vector2 m_origin;
		Viewport m_viewport;
		
		int m_axisPointCount = 100;

		Vector2 m_preScrollScreenMousePos;
		Vector2 m_preScrollWorldMousePos;

		EasingObject<float> m_meterToPixelEasing = EasingObject(50.0f);

		sf::Font* m_font = nullptr;
	};

	
}
