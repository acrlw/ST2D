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
		land.set(200.0f, 0.1f);
		capsule.set(1.0f, 2.0f);
		ellipse.set(1.0f, 2.0f);
		circle.setRadius(0.25f);
		edge.set(Vector2(-10.0f, -0.5f), Vector2(10.0f, -0.5f));
		polygon.set({ Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f), Vector2(1.0f, 1.0f) });

		createObjects();
	}

	void PhysicsScene::onUnLoad()
	{
		clearObjects();
	}

	void PhysicsScene::onUpdate(float deltaTime)
	{
		ZoneScopedN("[PhysicsScene] On Update");

		if (m_simulate)
		{
			float dt = 1.0f / static_cast<float>(m_frequency);
			step(dt);
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
				renderer.aabb(m_aabbs[i], Palette::Teal);

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

		if(m_showGraphColoring && !m_objectGraph.m_colorToEdges.empty())
		{
			float max = static_cast<float>(m_objectGraph.m_colorToEdges.size());
			for (int i = 0; i < m_objectGraph.m_colorToEdges.size(); ++i)
			{
				float value = static_cast<float>(i) / max;
				Color color = gistRainbowColormap(value);
				for (auto&& elem : m_objectGraph.m_colorToEdges[i])
				{
					if(m_contacts[elem].count > 0)
					{
						renderer.point(m_contacts[elem].pair.points[0], color, 5);
						renderer.point(m_contacts[elem].pair.points[2], color, 5);
						if(m_contacts[elem].count == 2)
						{
							renderer.point(m_contacts[elem].pair.points[1], color, 5);
							renderer.point(m_contacts[elem].pair.points[3], color, 5);
						}
					}
				}
			}

		}

	}


	void PhysicsScene::onRenderUI()
	{
		ImGui::Begin("Scene Settings");

		int count = m_count;
		ImGui::DragInt("Count", &count, 1, 1, 5000);
		if (m_count != count)
		{
			m_count = count;
			createObjects();
		}

		ImGui::SeparatorText("Physics");

		ImGui::Checkbox("Simulate", &m_simulate);

		ImGui::SameLine();

		if (ImGui::Button("Step"))
		{
			m_simulate = false;
			step(1.0f / static_cast<float>(m_frequency));
		}

		ImGui::SliderInt("Frequency", &m_frequency, 1, 240);
		ImGui::SliderInt("Vel Iteration", &m_solveVelocityCount, 1, 100);
		ImGui::SliderInt("Pos Iteration", &m_solvePositionCount, 1, 100);

		ImGui::Columns(2);
		ImGui::Checkbox("Gravity", &m_enableGravity);
		ImGui::Checkbox("Damping", &m_enableDamping);
		ImGui::Checkbox("Warmstart", &m_enableWarmstart);
		ImGui::NextColumn();
		ImGui::Checkbox("Vel Block Solver", &m_enableVelocityBlockSolver);
		ImGui::Checkbox("Pos Block Solver", &m_enablePositionBlockSolver);
		ImGui::Columns(1);

		ImGui::DragFloat("Bias Factor", &m_biasFactor, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("Slop", &m_slop, 0.001f, 0.005f, 1.0f);

		ImGui::SeparatorText("Visibility");

		ImGui::Columns(2);
		ImGui::Checkbox("Show Object", &m_showObject);
		ImGui::Checkbox("Show Object Id", &m_showObjectID);
		ImGui::Checkbox("Show Transform", &m_showTransform);
		ImGui::Checkbox("Show Joint", &m_showJoint);
		ImGui::NextColumn();

		ImGui::Checkbox("Show AABB", &m_showAABB);
		ImGui::Checkbox("Show Grid", &m_showGrid);
		ImGui::Checkbox("Show DBVT", &m_showDBVT);
		ImGui::Checkbox("Show Graph Coloring", &m_showGraphColoring);

		ImGui::Columns(1);

		ImGui::Checkbox("Show Contacts", &m_showContacts);
		ImGui::Checkbox("Show Contacts Magnitude", &m_showContactsMagnitude);

		ImGui::DragFloat("Expand Ratio", &m_expandRatio, 0.01f, 0.0f, 1.0f);

		ImGui::SeparatorText("Debug Text");
		ImGui::Text("Graph Color Size: %d", m_objectGraph.m_colorToEdges.size());

		ImGui::End();
	}

	void PhysicsScene::step(float dt)
	{
		if (m_flagInitial)
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
					m_frictions.emplace_back(0.8f);

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
			trans.position.set(0.0f, -0.05);

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

				const real imA = m_invMasses[key.objectIdA];
				const real imB = m_invMasses[key.objectIdB];
				const real iiA = m_invInertias[key.objectIdA];
				const real iiB = m_invInertias[key.objectIdB];

				for(int i = 0; i < value.count; ++i)
				{
					auto& contact = value.contacts[i];

					contact.localA = transformA.inverseTranslatePoint(value.pair.points[i]);
					contact.localB = transformB.inverseTranslatePoint(value.pair.points[i + 2]);
					contact.rA = transformA.position - value.pair.points[i];
					contact.rB = transformB.position - value.pair.points[i + 2];
					contact.penetration = (value.pair.points[i] - value.pair.points[i + 2]).length();

					const real rnA = contact.rA.cross(value.normal);
					const real rnB = contact.rB.cross(value.normal);
					const real rtA = contact.rA.cross(value.tangent);
					const real rtB = contact.rB.cross(value.tangent);

					const real kNormal = imA + iiA * rnA * rnA +
						imB + iiB * rnB * rnB;

					const real kTangent = imA + iiA * rtA * rtA +
						imB + iiB * rtB * rtB;

					contact.effectiveMassNormal = realEqual(kNormal, 0.0f) ? 0 : 1.0f / kNormal;
					contact.effectiveMassTangent = realEqual(kTangent, 0.0f) ? 0 : 1.0f / kTangent;

					if(m_enableWarmstart)
					{
						Vector2 impulse = contact.sumNormalImpulse * value.normal + contact.sumTangentImpulse * value.tangent;

						m_velocities[key.objectIdA] += imA * impulse;
						m_angularVelocities[key.objectIdA] += iiA * contact.rA.cross(impulse);
						m_velocities[key.objectIdB] -= imB * impulse;
						m_angularVelocities[key.objectIdB] -= iiB * contact.rB.cross(impulse);
					}

					Vector2 wa = Vector2::crossProduct(m_angularVelocities[key.objectIdA], contact.rA);
					Vector2 wb = Vector2::crossProduct(m_angularVelocities[key.objectIdB], contact.rB);
					contact.vA = m_velocities[key.objectIdA] + wa;
					contact.vB = m_velocities[key.objectIdB] + wb;

				}

				if(value.count == 2 && m_enableVelocityBlockSolver)
				{

					real rn1A = value.contacts[0].rA.cross(value.normal);
					real rn1B = value.contacts[0].rB.cross(value.normal);
					real rn2A = value.contacts[1].rA.cross(value.normal);
					real rn2B = value.contacts[1].rB.cross(value.normal);

					real k11 = imA + iiA * rn1A * rn1A + imB + iiB * rn1B * rn1B;
					real k12 = imA + iiA * rn1A * rn2A + imB + iiB * rn1B * rn2B;
					real k22 = imA + iiA * rn2A * rn2A + imB + iiB * rn2B * rn2B;

					bool conditioner = k11 * k11 < 1000.0f * (k11 * k22 - k12 * k12);

					//numerical stability check to ensure invertible matrix
					if (conditioner)
					{
						value.k.set(k11, k12, k12, k22);
						value.normalMass.set(k11, k12, k12, k22);
						value.normalMass.invert();
					}

				}

			}
		}

	}

	void PhysicsScene::processVelocity(const ObjectPair& pair)
	{
		if(m_contacts[pair].count > 0)
		{
			// solve one by one
			auto& contact = m_contacts[pair];

			for(int i = 0;i < m_contacts[pair].count; ++i)
			{
				// solve friction first
				auto& singleContact = contact.contacts[i];
				singleContact.vA = m_velocities[pair.objectIdA] + Vector2::crossProduct(m_angularVelocities[pair.objectIdA], singleContact.rA);
				singleContact.vB = m_velocities[pair.objectIdB] + Vector2::crossProduct(m_angularVelocities[pair.objectIdB], singleContact.rB);
				Vector2 dv = singleContact.vA - singleContact.vB;
				real jvt = contact.tangent.dot(dv);
				real lambdaT = -jvt * singleContact.effectiveMassTangent;

				real maxFriction = contact.friction * singleContact.sumTangentImpulse;
				real newImpulse = Math::clamp(singleContact.sumTangentImpulse + lambdaT, -maxFriction, maxFriction);
				lambdaT = newImpulse - singleContact.sumTangentImpulse;
				singleContact.sumTangentImpulse = newImpulse;

				Vector2 impulseT = contact.tangent * lambdaT;

				m_velocities[pair.objectIdA] += impulseT * m_invMasses[pair.objectIdA];
				m_angularVelocities[pair.objectIdA] += m_invInertias[pair.objectIdA] * singleContact.rA.cross(impulseT);

				m_velocities[pair.objectIdB] -= impulseT * m_invMasses[pair.objectIdB];
				m_angularVelocities[pair.objectIdB] -= m_invInertias[pair.objectIdB] * singleContact.rB.cross(impulseT);

				// then solve contact
			}

			if (m_contacts[pair].count == 2 && m_enableVelocityBlockSolver)
			{
				// solve by block
				Vector2 wA1 = Vector2::crossProduct(m_angularVelocities[pair.objectIdA], contact.contacts[0].rA);
				Vector2 wB1 = Vector2::crossProduct(m_angularVelocities[pair.objectIdB], contact.contacts[0].rB);
				Vector2 wA2 = Vector2::crossProduct(m_angularVelocities[pair.objectIdA], contact.contacts[1].rA);
				Vector2 wB2 = Vector2::crossProduct(m_angularVelocities[pair.objectIdB], contact.contacts[1].rB);
					    
				Vector2 vA1 = m_velocities[pair.objectIdA] + wA1;
				Vector2 vB1 = m_velocities[pair.objectIdB] + wB1;
				Vector2 vA2 = m_velocities[pair.objectIdA] + wA2;
				Vector2 vB2 = m_velocities[pair.objectIdB] + wB2;

				Vector2 dv1 = vA1 - vB1;
				Vector2 dv2 = vA2 - vB2;
				real jv1 = contact.normal.dot(dv1 - contact.contacts[0].velocityBias);
				real jv2 = contact.normal.dot(dv2 - contact.contacts[1].velocityBias);

				Matrix2x2& A = contact.k;
				Vector2 b(jv1, jv2);
				Vector2 x(contact.contacts[0].sumNormalImpulse, contact.contacts[1].sumNormalImpulse);
				Vector2 nx;
				Vector2 d;
				b = b - A.multiply(x);

				for (;;)
				{
					//1. b_1 < 0 && b_2 < 0
					nx = contact.normalMass.multiply(-b);
					if (nx.x >= 0.0f && nx.y >= 0.0f)
						break;

					//2. b_1 < 0 && b_2 > 0
					nx.x = contact.contacts[0].effectiveMassNormal * -b.x;
					nx.y = 0.0f;
					jv1 = 0.0f;
					jv2 = A.m12 * nx.x + b.y;
					if (nx.x >= 0.0f && jv2 >= 0.0f)
						break;

					//3. b_1 > 0 && b_2 < 0
					nx.x = 0.0f;
					nx.y = -contact.contacts[1].effectiveMassNormal * b.y;
					jv1 = A.m21 * nx.y + b.x;
					jv2 = 0.0f;
					if (nx.y >= 0.0f && jv1 >= 0.0f)
						break;

					//4. b_1 > 0 && b_2 > 0
					nx.clear();
					jv1 = b.x;
					jv2 = b.y;
					if (jv1 >= 0.0f && jv2 >= 0.0f)
						break;

					//hit the unknown cases
					break;
				}

				d = nx - x;

				real& lambda1 = d.x;
				real& lambda2 = d.y;

				Vector2 impulse1 = lambda1 * contact.normal;
				Vector2 impulse2 = lambda2 * contact.normal;

				m_velocities[pair.objectIdA] += (impulse1 + impulse2) * m_invMasses[pair.objectIdA];
				m_angularVelocities[pair.objectIdA] += m_invInertias[pair.objectIdA] * contact.contacts[0].rA.cross(impulse1 + impulse2);

				m_velocities[pair.objectIdB] -= (impulse1 + impulse2) * m_invMasses[pair.objectIdB];
				m_angularVelocities[pair.objectIdB] -= m_invInertias[pair.objectIdB] * contact.contacts[0].rB.cross(impulse1 + impulse2);

				contact.contacts[0].sumNormalImpulse = nx.x;
				contact.contacts[1].sumNormalImpulse = nx.y;
			}
			else
			{
				for (int i = 0; i < m_contacts[pair].count; ++i)
				{
					// solve friction first
					auto& singleContact = contact.contacts[i];
					singleContact.vA = m_velocities[pair.objectIdA] + Vector2::crossProduct(m_angularVelocities[pair.objectIdA], singleContact.rA);
					singleContact.vB = m_velocities[pair.objectIdB] + Vector2::crossProduct(m_angularVelocities[pair.objectIdB], singleContact.rB);
					Vector2 dv = singleContact.vA - singleContact.vB;
					real jvn = contact.normal.dot(dv + singleContact.velocityBias);
					real lamndaN = -jvn * singleContact.effectiveMassNormal;

					real oldImpulse = singleContact.sumNormalImpulse;
					singleContact.sumNormalImpulse = Math::max(oldImpulse + lamndaN, 0);
					lamndaN = singleContact.sumNormalImpulse - oldImpulse;

					Vector2 impulseN = lamndaN * contact.normal;

					m_velocities[pair.objectIdA] += impulseN * m_invMasses[pair.objectIdA];
					m_angularVelocities[pair.objectIdA] += m_invInertias[pair.objectIdA] * singleContact.rA.cross(impulseN);

					m_velocities[pair.objectIdB] -= impulseN * m_invMasses[pair.objectIdB];
					m_angularVelocities[pair.objectIdB] -= m_invInertias[pair.objectIdB] * singleContact.rB.cross(impulseN);

				}
			}

		}
	}

	void PhysicsScene::processPosition(const ObjectPair& pair)
	{
		auto& contact = m_contacts[pair];
		if (contact.count <= 0)
			return;

		Transform transformA(m_positions[pair.objectIdA], m_rotations[pair.objectIdA], 1.0f);
		Transform transformB(m_positions[pair.objectIdB], m_rotations[pair.objectIdB], 1.0f);

		const real imA = m_invMasses[pair.objectIdA];
		const real imB = m_invMasses[pair.objectIdB];
		const real iiA = m_invInertias[pair.objectIdA];
		const real iiB = m_invInertias[pair.objectIdB];

		if (contact.count == 2 && m_enablePositionBlockSolver)
		{
			// solve block

			Vector2 pA1 = transformA.translatePoint(contact.contacts[0].localA);
			Vector2 pB1 = transformB.translatePoint(contact.contacts[0].localB);

			Vector2 pA2 = transformA.translatePoint(contact.contacts[1].localA);
			Vector2 pB2 = transformB.translatePoint(contact.contacts[1].localB);

			Vector2 rA1 = pA1 - transformA.position;
			Vector2 rB1 = pB1 - transformB.position;

			Vector2 rA2 = pA2 - transformA.position;
			Vector2 rB2 = pB2 - transformB.position;

			Vector2 c1 = pA1 - pB1;
			Vector2 c2 = pB2 - pA2;

			real bias1 = Math::max(m_biasFactor * (c1.dot(contact.normal) - m_slop), 0.0f);
			real bias2 = Math::max(m_biasFactor * (c2.dot(contact.normal) - m_slop), 0.0f);

			const real bias = Math::min(bias1, bias2);

			bias1 = -bias;
			bias2 = -bias;


			real rn1A = rA1.cross(contact.normal);
			real rn1B = rB1.cross(contact.normal);
			real rn2A = rA2.cross(contact.normal);
			real rn2B = rB2.cross(contact.normal);

			real k11 = imA + iiA * rn1A * rn1A + imB + iiB * rn1B * rn1B;
			real k12 = imA + iiA * rn1A * rn2A + imB + iiB * rn1B * rn2B;
			real k22 = imA + iiA * rn2A * rn2A + imB + iiB * rn2B * rn2B;

			real determinant = (k11 * k22 - k12 * k12);
			real d1 = k11 * k11;
			real d2 = 1000.0f * (k11 * k22 - k12 * k12);
			bool conditioner = k11 * k11 < 1000.0f * (k11 * k22 - k12 * k12);

			//numerical stability check to ensure invertible matrix
			Matrix2x2 invA;
			if (conditioner)
			{
				invA.set(k11, k12, k12, k22);
				invA.invert();
			}
			else
				return;

			Vector2 b(bias1, bias2);
			Vector2 d;

			for (;;)
			{
				//1. b_1 < 0 && b_2 < 0
				Vector2 x = invA.multiply(-b);
				if (x.x >= 0.0f && x.y >= 0.0f)
				{
					d = x;
					break;
				}
				//2. b_1 < 0 && b_2 > 0
				x.x = -b.x / k11;
				x.y = 0.0f;
				bias2 = k12 * x.x + b.y;
				if (x.x >= 0.0f && bias2 >= 0.0f)
				{
					d = x;
					break;
				}

				//3. b_1 > 0 && b_2 < 0
				x.x = 0.0f;
				x.y = -b.y / k22;
				bias1 = k12 * x.y + b.x;
				if (x.y >= 0.0f && bias1 >= 0.0f)
				{
					d = x;
					break;
				}

				//4. b_1 > 0 && b_2 > 0
				//d = zero

				break;
			}

			Vector2 impulse1 = contact.normal * d.x;
			Vector2 impulse2 = contact.normal * d.y;

			m_positions[pair.objectIdA] += (impulse1 + impulse2) * imA;
			m_rotations[pair.objectIdA] += iiA * (rA1.cross(impulse1) + rA2.cross(impulse2));

			m_positions[pair.objectIdB] -= (impulse1 + impulse2) * imB;
			m_rotations[pair.objectIdB] -= iiB * (rB1.cross(impulse1) + rB2.cross(impulse2));


		}
		else
		{
			// solve one by one
			for (int i = 0; i < contact.count; ++i)
			{
				Vector2 pA = transformA.translatePoint(contact.contacts[i].localA);
				Vector2 pB = transformB.translatePoint(contact.contacts[i].localB);
				Vector2 rA = pA - transformA.position;
				Vector2 rB = pB - transformB.position;
				Vector2 c = pB - pA;

				const real bias = Math::max(m_biasFactor * (c.dot(contact.normal) - m_slop), 0.0f);


				const real rnA = rA.cross(contact.normal);
				const real rnB = rB.cross(contact.normal);

				const real kNormal = imA + iiA * rnA * rnA +
					imB + iiB * rnB * rnB;

				contact.contacts[i].effectiveMassNormal = realEqual(kNormal, 0.0f) ? 0 : 1.0f / kNormal;

				real lambda = contact.contacts[i].effectiveMassNormal * bias;
				lambda = Math::max(lambda, 0);

				Vector2 impulse = lambda * contact.normal;

				m_positions[pair.objectIdA] += impulse * imA;
				m_rotations[pair.objectIdA] += iiA * rA.cross(impulse);

				m_positions[pair.objectIdB] -= impulse * imB;
				m_rotations[pair.objectIdB] -= iiB * rB.cross(impulse);
			}
		}
	}
}


