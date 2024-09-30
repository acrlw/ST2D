#include "PhysicsScene.h"
#include "xmmintrin.h"
#include "immintrin.h"

namespace STEditor
{
	PhysicsScene::~PhysicsScene()
	{

	}

	void PhysicsScene::onLoad()
	{
		rect.set(1.0f, 1.0f);
		land.set(400.0f, 0.1f);
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
		if (m_simulate)
		{
			float dt = 1.0f / static_cast<float>(m_frequency);
			step(dt);
		}
	}

	void PhysicsScene::onRender(Renderer2D& renderer)
	{
		ZoneScopedN("[Scene] On Render");

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

			if(m_showVelocity)
			{
				Vector2 end = m_positions[i] + m_velocities[i];

				if(m_showVelocityMagnitude)
				{
					real mag = m_velocities[i].length();
					std::string str = std::format("Vel: {:.3f}", mag);
					renderer.text(end + Vector2(0.1f, 0.1f), Palette::LightCyan, str);
				}

				renderer.arrow(m_positions[i], end, Palette::LightCyan);
			}

			if(m_showAngularVelocity)
			{
				Vector2 end = m_positions[i] - Vector2(0.1f, 0.1f);
				std::string str = std::format("Ang Vel: {:.3f}", m_angularVelocities[i]);
				renderer.text(end, Palette::Teal, str);
			}
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
			for(auto& value : m_contactManifolds | std::views::values)
			{
				if (value.count == 0)
					continue;

				renderer.point(value.pair.points[0], Palette::LightRed);
				renderer.point(value.pair.points[2], Palette::LightBlue);

				real t = 0.05f;
				std::string str;

				if (m_showContactsMagnitude)
				{
					str = std::format("{:.5f}", value.contacts[0].sumNormalImpulse);
					renderer.text(value.pair.points[0] - Vector2(0.0f, t), Palette::LightCyan, str);
				}

				if (m_showContactNormal)
				{
					Vector2 end = value.pair.points[0] + value.normal * value.contacts[0].sumNormalImpulse;
					renderer.arrow(value.pair.points[0], end, Palette::LightCyan);
				}

				if (m_showFrictionMagnitude)
				{
					str = std::format("{:.5f}", value.contacts[0].sumTangentImpulse);
					renderer.text(value.pair.points[0] - Vector2(t, 0.0f), Palette::Yellow, str);
				}


				if (m_showFrictionNormal)
				{
					Vector2 end = value.pair.points[0] + value.tangent * value.contacts[0].sumTangentImpulse;
					renderer.arrow(value.pair.points[0], end, Palette::Yellow);
				}

				if (value.count == 2)
				{
					renderer.point(value.pair.points[1], Palette::LightRed);
					renderer.point(value.pair.points[3], Palette::LightBlue);

					if (m_showContactsMagnitude)
					{
						str = std::format("{:.5f}", value.contacts[1].sumNormalImpulse);
						renderer.text(value.pair.points[1] - Vector2(0.0f, t), Palette::LightCyan, str);
					}

					if (m_showContactNormal)
					{
						Vector2 end = value.pair.points[1] + value.normal * value.contacts[1].sumNormalImpulse;
						renderer.arrow(value.pair.points[1], end, Palette::LightCyan);
					}

					if (m_showFrictionMagnitude)
					{
						str = std::format("{:.5f}", value.contacts[1].sumTangentImpulse);
						renderer.text(value.pair.points[1] - Vector2(t, 0.0f), Palette::Yellow, str);
					}

					if(m_showFrictionNormal)
					{
						Vector2 end = value.pair.points[1] + value.tangent * value.contacts[1].sumTangentImpulse;
						renderer.arrow(value.pair.points[1], end, Palette::Yellow);
					}
				}

			}



		}

		if(m_showDBVTLeaf)
		{
			for (auto&& elem : m_dbvt.m_leaves)
			{
				if(elem.isValid())
				{
					renderer.aabb(elem.binding.aabb, Palette::Cyan);
				}
			}
		}

		if(m_showGridLeaf)
		{
			for (auto&& elem : m_grid.m_objects)
			{
				renderer.aabb(elem.binding.aabb, Palette::Orange);
			}
		}


		if(m_showGraphColoring && !m_objectGraph.m_colorToEdges.empty())
		{
			ZoneScopedN("Graph Coloring");
			float max = static_cast<float>(m_objectGraph.m_colorToEdges.size());
			for (int i = 0; i < m_objectGraph.m_colorToEdges.size(); ++i)
			{
				float value = static_cast<float>(i) / max;
				Color color = gistRainbowColormap(value);
				for (auto&& elem : m_objectGraph.m_colorToEdges[i])
				{
					if(m_contactManifolds[elem].count > 0)
					{
						renderer.point(m_contactManifolds[elem].pair.points[0], color, 4.0f);
						renderer.point(m_contactManifolds[elem].pair.points[2], color, 4.0f);
						if(m_contactManifolds[elem].count == 2)
						{
							renderer.point(m_contactManifolds[elem].pair.points[1], color, 4.0f);
							renderer.point(m_contactManifolds[elem].pair.points[3], color, 4.0f);
						}
					}
				}
			}
		}

