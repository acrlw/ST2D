#include "PhysicsScene.h"
#include "xmmintrin.h"
#include "emmintrin.h"
#include "immintrin.h"

namespace STEditor
{
	PhysicsScene::~PhysicsScene()
	{

	}

	void PhysicsScene::onLoad()
	{
		rect.set(0.5f, 0.5f);
		land.set(80.0f, 0.1f);
		capsule.set(1.0f, 2.0f);
		ellipse.set(1.0f, 2.0f);
		circle.setRadius(0.25f);
		edge.set(Vector2(-10.0f, -0.5f), Vector2(10.0f, -0.5f));
		polygon.set({ Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f), Vector2(1.0f, 1.0f) });

		createObjects();
	}

	void PhysicsScene::onUnLoad()
	{

	}



	void PhysicsScene::onUpdate(float deltaTime)
	{
		ZoneScopedN("[PhysicsScene] On Update");

		if (!m_simulate)
			return;

		float dt = 1.0f / 60.0f;

		if(m_flagInitial)
		{
			m_flagInitial = false;

			generateContacts(dt);
		}
		

		// integrate velocities
		integrateVelocities(dt);

		// set up contact constraint
		setUpConstraint(dt);

		// solve velocity
		solveVelocities(dt);

		// integrate positions
		integratePositions(dt);

		// solve position
		solvePositions(dt);

		// disable all contacts

		for (auto& value : m_contacts | std::views::values)
			value.count = 0;

		// update broad phase
		updateBroadphase(dt);

		// update narrow phase and generate contacts
		generateContacts(dt);

		// clear all force and torque

		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			m_forces[i].clear();
			m_torques[i] = 0.0f;
		}
	}

	void PhysicsScene::onRender(Renderer2D& renderer)
	{
		ZoneScopedN("[BroadphaseScene] On Render");

		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			Transform transform(m_positions[i], m_rotations[i], 1.0f);

			if (m_showObjectID)
			{
				std::string id = std::to_string(m_objectIds[i]);
				renderer.text(m_positions[i], Palette::Gray, id);
			}

			if (m_showTransform)
				renderer.orientation(transform);

			if (m_showObject)
				renderer.shape(transform, m_shapes[i], Palette::Green);

			if (m_showAABB)
				renderer.aabb(m_aabbs[i], Palette::Green);

		}

		if (m_showGrid)
		{
			auto color = Palette::Orange;
			for (const auto& [key, value] : m_grid.usedCells())
			{
				if (value.empty())
					continue;

				real row = static_cast<real>(key.row);
				real col = static_cast<real>(key.col);

				Vector2 start = m_grid.gridShift() + Vector2(col * m_grid.cellWidth(), row * m_grid.cellHeight());
				Vector2 end = m_grid.gridShift() + Vector2((col + 1.0f) * m_grid.cellWidth(), (row + 1.0f) * m_grid.cellHeight());
				AABB aabb;
				aabb.position = (start + end) * 0.5f;
				aabb.width = m_grid.cellWidth();
				aabb.height = m_grid.cellHeight();

				renderer.aabb(aabb, color);

			}
		}

		if (m_showDBVT)
		{

			m_dbvtStack.push_back(m_dbvt.rootIndex());
			while (!m_dbvtStack.empty())
			{
				int index = m_dbvtStack.back();
				m_dbvtStack.pop_back();

				if (index == -1)
					continue;

				AABB aabb = m_dbvt.nodes()[index].aabb;
				aabb.expand(m_expandRatio * static_cast<real>(1 + m_dbvt.nodes()[index].height));


				renderer.aabb(aabb, Palette::Cyan);

				m_dbvtStack.push_back(m_dbvt.nodes()[index].left);
				m_dbvtStack.push_back(m_dbvt.nodes()[index].right);

			}


		}

		if(m_showContacts)
		{
			for(auto& value : m_contacts | std::views::values)
			{
				if (value.count == 0)
					continue;

				renderer.point(value.pair.points[0], Palette::LightRed);
				renderer.point(value.pair.points[2], Palette::LightBlue);

				if(value.count == 2)
				{
					renderer.point(value.pair.points[1], Palette::LightRed);
					renderer.point(value.pair.points[3], Palette::LightBlue);
				}
			}

			if(m_showContactsMagnitude)
			{
				
			}
		}

	}


	void PhysicsScene::onRenderUI()
	{
		ImGui::Begin("Physics");

		int count = m_count;
		ImGui::DragInt("Count", &count, 1, 1, 5000);
		if (m_count != count)
		{
			m_count = count;
			createObjects();
		}

		ImGui::Checkbox("Simulate", &m_simulate);
		ImGui::Checkbox("Enable Gravity", &m_enableGravity);
		ImGui::Checkbox("Enable Damping", &m_enableDamping);
		ImGui::Checkbox("Enable Warmstart", &m_enableWarmstart);
		ImGui::Checkbox("Enable Vel Block Solver", &m_enableVelocityBlockSolver);
		ImGui::Checkbox("Enable Pos Block Solver", &m_enablePositionBlockSolver);

		ImGui::Checkbox("Show Object", &m_showObject);
		ImGui::Checkbox("Show Object Id", &m_showObjectID);
		ImGui::Checkbox("Show Transform", &m_showTransform);
		ImGui::Checkbox("Show Joint", &m_showJoint);

		ImGui::Checkbox("Show AABB", &m_showAABB);
		ImGui::Checkbox("Show Grid", &m_showGrid);
		ImGui::Checkbox("Show DBVT", &m_showDBVT);
		ImGui::Checkbox("Show Graph Coloring", &m_showGraphColor);
		ImGui::Checkbox("Show Contacts", &m_showContacts);
		ImGui::Checkbox("Show Contacts Magnitude", &m_showContactsMagnitude);

		ImGui::DragFloat("Expand Ratio", &m_expandRatio, 0.01f, 0.0f, 1.0f);

		ImGui::End();
	}

	void PhysicsScene::createObjects()
	{
		clearObjects();

		std::vector<BroadphaseObjectBinding> bindings;
		bindings.reserve(m_count);

		{
			ZoneScopedN("[PhysicsScene] Create and add shape to list");

			real offset = 0.6f;
			real max = static_cast<real>(m_count);
			real xSpacing = 0.2f;
			real ySpacing = 0.01f;
			for (real j = 0; j < max; j += 0.5f)
			{
				for (real i = 0.0; i < max - j; i += 0.5f)
				{
					Transform t;
					t.position.set({ i * (1.0f + xSpacing) + offset, j * (1.0f + ySpacing) + 0.24f });
					t.rotation = 0;

					auto id = m_objectIdPool.getNewId();
					m_objectIds.push_back(id);

					m_positions.emplace_back(t.position);
					m_rotations.emplace_back(t.rotation);
					m_velocities.emplace_back(0.0f, 0.0f);
					m_angularVelocities.emplace_back(0.0f);
					m_forces.emplace_back(0.0f);
					m_torques.emplace_back(0.0f);
					m_masses.emplace_back(1.0f + i);
					m_inertias.emplace_back(1.0f + i);
					m_invMasses.emplace_back(1.0f / (1.0f + i));
					m_invInertias.emplace_back(1.0f / (1.0f + i));
					m_restitutions.emplace_back(0.0f);
					m_frictions.emplace_back(0.5f);

					m_aabbs.push_back(AABB::fromShape(t, &rect));
					m_bitmasks.push_back(1);
					m_shapes.push_back(&rect);

					bindings.emplace_back(m_objectIds.back(), 1, m_aabbs.back());

				}
				offset += 0.3f;
			}
		}
		{

			Transform trans;
			trans.position.set(40.0f, -0.05);

			m_landId = m_objectIdPool.getNewId();
			m_objectIds.push_back(m_landId);

			m_positions.emplace_back(trans.position);
			m_rotations.emplace_back(trans.rotation);
			m_velocities.emplace_back();
			m_angularVelocities.emplace_back(0.0f);
			m_forces.emplace_back(0.0f);
			m_torques.emplace_back(0.0f);
			m_masses.emplace_back(0.0f);
			m_inertias.emplace_back(0.0f);
			m_invMasses.emplace_back(0.0f);
			m_invInertias.emplace_back(0.0f);
			m_restitutions.emplace_back(0.0f);
			m_frictions.emplace_back(1.0f);

			m_shapes.push_back(&land);
			m_bitmasks.push_back(1);
			m_aabbs.push_back(AABB::fromShape(trans, &land));

			bindings.emplace_back(m_landId, 1, m_aabbs.back());
		}

		{
			ZoneScopedN("[PhysicsScene] Add to Grid");
			for (auto&& binding : bindings)
				m_grid.addObject(binding);
		}

		{
			ZoneScopedN("[PhysicsScene] Add to DBVT");
			for (auto&& binding : bindings)
				m_dbvt.addObject(binding);

		}


	}

	void PhysicsScene::clearObjects()
	{
		m_dbvt.clearAllObjects();
		m_grid.clearAllObjects();

		m_objectIdPool.reset();
		m_objectIds.clear();

		m_positions.clear();
		m_rotations.clear();
		m_velocities.clear();
		m_angularVelocities.clear();
		m_forces.clear();
		m_torques.clear();
		m_masses.clear();
		m_inertias.clear();
		m_invMasses.clear();
		m_invInertias.clear();
		m_restitutions.clear();
		m_frictions.clear();

		m_aabbs.clear();
		m_bitmasks.clear();
		m_shapes.clear();
	}

	void PhysicsScene::generateContacts(float dt)
	{
		ZoneScopedN("Contacts Generation");

		std::vector<ObjectPair> pairs = m_dbvt.queryOverlaps();

		// narrow phase generate contacts

		for (auto&& elem : pairs)
		{
			if (!m_aabbs[elem.objectIdA].collide(m_aabbs[elem.objectIdB]))
				continue;

			Transform transformA(m_positions[elem.objectIdA], m_rotations[elem.objectIdA], 1.0f);
			Transform transformB(m_positions[elem.objectIdB], m_rotations[elem.objectIdB], 1.0f);

			auto simplex = Narrowphase::gjk(transformA, m_shapes[elem.objectIdA], transformB, m_shapes[elem.objectIdB]);
			if (simplex.isContainOrigin)
			{
				auto info = Narrowphase::epa(simplex, transformA, m_shapes[elem.objectIdA], transformB, m_shapes[elem.objectIdB]);
				auto contacts = Narrowphase::generateContacts(info, transformA, m_shapes[elem.objectIdA], transformB, m_shapes[elem.objectIdB]);

				if (m_contacts.contains(elem) && contacts.ids[0].key != m_contacts[elem].pair.ids[0].key
					|| contacts.ids[1].key != m_contacts[elem].pair.ids[1].key)
				{
					m_contacts[elem].contacts[0].sumNormalImpulse = 0.0f;
					m_contacts[elem].contacts[0].sumTangentImpulse = 0.0f;
					m_contacts[elem].contacts[1].sumNormalImpulse = 0.0f;
					m_contacts[elem].contacts[1].sumTangentImpulse = 0.0f;
				}

				Contact contact;
				contact.count = contacts.count;
				contact.pair = contacts;
				contact.normal = info.normal;
				contact.tangent = info.normal.perpendicular();
				contact.penetration = info.penetration;
				m_contacts[elem] = contact;
			}
		}

		// graph coloring

		{
			ZoneScopedN("Graph Coloring");
			m_objectGraph.clearGraph();
			m_objectGraph.addEnableColorRepeated(m_landId);
			m_objectGraph.buildGraph(pairs);
		}
	}

	void PhysicsScene::updateBroadphase(float dt)
	{
		ZoneScopedN("Update Broadphase");
		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			m_aabbs[i] = AABB::fromShape(Transform(m_positions[i], m_rotations[i], 1.0f), m_shapes[i]);
			BroadphaseObjectBinding binding(m_objectIds[i], m_bitmasks[i], m_aabbs[i]);
			m_dbvt.updateObject(binding);
			m_grid.updateObject(binding);
		}
	}

	void PhysicsScene::integrateVelocities(float dt)
	{
		ZoneScopedN("Integrate Velocities");
		Vector2 gravity;
		if (m_enableGravity)
			gravity.set(0.0f, -9.8f);

		real lvd = 1.0f;
		real avd = 1.0f;
		if (m_enableDamping)
		{
			lvd = 1.0f / (1.0f + dt * m_linearVelocityDamping);
			avd = 1.0f / (1.0f + dt * m_angularVelocityDamping);
		}

		// use sse

		__m128 dt4 = _mm_set1_ps(dt);
		__m256 dt8 = _mm256_set1_ps(dt);
		__m256 lvd4 = _mm256_set1_ps(lvd);
		__m128 avd4 = _mm_set1_ps(avd);
		//fill gravity 8 with (0, -9.8, 0, -9.8 ...)
		__m128 grav = _mm_set_ps(gravity.y, gravity.x, gravity.y, gravity.x);
		__m256 vGravity = _mm256_set_m128(grav, grav);

		int index = 0;
		for (; index + 4 <= m_objectIds.size(); index += 4)
		{
			__m256 vVel = _mm256_loadu_ps(&m_velocities[index].x);
			__m256 vForce = _mm256_loadu_ps(&m_forces[index].x);

			__m128 vAngularVel = _mm_loadu_ps(&m_angularVelocities[index]);
			__m128 vTorque = _mm_loadu_ps(&m_torques[index]);

			__m128 vTmpInvMass = _mm_loadu_ps(&m_invMasses[index]);
			__m128 vInvMassLo = _mm_unpacklo_ps(vTmpInvMass, vTmpInvMass);
			__m128 vInvMassHi = _mm_unpackhi_ps(vTmpInvMass, vTmpInvMass);

			__m256 vInvMass = _mm256_set_m128(vInvMassHi, vInvMassLo);


			__m128 vTmpMass = _mm_loadu_ps(&m_masses[index]);
			__m128 vMassLo = _mm_unpacklo_ps(vTmpMass, vTmpMass);
			__m128 vMassHi = _mm_unpackhi_ps(vTmpMass, vTmpMass);
			__m256 vMass = _mm256_set_m128(vMassHi, vMassLo);

			__m128 vInvInertia = _mm_loadu_ps(&m_invInertias[index]);

			// vForce += vGravity * vMass
			// vVel += vForce * vInvMass * dt8
			// vVel *= lvd4

			vForce = _mm256_add_ps(vForce, _mm256_mul_ps(vGravity, vMass));
			vVel = _mm256_add_ps(vVel, _mm256_mul_ps(_mm256_mul_ps(vForce, vInvMass), dt8));
			vVel = _mm256_mul_ps(vVel, lvd4);

			// vAngularVel += vTorque * vInvInertia * dt4
			// vAngularVel *= avd4

			vAngularVel = _mm_add_ps(vAngularVel, _mm_mul_ps(vTorque, _mm_mul_ps(vInvInertia, dt4)));
			vAngularVel = _mm_mul_ps(vAngularVel, avd4);

			// store back

			_mm256_storeu_ps(&m_velocities[index].x, vVel);
			_mm_storeu_ps(&m_angularVelocities[index], vAngularVel);


		}

		for (; index < m_objectIds.size(); ++index)
		{
			m_forces[index] += gravity * m_masses[index];
			m_velocities[index] += m_forces[index] * m_invMasses[index] * dt;
			m_angularVelocities[index] += m_torques[index] * m_invInertias[index] * dt;

			m_velocities[index] *= lvd;
			m_angularVelocities[index] *= avd;
		}
	}

	void PhysicsScene::solveVelocities(float dt)
	{
		ZoneScopedN("Solve Velocity");

		for(int solveIndex = 0;solveIndex < m_solveVelocityCount; ++solveIndex)
		{
			for (auto&& elem : m_objectGraph.m_colorToEdges)
			{
				std::vector<std::future<void>> futures;

				for (auto&& pair : elem)
				{
					futures.emplace_back(
						m_threadPool.enqueue([this, pair, dt]
							{
								processVelocity(pair);
							}));

				}

				for (auto& future : futures)
					future.get();
			}
		}


	}

	void PhysicsScene::integratePositions(float dt)
	{
		ZoneScopedN("Integrate Positions");

		int index = 0;

		for (; index + 4 <= m_objectIds.size(); index += 4)
		{
			__m256 vVel = _mm256_loadu_ps(&m_velocities[index].x);
			__m128 vAngularVel = _mm_loadu_ps(&m_angularVelocities[index]);

			__m256 vPos = _mm256_loadu_ps(&m_positions[index].x);
			__m128 vRot = _mm_loadu_ps(&m_rotations[index]);

			vPos = _mm256_add_ps(vPos, _mm256_mul_ps(vVel, _mm256_set1_ps(dt)));
			vRot = _mm_add_ps(vRot, _mm_mul_ps(vAngularVel, _mm_set1_ps(dt)));

			_mm256_storeu_ps(&m_positions[index].x, vPos);
			_mm_storeu_ps(&m_rotations[index], vRot);
		}

		for (; index < m_objectIds.size(); ++index)
		{
			m_positions[index] += m_velocities[index] * dt;
			m_rotations[index] += m_angularVelocities[index] * dt;
		}
	}

	void PhysicsScene::solvePositions(float dt)
	{
		ZoneScopedN("Solve Position");
		for (int solveIndex = 0; solveIndex < m_solvePositionCount; ++solveIndex)
		{
			for (auto&& elem : m_objectGraph.m_colorToEdges)
			{
				std::vector<std::future<void>> futures;

				for (auto&& pair : elem)
				{
					futures.emplace_back(
						m_threadPool.enqueue([this, pair, dt]
							{
								processPosition(pair);
							}));

				}

				for (auto& future : futures)
					future.get();
			}
		}
	}

	void PhysicsScene::setUpConstraint(float dt)
	{
		ZoneScopedN("Setup Contact Constraint");
		for(auto&& [key ,value] : m_contacts)
		{
			if(value.count > 0)
			{
				Transform transformA(m_positions[key.objectIdA], m_rotations[key.objectIdA], 1.0f);
				Transform transformB(m_positions[key.objectIdB], m_rotations[key.objectIdB], 1.0f);

				value.restitution = Math::min(m_restitutions[key.objectIdA], m_restitutions[key.objectIdB]);
				value.friction = Math::sqrt(m_frictions[key.objectIdA] * m_frictions[key.objectIdB]);

				for(int i = 0; i < value.count; ++i)
				{
					value.contacts[i].localA = transformA.inverseTranslatePoint(value.pair.points[i]);
					value.contacts[i].localB = transformB.inverseTranslatePoint(value.pair.points[i + 2]);
					value.contacts[i].rA = transformA.position - value.pair.points[i];
					value.contacts[i].rB = transformB.position - value.pair.points[i + 2];
					value.contacts[i].penetration = (value.pair.points[i] - value.pair.points[i + 2]).length();
					
					const real im_a = m_invMasses[key.objectIdA];
					const real im_b = m_invMasses[key.objectIdB];
					const real ii_a = m_invInertias[key.objectIdA];
					const real ii_b = m_invInertias[key.objectIdB];

					const real rn_a = value.contacts[i].rA.cross(value.normal);
					const real rn_b = value.contacts[i].rB.cross(value.normal);
					const real rt_a = value.contacts[i].rA.cross(value.tangent);
					const real rt_b = value.contacts[i].rB.cross(value.tangent);

					const real kNormal = im_a + ii_a * rn_a * rn_a +
						im_b + ii_b * rn_b * rn_b;

					const real kTangent = im_a + ii_a * rt_a * rt_a +
						im_b + ii_b * rt_b * rt_b;

					value.contacts[i].effectiveMassNormal = realEqual(kNormal, 0.0f) ? 0 : 1.0f / kNormal;
					value.contacts[i].effectiveMassTangent = realEqual(kTangent, 0.0f) ? 0 : 1.0f / kTangent;

					if(m_enableWarmstart)
					{
						Vector2 impulse = value.contacts[i].sumNormalImpulse * value.normal + value.contacts[i].sumTangentImpulse * value.tangent;

						m_velocities[key.objectIdA] += im_a * impulse;
						m_angularVelocities[key.objectIdA] += ii_a * value.contacts[i].rA.cross(impulse);
						m_velocities[key.objectIdB] -= im_b * impulse;
						m_angularVelocities[key.objectIdB] -= ii_b * value.contacts[i].rB.cross(impulse);
					}

					Vector2 wa = Vector2::crossProduct(m_angularVelocities[key.objectIdA], value.contacts[i].rA);
					Vector2 wb = Vector2::crossProduct(m_angularVelocities[key.objectIdB], value.contacts[i].rB);
					value.contacts[i].vA = m_velocities[key.objectIdA] + wa;
					value.contacts[i].vB = m_velocities[key.objectIdB] + wb;
					value.contacts[i].relativeVelocity = value.normal.dot(value.contacts[i].vA - value.contacts[i].vB);
				}

			}
		}

	}

	void PhysicsScene::processVelocity(const ObjectPair& pair)
	{
		if(m_contacts[pair].count == 2 && m_enableVelocityBlockSolver)
		{
			// solve block
		}
		else if(m_contacts[pair].count > 0)
		{
			// solve one by one
			for(int i = 0;i < m_contacts[pair].count; ++i)
			{
				// solve friction first
				auto& contact = m_contacts[pair];

				contact.contacts[i].vA = m_velocities[pair.objectIdA] + Vector2::crossProduct(m_angularVelocities[pair.objectIdA], contact.contacts[i].rA);
				contact.contacts[i].vB = m_velocities[pair.objectIdB] + Vector2::crossProduct(m_angularVelocities[pair.objectIdB], contact.contacts[i].rB);
				Vector2 dv = contact.contacts[i].vA - contact.contacts[i].vB;
				real jvt = contact.tangent.dot(dv);
				real lambdaT = -jvt * contact.contacts[i].effectiveMassTangent;

				real maxFriction = contact.friction * contact.contacts[i].sumTangentImpulse;
				real newImpulse = Math::clamp(contact.contacts[i].sumTangentImpulse + lambdaT, -maxFriction, maxFriction);
				lambdaT = newImpulse - contact.contacts[i].sumTangentImpulse;
				contact.contacts[i].sumTangentImpulse = newImpulse;

				Vector2 impulseT = contact.tangent * lambdaT;

				// then solve normal
			}
		}
	}

	void PhysicsScene::processPosition(const ObjectPair& pair)
	{
		if (m_contacts[pair].count == 2 && m_enablePositionBlockSolver)
		{
			// solve block
		}
		else if (m_contacts[pair].count > 0)
		{
			// solve one by one
			for (int i = 0; i < m_contacts[pair].count; ++i)
			{
				m_contacts[pair].contacts[i].velocityBias = 0.0f;
			}
		}
	}
}


