#pragma once

namespace STEditor
{
	struct Color
	{
		float r, g, b, a;
		constexpr Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
			: r(red), g(green), b(blue), a(alpha) {}

		constexpr Color(int red, int green, int blue, int alpha = 255)
			: r(static_cast<float>(red) / 255.0f), g(static_cast<float>(green) / 255.0f), b(static_cast<float>(blue) / 255.0f), a(static_cast<float>(alpha) / 255.0f) {}
	};

	namespace DarkPalette
	{
		constexpr Color Background(0.16f, 0.16f, 0.16f, 1.0f);
		constexpr Color Red(244, 67, 54);
		constexpr Color Green(76, 175, 80);
		constexpr Color Blue(3, 169, 244);
		constexpr Color Yellow(255, 235, 59);
		constexpr Color Cyan(0, 188, 212);
		constexpr Color Pink(233, 30, 99);
		constexpr Color Gray(189, 189, 189);
		constexpr Color Orange(255, 138, 101);
		constexpr Color Teal(29, 233, 182);
		constexpr Color Purple(156, 39, 176);

		constexpr Color LightRed(255, 205, 210);
		constexpr Color LightGreen(139, 195, 74);
		constexpr Color LightBlue(0, 188, 212);
		constexpr Color LightCyan(178, 235, 242);
		constexpr Color LightGray(158, 158, 158);

		constexpr Color DarkRed(211, 47, 47);
		constexpr Color DarkGreen(44, 113, 48);
		constexpr Color DarkBlue(2, 136, 209);

	}

	namespace LightPalette
	{
		constexpr Color Background(1.0f, 1.0f, 1.0f, 1.0f);
		constexpr Color Red(229, 57, 53);
		constexpr Color Green(67, 160, 71);
		constexpr Color Blue(30, 136, 229);
		constexpr Color Yellow(242, 190, 69);
		constexpr Color Cyan(0, 131, 143);
		constexpr Color Pink(194, 24, 91);
		constexpr Color Gray(117, 117, 117);
		constexpr Color Orange(230, 81, 0);
		constexpr Color Teal(0, 105, 92);
		constexpr Color Purple(106, 27, 154);

		constexpr Color DarkRed(183, 28, 28);
		constexpr Color DarkGreen(27, 94, 32);
		constexpr Color DarkBlue(13, 71, 161);
	}

	enum class ThemeMode {
		Dark,
		Light
	};

	class ColorManager {
	private:
		ColorManager() = default;
		static ColorManager& instance() {
			static ColorManager inst;
			return inst;
		}
		ThemeMode m_mode;

		Color m_background = DarkPalette::Background;

		Color m_red = DarkPalette::Red;
		Color m_green = DarkPalette::Green;
		Color m_blue = DarkPalette::Blue;
		Color m_yellow = DarkPalette::Yellow;
		Color m_cyan = DarkPalette::Cyan;
		Color m_pink = DarkPalette::Pink;
		Color m_gray = DarkPalette::Gray;
		Color m_orange = DarkPalette::Orange;
		Color m_teal = DarkPalette::Teal;
		Color m_purple = DarkPalette::Purple;

		Color m_lightRed = DarkPalette::LightRed;
		Color m_lightGreen = DarkPalette::LightGreen;
		Color m_lightBlue = DarkPalette::LightBlue;
		Color m_lightCyan = DarkPalette::LightCyan;
		Color m_lightGray = DarkPalette::LightGray;

		Color m_darkRed = DarkPalette::DarkRed;
		Color m_darkGreen = DarkPalette::DarkGreen;
		Color m_darkBlue = DarkPalette::DarkBlue;
	public:
		static const Color& Background() { return instance().m_background; }
		static const Color& Red() { return instance().m_red; }
		static const Color& Green() { return instance().m_green; }
		static const Color& Blue() { return instance().m_blue; }
		static const Color& Cyan() { return instance().m_cyan; }
		static const Color& Yellow() { return instance().m_yellow; }
		static const Color& Pink() { return instance().m_pink; }
		static const Color& Gray() { return instance().m_gray; }
		static const Color& Orange() { return instance().m_orange; }
		static const Color& Teal() { return instance().m_teal; }
		static const Color& Purple() { return instance().m_purple; }

		static const Color& LightRed() { return instance().m_lightRed; }
		static const Color& LightGreen() { return instance().m_lightGreen; }
		static const Color& LightBlue() { return instance().m_lightBlue; }
		static const Color& LightCyan() { return instance().m_lightCyan; }
		static const Color& LightGray() { return instance().m_lightGray; }

		static const Color& DarkRed() { return instance().m_darkRed; }
		static const Color& DarkGreen() { return instance().m_darkGreen; }
		static const Color& DarkBlue() { return instance().m_darkBlue; }


		static void setThemeMode(ThemeMode mode);
	};
	namespace Palette
	{
		inline const Color& Background = ColorManager::Background();
		inline const Color& Red = ColorManager::Red();
		inline const Color& Green = ColorManager::Green();
		inline const Color& Blue = ColorManager::Blue();
		inline const Color& Cyan = ColorManager::Cyan();
		inline const Color& Yellow = ColorManager::Yellow();
		inline const Color& Pink = ColorManager::Pink();
		inline const Color& Gray = ColorManager::Gray();
		inline const Color& Orange = ColorManager::Orange();
		inline const Color& Teal = ColorManager::Teal();
		inline const Color& Purple = ColorManager::Purple();

		inline const Color& LightRed = ColorManager::LightRed();
		inline const Color& LightGreen = ColorManager::LightGreen();
		inline const Color& LightBlue = ColorManager::LightBlue();
		inline const Color& LightCyan = ColorManager::LightCyan();
		inline const Color& LightGray = ColorManager::LightGray();

		inline const Color& DarkRed = ColorManager::DarkRed();
		inline const Color& DarkGreen = ColorManager::DarkGreen();
		inline const Color& DarkBlue = ColorManager::DarkBlue();

		inline void setThemeMode(ThemeMode mode) {
			ColorManager::setThemeMode(mode);
		}
	}
}