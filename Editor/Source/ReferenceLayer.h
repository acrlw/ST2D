#pragma once

#include "Renderer2D.h"

namespace STEditor
{
	using namespace ST;

	class ReferenceLayer
	{
	public:

		void onRender(Renderer2D& renderer);

		bool& coordsVisible() { return m_coordsVisible; }
		bool& gridVisible() { return m_gridVisible; }
	private:

		bool m_coordsVisible = true;
		bool m_gridVisible = true;

		int m_gridMaxPoint = 100;
		Color axisColor = DarkPalette::Green;
		Color thin = DarkPalette::DarkGreen;
		Color thick = DarkPalette::DarkGreen;
		float numberPixelOffset = 6.0f;

	};
}