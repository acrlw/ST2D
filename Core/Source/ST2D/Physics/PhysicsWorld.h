#pragma once

#include "ST2D/Core.h"
#include "ST2D/IdPool.h"
#include "ST2D/Shape/AABB.h"

namespace ST
{
	class ST_API PhysicsWorld
	{
	public:


	private:

		std::vector<int> m_objectIds;

		std::vector<Transform> m_transform;
		std::vector<Vector2> m_velocity;
		std::vector<float> m_angularVelocity;
		std::vector<Vector2> m_force;
		std::vector<float> m_torque;
		std::vector<float> m_mass;
		std::vector<float> m_inertia;
		std::vector<float> m_invMass;
		std::vector<float> m_invInertia;
		std::vector<float> m_restitution;
		std::vector<float> m_friction;

		std::vector<ST::AABB> m_aabb;
		std::vector<uint32_t> m_bitmask;
		std::vector<ST::Shape*> m_shape;


		IdPool m_objectIdPool;
	};
}