		if(m_showContactOrder && !m_objectPairs.empty())
		{
			ZoneScopedN("Contact Order");
			float max = static_cast<float>(m_objectPairs.size());
			for (int i = 0; i < m_objectPairs.size(); ++i)
			{
				float value = static_cast<float>(i) / max;
				Color color = jetColorMap(value);
				auto& elem = m_objectPairs[i];
				if (m_contactManifolds[elem].count > 0)
				{
					renderer.point(m_contactManifolds[elem].pair.points[0], color, 3);
					renderer.point(m_contactManifolds[elem].pair.points[2], color, 3);
					if (m_contactManifolds[elem].count == 2)
					{
						renderer.point(m_contactManifolds[elem].pair.points[1], color, 3);
						renderer.point(m_contactManifolds[elem].pair.points[3], color, 3);
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

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			m_stepCount = 0;
			m_threadPool.stopAll();
			clearObjects();
			createObjects();
			m_threadPool.readyToWork();
		}



		if (ImGui::Button("Print Tree"))
		{
			m_dbvt.printTree();
		}

		ImGui::SameLine();

		if(ImGui::Button("Query DBVT"))
		{
			auto pairs = m_dbvt.queryOverlaps();
			
			std::string str;
			for (auto iter = pairs.begin(); iter != pairs.end(); ++iter)
			{
				str += std::format("({0}, {1})", iter->idA, iter->idB);
				if (iter != pairs.end() - 1)
					str += ", ";
			}
			CORE_INFO("DBVT Result: {}", str);
			CORE_INFO("DBVT Overlaps Count: {}", pairs.size());
		}

		ImGui::SameLine();

		if (ImGui::Button("Query Grid"))
		{
			auto pairs = m_grid.queryOverlaps();
			
			std::string str;
			for (auto iter = pairs.begin(); iter != pairs.end(); ++iter)
			{
				str += std::format("({0}, {1})", iter->idA, iter->idB);
				if (iter != pairs.end() - 1)
					str += ", ";
			}
			CORE_INFO("Grid Result: {}", str);
			CORE_INFO("Grid Overlaps Count: {}", pairs.size());
		}

		ImGui::SameLine();

		if (ImGui::Button("Print Graph"))
		{
			m_objectGraph.buildIsland();
			m_objectGraph.printGraph();
		}

		ImGui::Text("Step Count: %d, Frame Time: %f", m_stepCount, m_currentFrameTime);

		ImGui::DragInt("Frequency", &m_frequency, 1, 240);
		ImGui::DragInt("Vel Iteration", &m_solveVelocityCount, 1, 100);
		ImGui::DragInt("Pos Iteration", &m_solvePositionCount, 1, 100);

		ImGui::Columns(2);
		ImGui::Checkbox("Gravity", &m_enableGravity);
		ImGui::Checkbox("Damping", &m_enableDamping);
		ImGui::Checkbox("Warmstart", &m_enableWarmstart);
		ImGui::Checkbox("Use Graph Coloring", &m_solveByGraphColoring);
		ImGui::Checkbox("Coloring", &m_enableGraphColoring);
		ImGui::Checkbox("DBVT Rebuild", &m_enableDBVTRebuild);
		ImGui::NextColumn();
		ImGui::Checkbox("Vel Block Solver", &m_enableVelocityBlockSolver);
		ImGui::Checkbox("Pos Block Solver", &m_enablePositionBlockSolver);
		ImGui::Checkbox("Parallel Process", &m_parallelProcessing);
		ImGui::Checkbox("Use SIMD", &m_useSIMD);
		ImGui::Checkbox("Enable Grid", &m_enableGrid);
		ImGui::Checkbox("Enable DBVT", &m_enableDBVT);
		ImGui::Columns(1);

		ImGui::Combo("Broadphase", &m_currentBroadphaseIndex, "DBVT\0Grid");

		ImGui::Checkbox("Show DBVT Leaf", &m_showDBVTLeaf);
		ImGui::SameLine();
		ImGui::Checkbox("Show Grid Leaf", &m_showGridLeaf);

		ImGui::DragFloat("Bias Factor", &m_biasFactor, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("Slop", &m_slop, 0.001f, 0.005f, 1.0f);

		ImGui::DragFloat("Linear Damp", &m_linearVelocityDamping, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Angular Damp", &m_angularVelocityDamping, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("AABB Expand Ratio", &m_expandRatio, 0.01f, 0.0f, 1.0f);

		ImGui::SeparatorText("Visibility");

		ImGui::Columns(2);
		ImGui::Checkbox("Object", &m_showObject);
		ImGui::Checkbox("Object Id", &m_showObjectID);
		ImGui::Checkbox("Transform", &m_showTransform);
		ImGui::Checkbox("AABB", &m_showAABB);
		ImGui::Checkbox("Grid", &m_showGrid);
		ImGui::Checkbox("DBVT", &m_showDBVT);
		ImGui::Checkbox("Graph Coloring", &m_showGraphColoring);
		ImGui::Checkbox("Contact  Order", &m_showContactOrder);
		ImGui::NextColumn();
		ImGui::Checkbox("Velocity", &m_showVelocity);
		ImGui::Checkbox("Velocity Mag", &m_showVelocityMagnitude);
		ImGui::Checkbox("Angular Vel", &m_showAngularVelocity);
		ImGui::Checkbox("Joint", &m_showJoint);
		ImGui::Checkbox("Contact", &m_showContacts);
		ImGui::Checkbox("Contact Mag", &m_showContactsMagnitude);
		ImGui::Checkbox("Contact Normal", &m_showContactNormal);
		ImGui::Checkbox("Friction Mag", &m_showFrictionMagnitude);
		ImGui::Checkbox("Friction Normal", &m_showFrictionNormal);
		ImGui::Columns(1);


		ImGui::SeparatorText("Debug Text");
		ImGui::Text("Graph Color Size: %d", m_objectGraph.m_colorToEdges.size());

		ImGui::End();
	}

	void PhysicsScene::step(float dt)
	{
		ZoneScopedN("[Scene] Step");

		if (m_flagInitial)
		{
			m_flagInitial = false;

			generateAndColorContacts(dt);
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

		for (auto& value : m_contactManifolds | std::views::values)
			value.enable = false;

		// update broad phase
		updateBroadphase(dt);

		// update narrow phase and generate contacts
		generateAndColorContacts(dt);

		for (auto& value : m_contactManifolds | std::views::values)
		{
			if (!value.enable)
			{
				value.count = 0;
			}
		}

		// clear all force and torque

		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			m_forces[i].clear();
			m_torques[i] = 0.0f;
		}

		m_stepCount++;
		double currentTime = glfwGetTime();
		m_currentFrameTime = (currentTime - m_stepLastFrameTime) * 1000.0;
		m_stepLastFrameTime = currentTime;
	}

	real PhysicsScene::naturalFrequency(real frequency)
	{
		return Constant::DoublePi * frequency;
	}

	real PhysicsScene::springDampingCoefficient(real mass, real naturalFrequency, real dampingRatio)
	{
		return dampingRatio * 2.0f * mass * naturalFrequency;
	}

	real PhysicsScene::springStiffness(real mass, real naturalFrequency)
	{
		return mass * naturalFrequency * naturalFrequency;
	}

	real PhysicsScene::constraintImpulseMixing(real dt, real stiffness, real damping)
	{
		real cim = dt * (dt * stiffness + damping);
		return realEqual(cim, 0.0f) ? 0.0f : 1.0f / cim;
	}

	real PhysicsScene::errorReductionParameter(real dt, real stiffness, real damping)
	{
		real erp = dt * stiffness + damping;
		return realEqual(erp, 0.0f) ? 0.0f : stiffness / erp;
	}

	void PhysicsScene::createObjects()
	{
		clearObjects();

		std::vector<BroadphaseObjectBinding> bindings;
		bindings.reserve(m_count);

		{
			ZoneScopedN("[Scene] Create and add shape to list");

			real offset = 1.0f;
			real max = static_cast<real>(m_count);

			for (real j = 0; j < max; j += 1.0f)
			{
				for (real i = 0.0; i < max - j; i += 1.0f)
				{
					Transform t;
					t.position.set( i * 1.11f + offset, j * 1.05f + 0.6f);
					t.rotation = 0;

					auto id = m_objectIdPool.getNewId();
					m_objectIds.push_back(id);

					m_bitmasks.push_back(1);
					m_shapes.push_back(&rect);
					m_aabbs.push_back(AABB::fromShape(t, m_shapes.back()));

					m_positions.emplace_back(t.position);
					m_rotations.emplace_back(t.rotation);
					m_velocities.emplace_back();
					m_angularVelocities.emplace_back(0.0f);
					m_forces.emplace_back(0.0f);
					m_torques.emplace_back(0.0f);
					m_masses.emplace_back(1.0f);
					real inertia = computeInertia(1.0f, m_shapes.back());
					real invInertia = inertia > 0.0f ? 1.0f / inertia : 0.0f;
					m_inertias.emplace_back(inertia);
					m_invMasses.emplace_back(1.0f);
					m_invInertias.emplace_back(invInertia);
					m_restitutions.emplace_back(0.0f);
					m_frictions.emplace_back(1.0f);


					bindings.emplace_back(m_objectIds.back(), 1, m_aabbs.back());

				}
				offset += 0.5f;
			}
		}
		{

			Transform trans;
			trans.position.set(0.0f, -0.05f);

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
			m_objectGraph.addEnableColorRepeated(m_landId);
		}

		if(m_enableGrid)
		{
			ZoneScopedN("[Scene] Add to Grid");
			for (auto&& binding : bindings)
				m_grid.addObject(binding);
		}

		{
			ZoneScopedN("[Scene] Add to DBVT");
			for (auto&& binding : bindings)
				m_dbvt.addObject(binding);
		}


	}

	void PhysicsScene::clearObjects()
	{
		m_objectPairs.clear();
		m_dbvt.clearAllObjects();
		m_grid.clearAllObjects();
		m_contactManifolds.clear();
		m_objectGraph.clearGraph();

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

	void PhysicsScene::generateAndColorContacts(float dt)
	{
		ZoneScopedN("Generate and Color Contacts");

		if(m_currentBroadphaseIndex == 0)
			m_objectPairs = m_dbvt.queryOverlaps();
		else if(m_currentBroadphaseIndex == 1)
			m_objectPairs = m_grid.queryOverlaps();

		{
			ZoneScopedN("Contacts Generation");

			// narrow phase generate contacts

			for (auto&& objPair : m_objectPairs)
			{
				if (!m_aabbs[objPair.idA].collide(m_aabbs[objPair.idB]))
					continue;

				Transform transformA(m_positions[objPair.idA], m_rotations[objPair.idA], 1.0f);
				Transform transformB(m_positions[objPair.idB], m_rotations[objPair.idB], 1.0f);

				const bool isRoundA = m_shapes[objPair.idA]->type() == ShapeType::Circle || m_shapes[objPair.idA]->type() == ShapeType::Ellipse;
				const bool isRoundB = m_shapes[objPair.idB]->type() == ShapeType::Circle || m_shapes[objPair.idB]->type() == ShapeType::Ellipse;

				auto simplex = Narrowphase::gjk(transformA, m_shapes[objPair.idA], transformB, m_shapes[objPair.idB]);
				if (simplex.isContainOrigin)
				{
					auto info = Narrowphase::epa(simplex, transformA, m_shapes[objPair.idA], transformB, m_shapes[objPair.idB]);
					auto newContacts = Narrowphase::generateContacts(info, transformA, m_shapes[objPair.idA], transformB, m_shapes[objPair.idB]);

					ContactManifold manifold;
					manifold.count = newContacts.count;
					manifold.pair = newContacts;
					manifold.normal = info.normal;
					manifold.tangent = info.normal.perpendicular();
					manifold.penetration = info.penetration;

					if (m_contactManifolds.contains(objPair))
					{
						for (int j = 0; j < manifold.pair.count; ++j)
						{
							Vector2 newLocalA = transformA.inverseTranslatePoint(manifold.pair.points[j]);
							Vector2 newLocalB = transformB.inverseTranslatePoint(manifold.pair.points[j + 2]);
							for (int i = 0; i < m_contactManifolds[objPair].count; ++i)
							{
								Vector2 oldLocalA = m_contactManifolds[objPair].contacts[i].localA;
								Vector2 oldLocalB = m_contactManifolds[objPair].contacts[i].localB;
								const bool isPointA = oldLocalA.fuzzyEqual(newLocalA, Constant::TrignometryEpsilon);
								const bool isPointB = oldLocalB.fuzzyEqual(newLocalB, Constant::TrignometryEpsilon);
								if (isPointA || isPointB || isRoundA || isRoundB)
								{
									//satisfy the condition, give the old accumulated value to new value
									manifold.contacts[j] = m_contactManifolds[objPair].contacts[i];
								}
							}
						}
					}

					m_contactManifolds[objPair] = manifold;
				}
			}
		}

		// graph coloring
		if(m_enableGraphColoring)
		{
			ZoneScopedN("Graph Coloring");

			m_objectGraph.incrementalUpdateEdges(m_objectPairs);
			m_objectGraph.outputColorResult();

			//m_objectGraph.clearGraph();
			//m_objectGraph.addEnableColorRepeated(m_landId);
			//m_objectGraph.buildGraph(m_objectPairs);
			//m_objectGraph.outputColorResult();
		}
	}

	void PhysicsScene::updateBroadphase(float dt)
	{
		ZoneScopedN("Update Broadphase");

		if(m_enableDBVT)
		{
			if(m_enableDBVTRebuild)
			{
				m_dbvt.clearAllObjects();
				m_dbvt.setOnlyInsert(true);
			}
			else
				m_dbvt.setOnlyInsert(false);
		}

		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			m_aabbs[i] = AABB::fromShape(Transform(m_positions[i], m_rotations[i], 1.0f), m_shapes[i]);
			BroadphaseObjectBinding binding(m_objectIds[i], m_bitmasks[i], m_aabbs[i]);

			if(m_enableDBVT)
			{
				if (m_enableDBVTRebuild)
					m_dbvt.addObject(binding);
				else
					m_dbvt.updateObject(binding);
			}

			if (m_enableGrid)
				m_grid.updateObject(binding);
		}
		if(m_enableDBVT && m_enableDBVTRebuild)
			m_dbvt.rebuildTree();
	}

	void PhysicsScene::integrateVelocities(float dt)
	{
		ZoneScopedN("Integrate Velocities");
		Vector2 gravity;
		if (m_enableGravity)
			gravity.set(0.0f, -10.0f);

		real lvd = 1.0f;
		real avd = 1.0f;
		if (m_enableDamping)
		{
			lvd = 1.0f / (1.0f + dt * m_linearVelocityDamping);
			avd = 1.0f / (1.0f + dt * m_angularVelocityDamping);
		}

		// parallel

		if(m_useSIMD)
		{
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
				m_velocities[index] += m_invMasses[index] * m_forces[index] * dt;
				m_angularVelocities[index] +=  m_invInertias[index] * m_torques[index] * dt;

				m_velocities[index] *= lvd;
				m_angularVelocities[index] *= avd;
			}
		}
		else
		{
			for (int i = 0; i < m_objectIds.size(); ++i)
			{
				m_forces[i] += gravity * m_masses[i];
				m_velocities[i] += m_forces[i] * m_invMasses[i] * dt;
				m_angularVelocities[i] += m_torques[i] * m_invInertias[i] * dt;

				m_velocities[i] *= lvd;
				m_angularVelocities[i] *= avd;
			}
		}

	}

	void PhysicsScene::solveVelocities(float dt)
	{
		ZoneScopedN("Solve Velocity");

		for (int solveIndex = 0; solveIndex < m_solveVelocityCount; ++solveIndex)
		{
			//parallel
			if (m_parallelProcessing)
			{
				for (auto&& elem : m_objectGraph.m_colorToEdges)
				{
					std::vector<std::future<void>> futures;

					for (auto&& pair : elem)
					{
						futures.emplace_back(
							m_threadPool.enqueue([this, pair, dt]
								{
									solveContactVelocity(pair);
								}));

					}

					for (auto& future : futures)
						future.get();
				}
			}
			else
			{
				if(m_solveByGraphColoring)
				{
					for (auto&& elem : m_objectGraph.m_colorToEdges)
						for (auto&& pair : elem)
							solveContactVelocity(pair);
				}
				else
				{
					for (auto&& elem : m_objectPairs)
						solveContactVelocity(elem);
				}
			}

			solveJointVelocity(dt);


		}


	}

	void PhysicsScene::integratePositions(float dt)
	{
		ZoneScopedN("Integrate Positions");

		if (m_useSIMD)
		{
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
		else
		{
			for (int i = 0; i < m_objectIds.size(); ++i)
			{
				m_positions[i] += m_velocities[i] * dt;
				m_rotations[i] += m_angularVelocities[i] * dt;
			}
		}
	}

	void PhysicsScene::solvePositions(float dt)
	{
		ZoneScopedN("Solve Position");
		for (int solveIndex = 0; solveIndex < m_solvePositionCount; ++solveIndex)
		{
			if(m_parallelProcessing)
			{
				for (auto&& elem : m_objectGraph.m_colorToEdges)
				{
					std::vector<std::future<void>> futures;

					for (auto&& pair : elem)
					{
						futures.emplace_back(
							m_threadPool.enqueue([this, pair, dt]
								{
									solveContactPosition(pair);
								}));

					}

					for (auto& future : futures)
						future.get();
				}
			}
			else
			{
				if (m_solveByGraphColoring)
				{
					for (auto&& elem : m_objectGraph.m_colorToEdges)
						for (auto&& pair : elem)
							solveContactPosition(pair);
				}
				else
				{
					for (auto&& elem : m_objectPairs)
						solveContactPosition(elem);
				}
			}

			solveJointPosition(dt);
		}
	}

	void PhysicsScene::setUpConstraint(float dt)
	{
		ZoneScopedN("Setup Contact Constraint");
		for(auto&& [key ,value] : m_contactManifolds)
		{
			if(value.count > 0)
			{
				Transform transformA(m_positions[key.idA], m_rotations[key.idA], 1.0f);
				Transform transformB(m_positions[key.idB], m_rotations[key.idB], 1.0f);

				value.restitution = Math::min(m_restitutions[key.idA], m_restitutions[key.idB]);
				value.friction = Math::sqrt(m_frictions[key.idA] * m_frictions[key.idB]);

				const real imA = m_invMasses[key.idA];
				const real imB = m_invMasses[key.idB];
				const real iiA = m_invInertias[key.idA];
				const real iiB = m_invInertias[key.idB];

				for(int i = 0; i < value.count; ++i)
				{
					auto& contact = value.contacts[i];

					contact.localA = transformA.inverseTranslatePoint(value.pair.points[i]);
					contact.localB = transformB.inverseTranslatePoint(value.pair.points[i + 2]);
					contact.rA = value.pair.points[i] - transformA.position;
					contact.rB = value.pair.points[i + 2] - transformB.position;
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

						m_velocities[key.idA] += imA * impulse;
						m_angularVelocities[key.idA] += iiA * contact.rA.cross(impulse);

						m_velocities[key.idB] += imB * -impulse;
						m_angularVelocities[key.idB] += iiB * contact.rB.cross(-impulse);
					}

					Vector2 wa = Vector2::crossProduct(m_angularVelocities[key.idA], contact.rA);
					Vector2 wb = Vector2::crossProduct(m_angularVelocities[key.idB], contact.rB);
					contact.vA = m_velocities[key.idA] + wa;
					contact.vB = m_velocities[key.idB] + wb;

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

					//numerical stability check to ensure invertible matrix
					bool conditioner = k11 * k11 < 1000.0f * (k11 * k22 - k12 * k12);

					value.k.set(k11, k12, k12, k22);
					value.normalMass.set(k11, k12, k12, k22);
					value.normalMass.invert();

				}

			}
		}

	}

	void PhysicsScene::solveContactVelocity(const ObjectPair& pair)
	{
		if(m_contactManifolds[pair].count > 0)
		{
			// solve one by one
			auto& manifold = m_contactManifolds[pair];

			for(int i = 0;i < m_contactManifolds[pair].count; ++i)
			{
				// solve friction first

				auto& singleContact = manifold.contacts[i];
				singleContact.vA = m_velocities[pair.idA] + Vector2::crossProduct(m_angularVelocities[pair.idA], singleContact.rA);
				singleContact.vB = m_velocities[pair.idB] + Vector2::crossProduct(m_angularVelocities[pair.idB], singleContact.rB);
				Vector2 dv = singleContact.vA - singleContact.vB;

				real jvt = manifold.tangent.dot(dv);
				real lambdaT = singleContact.effectiveMassTangent * -jvt;

				real maxFriction = manifold.friction * singleContact.sumNormalImpulse;
				real newImpulse = Math::clamp(singleContact.sumTangentImpulse + lambdaT, -maxFriction, maxFriction);
				lambdaT = newImpulse - singleContact.sumTangentImpulse;
				singleContact.sumTangentImpulse = newImpulse;

				Vector2 impulseT =  lambdaT * manifold.tangent;

				m_velocities[pair.idA] += impulseT * m_invMasses[pair.idA];
				m_angularVelocities[pair.idA] += m_invInertias[pair.idA] * singleContact.rA.cross(impulseT);

				m_velocities[pair.idB] += (-impulseT) * m_invMasses[pair.idB];
				m_angularVelocities[pair.idB] += m_invInertias[pair.idB] * singleContact.rB.cross(-impulseT);

				// then solve contact
			}

			if (m_contactManifolds[pair].count == 2 && m_enableVelocityBlockSolver)
			{
				// solve by block
				Vector2 wA1 = Vector2::crossProduct(m_angularVelocities[pair.idA], manifold.contacts[0].rA);
				Vector2 wB1 = Vector2::crossProduct(m_angularVelocities[pair.idB], manifold.contacts[0].rB);
				Vector2 wA2 = Vector2::crossProduct(m_angularVelocities[pair.idA], manifold.contacts[1].rA);
				Vector2 wB2 = Vector2::crossProduct(m_angularVelocities[pair.idB], manifold.contacts[1].rB);
					    
				Vector2 vA1 = m_velocities[pair.idA] + wA1;
				Vector2 vB1 = m_velocities[pair.idB] + wB1;
				Vector2 vA2 = m_velocities[pair.idA] + wA2;
				Vector2 vB2 = m_velocities[pair.idB] + wB2;

				Vector2 dv1 = vA1 - vB1;
				Vector2 dv2 = vA2 - vB2;
				real jv1 = manifold.normal.dot(dv1 - manifold.contacts[0].velocityBias);
				real jv2 = manifold.normal.dot(dv2 - manifold.contacts[1].velocityBias);

				Matrix2x2& A = manifold.k;
				Vector2 b(jv1, jv2);
				Vector2 x(manifold.contacts[0].sumNormalImpulse, manifold.contacts[1].sumNormalImpulse);
				Vector2 nx;
				Vector2 d;

				b = b - A.multiply(x);

				for (;;)
				{
					//1. b_1 < 0 && b_2 < 0
					nx = manifold.normalMass.multiply(-b);
					if (nx.x >= 0.0f && nx.y >= 0.0f)
						break;

					//2. b_1 < 0 && b_2 > 0
					nx.x = manifold.contacts[0].effectiveMassNormal * -b.x;
					nx.y = 0.0f;
					jv1 = 0.0f;
					jv2 = A.m12 * nx.x + b.y;
					if (nx.x >= 0.0f && jv2 >= 0.0f)
						break;

					//3. b_1 > 0 && b_2 < 0
					nx.x = 0.0f;
					nx.y = -manifold.contacts[1].effectiveMassNormal * b.y;
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

				Vector2 impulse1 = lambda1 * manifold.normal;
				Vector2 impulse2 = lambda2 * manifold.normal;

				m_velocities[pair.idA] += (impulse1 + impulse2) * m_invMasses[pair.idA];
				m_angularVelocities[pair.idA] += m_invInertias[pair.idA] * (manifold.contacts[0].rA.cross(impulse1) + manifold.contacts[1].rA.cross(impulse2));

				m_velocities[pair.idB] += -(impulse1 + impulse2) * m_invMasses[pair.idB];
				m_angularVelocities[pair.idB] += m_invInertias[pair.idB] * (manifold.contacts[0].rB.cross(-impulse1) + manifold.contacts[1].rB.cross(-impulse2));

				manifold.contacts[0].sumNormalImpulse = nx.x;
				manifold.contacts[1].sumNormalImpulse = nx.y;
			}
			else
			{
				for (int i = 0; i < m_contactManifolds[pair].count; ++i)
				{
					// solve normal
					auto& singleContact = manifold.contacts[i];
					singleContact.vA = m_velocities[pair.idA] + Vector2::crossProduct(m_angularVelocities[pair.idA], singleContact.rA);
					singleContact.vB = m_velocities[pair.idB] + Vector2::crossProduct(m_angularVelocities[pair.idB], singleContact.rB);
					Vector2 dv = singleContact.vA - singleContact.vB;

					real jvn = manifold.normal.dot(dv + singleContact.velocityBias);
					real lamndaN = -jvn * singleContact.effectiveMassNormal;

					real oldImpulse = singleContact.sumNormalImpulse;
					singleContact.sumNormalImpulse = Math::max(oldImpulse + lamndaN, 0);
					lamndaN = singleContact.sumNormalImpulse - oldImpulse;

					Vector2 impulseN = lamndaN * manifold.normal;

					m_velocities[pair.idA] += impulseN * m_invMasses[pair.idA];
					m_angularVelocities[pair.idA] += m_invInertias[pair.idA] * singleContact.rA.cross(impulseN);

					m_velocities[pair.idB] -= impulseN * m_invMasses[pair.idB];
					m_angularVelocities[pair.idB] += m_invInertias[pair.idB] * singleContact.rB.cross(-impulseN);

				}
			}

		}
	}

	void PhysicsScene::solveContactPosition(const ObjectPair& pair)
	{
		auto& contact = m_contactManifolds[pair];
		if (contact.count <= 0)
			return;

		const real imA = m_invMasses[pair.idA];
		const real imB = m_invMasses[pair.idB];
		const real iiA = m_invInertias[pair.idA];
		const real iiB = m_invInertias[pair.idB];

		if (contact.count == 2 && m_enablePositionBlockSolver)
		{
			Transform transformA(m_positions[pair.idA], m_rotations[pair.idA], 1.0f);
			Transform transformB(m_positions[pair.idB], m_rotations[pair.idB], 1.0f);

			// solve block

			Vector2 pA1 = transformA.translatePoint(contact.contacts[0].localA);
			Vector2 pB1 = transformB.translatePoint(contact.contacts[0].localB);

			Vector2 pA2 = transformA.translatePoint(contact.contacts[1].localA);
			Vector2 pB2 = transformB.translatePoint(contact.contacts[1].localB);

			Vector2 rA1 = pA1 - transformA.position;
			Vector2 rB1 = pB1 - transformB.position;

			Vector2 rA2 = pA2 - transformA.position;
			Vector2 rB2 = pB2 - transformB.position;

			Vector2 c1 = pB1 - pA1;
			Vector2 c2 = pB2 - pA2;

			real bias1 = Math::max(m_biasFactor * (c1.dot(contact.normal) - m_slop), 0.0f);
			real bias2 = Math::max(m_biasFactor * (c2.dot(contact.normal) - m_slop), 0.0f);

			bias1 = -bias1;
			bias2 = -bias2;

			real rnA1 = rA1.cross(contact.normal);
			real rnB1 = rB1.cross(contact.normal);
			real rnA2 = rA2.cross(contact.normal);
			real rnB2 = rB2.cross(contact.normal);

			real k11 = imA + iiA * rnA1 * rnA1 + imB + iiB * rnB1 * rnB1;
			real k12 = imA + iiA * rnA1 * rnA2 + imB + iiB * rnB1 * rnB2;
			real k22 = imA + iiA * rnA2 * rnA2 + imB + iiB * rnB2 * rnB2;

			//numerical stability check to ensure invertible matrix
			Matrix2x2 invA;

			invA.set(k11, k12, k12, k22);
			invA.invert();

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

			m_positions[pair.idA] += (impulse1 + impulse2) * imA;
			m_rotations[pair.idA] += iiA * (rA1.cross(impulse1) + rA2.cross(impulse2));

			m_positions[pair.idB] -= (impulse1 + impulse2) * imB;
			m_rotations[pair.idB] += iiB * (rB1.cross(-impulse1) + rB2.cross(-impulse2));
		}
		else
		{
			// solve one by one
			for (int i = 0; i < contact.count; ++i)
			{
				Transform transformA(m_positions[pair.idA], m_rotations[pair.idA], 1.0f);
				Transform transformB(m_positions[pair.idB], m_rotations[pair.idB], 1.0f);

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

				real effectiveMassNormal = realEqual(kNormal, 0.0f) ? 0 : 1.0f / kNormal;

				real lambda = effectiveMassNormal * bias;
				lambda = Math::max(lambda, 0);

				Vector2 impulse = lambda * contact.normal;

				m_positions[pair.idA] += impulse * imA;
				m_rotations[pair.idA] += iiA * rA.cross(impulse);

				m_positions[pair.idB] -= imB * impulse;
				m_rotations[pair.idB] += iiB * rB.cross(-impulse);
			}
		}
	}

	void PhysicsScene::solveJointVelocity(real dt)
	{

	}

	void PhysicsScene::solveJointPosition(real dt)
	{

	}

	real PhysicsScene::computeInertia(real mass, const Shape* shape)
	{
		real inertia = 0.0f;

		switch (shape->type())
		{
		case ShapeType::Polygon:
		{
			auto polygon = static_cast<const ST::Polygon*>(shape);
			const Vector2 center = polygon->center();
			real sum1 = 0.0;
			real sum2 = 0.0;
			for (int i = 0; i < polygon->vertices().size(); ++i)
			{
				int next = (i + 1) % polygon->vertices().size();

				Vector2 n1 = polygon->vertices()[i] - center;
				Vector2 n2 = polygon->vertices()[next] - center;
				real cross = std::fabs(n1.cross(n2));
				real dot = n2.dot(n2) + n2.dot(n1) + n1.dot(n1);
				sum1 += cross * dot;
				sum2 += cross;
			}

			inertia = mass * (1.0f / 6.0f) * sum1 / sum2;

			break;
		}
		case ShapeType::Circle:
		{
			auto circle = static_cast<const ST::Circle*>(shape);
			inertia = mass * circle->radius() * circle->radius() / 2;
			break;
		}
		case ShapeType::Ellipse:
		{
			auto ellipse = static_cast<const ST::Ellipse*>(shape);
			const real a = ellipse->A();
			const real b = ellipse->B();
			inertia = mass * (a * a + b * b) * (1.0f / 5.0f);
			break;
		}
		case ShapeType::Capsule:
		{
			auto capsule = static_cast<const ST::Capsule*>(shape);

			real r, h;

			if (capsule->width() >= capsule->height())//Horizontal
			{
				r = capsule->height() / 2.0f;
				h = capsule->width() - capsule->height();
			}
			else//Vertical
			{
				r = capsule->width() / 2.0f;
				h = capsule->height() - capsule->width();
			}

			real volume = Constant::Pi * r * r + h * 2 * r;
			real rho = mass / volume;
			real massS = rho * Constant::Pi * r * r;
			real massC = rho * h * 2.0f * r;
			real inertiaC = (1.0f / 12.0f) * massC * (h * h + (2.0f * r) * (2.0f * r));
			real inertiaS = massS * r * r * 0.5f;
			inertia = inertiaC + inertiaS + massS * (3.0f * r + 2.0f * h) * h / 8.0f;

			break;
		}
		}

		return inertia;
	}
}
