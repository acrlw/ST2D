#include "Quaternion.h"

namespace ST
{
	Quaternion::Quaternion(const real& _s, const real& _i, const real& _j, const real& _k)
		: s(_s), v(_i, _j, _k)
	{
		
	}

	Quaternion::Quaternion(const Vector4& vec4)
	{
		s = vec4.w;
		v = Vector3(vec4.x, vec4.y, vec4.z);
	}

	Quaternion::Quaternion(const real& _s, const Vector3& vec3)
	{
		s = _s;
		v = vec3;
	}

	Quaternion::Quaternion(const Quaternion& copy)
	{
		s = copy.s;
		v = copy.v;
	}
}
