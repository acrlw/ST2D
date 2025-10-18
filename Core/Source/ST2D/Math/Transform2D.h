#pragma once
#include "Complex.h"
#include "Matrix2x2.h"

namespace ST
{
	struct ST_API Transform2D
	{
		//refer https://docs.unity3d.com/ScriptReference/Transform2D.html
		Vector2 position;
		real rotation = 0;
		real scale = 1.0f;

		Vector2 translatePoint(const Vector2& source) const
		{
			return Complex(rotation).multiply(source) * scale + position;
		}

		Vector2 inverseTranslatePoint(const Vector2& source) const
		{
			return Complex(-rotation).multiply(source - position) / scale;
		}

		Vector2 inverseRotatePoint(const Vector2& point) const
		{
			return Complex(-rotation).multiply(point);
		}
	};
}