#include "Palette.h"

namespace STEditor
{
	Color jetColorMap(float value)
	{
		value = std::clamp(value, 0.0f, 1.0f);

		Color color;
		if (value < 0.25f) {
			color.r = 0.0f;
			color.g = 4.0f * value;
			color.b = 1.0f;
		}
		else if (value < 0.5f) {
			color.r = 0.0f;
			color.g = 1.0f;
			color.b = 1.0f - 4.0f * (value - 0.25f);
		}
		else if (value < 0.75f) {
			color.r = 4.0f * (value - 0.5f);
			color.g = 1.0f;
			color.b = 0.0f;
		}
		else {
			color.r = 1.0f;
			color.g = 1.0f - 4.0f * (value - 0.75f);
			color.b = 0.0;
		}

		return color;
	}

	Color hsvToRgb(double h, double s, double v)
	{
		float r, g, b;

		int i = static_cast<int>(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);

		switch (i % 6) {
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
		}

		return { r, g, b };
	}

	Color gistRainbowColormap(double value)
	{

		value = std::max(0.0, std::min(1.0, value));

		float h = value;
		float s = 1.0;
		float v = 1.0;

		return hsvToRgb(h, s, v);
	}

	void ColorManager::setThemeMode(ThemeMode mode)
	{
		auto& inst = instance();
		inst.m_mode = mode;
		if (mode == ThemeMode::Dark) 
		{
			inst.m_background = DarkPalette::Background;

			inst.m_red = DarkPalette::Red;
			inst.m_green = DarkPalette::Green;
			inst.m_blue = DarkPalette::Blue;
			inst.m_yellow = DarkPalette::Yellow;
			inst.m_cyan = DarkPalette::Cyan;
			inst.m_pink = DarkPalette::Pink;
			inst.m_gray = DarkPalette::Gray;
			inst.m_orange = DarkPalette::Orange;
			inst.m_teal = DarkPalette::Teal;
			inst.m_purple = DarkPalette::Purple;

			inst.m_lightRed = DarkPalette::LightRed;
			inst.m_lightGreen = DarkPalette::LightGreen;
			inst.m_lightBlue = DarkPalette::LightBlue;
			inst.m_lightCyan = DarkPalette::LightCyan;
			inst.m_lightGray = DarkPalette::LightGray;

			inst.m_darkRed = DarkPalette::DarkRed;
			inst.m_darkGreen = DarkPalette::DarkGreen;
			inst.m_darkBlue = DarkPalette::DarkBlue;
		}
		else
		{
			inst.m_background = LightPalette::Background;

			inst.m_red = LightPalette::Red;
			inst.m_green = LightPalette::Green;
			inst.m_blue = LightPalette::Blue;
			inst.m_yellow = LightPalette::Yellow;
			inst.m_cyan = LightPalette::Cyan;
			inst.m_pink = LightPalette::Pink;
			inst.m_gray = LightPalette::Gray;
			inst.m_orange = LightPalette::Orange;
			inst.m_teal = LightPalette::Teal;
			inst.m_purple = LightPalette::Purple;

			inst.m_darkRed = LightPalette::DarkRed;
			inst.m_darkGreen = LightPalette::DarkGreen;
			inst.m_darkBlue = LightPalette::DarkBlue;
		}
	}
}
