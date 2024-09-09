#include "Palette.h"

namespace STEditor
{
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
