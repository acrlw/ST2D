#pragma once

#include "Vector4.h"

namespace ST
{
	struct ST_API Quaternion
	{
		Quaternion(const real& s, const real& i, const real& j, const real& k);
		Quaternion(const Vector4& vec4);
		Quaternion(const real& s, const Vector3& vec3);
		Quaternion(const Quaternion& copy) = default;
		Quaternion(Quaternion&& copy) = default;
		real s = 0.0f;
		Vector3 v;
	};
}
