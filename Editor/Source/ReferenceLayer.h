#pragma once

#include "Renderer2D.h"

namespace STEditor
{
	using namespace ST;

	class ReferenceLayer
	{
	public:

		void onRender(Renderer2D* renderer);

	private:

		bool m_visible = true;
		bool m_gridVisible = true;
		bool m_axisVisible = true;

		int m_axisPointCount = 100;

	};
}