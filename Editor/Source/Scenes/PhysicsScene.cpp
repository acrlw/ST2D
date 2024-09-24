#include "PhysicsScene.h"


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
		circle.setRadius(1.0f);
		edge.set(Vector2(-10.0f, -0.5f), Vector2(10.0f, -0.5f));
		polygon.set({ Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f), Vector2(1.0f, 1.0f) });

		createObjects();
	}

	void PhysicsScene::onUnLoad()
	{

	}

	void PhysicsScene::onUpdate(float deltaTime)
	{
		ZoneScopedN("[BroadphaseScene] On Update");

		// narrow phase generate contacts
		auto pairs = m_dbvt.queryOverlaps();
		for(auto&& elem : pairs)
		{
			if (!m_aabbs[elem.objectIdA].collide(m_aabbs[elem.objectIdB]))
				continue;

			auto simplex = Narrowphase::gjk(m_transforms[elem.objectIdA], m_shapes[elem.objectIdA], m_transforms[elem.objectIdB], m_shapes[elem.objectIdB]);
			if(simplex.isContainOrigin)
			{
				auto info = Narrowphase::epa(simplex, m_transforms[elem.objectIdA], m_shapes[elem.objectIdA], m_transforms[elem.objectIdB], m_shapes[elem.objectIdB]);
				auto contacts = Narrowphase::generateContacts(info, m_transforms[elem.objectIdA], m_shapes[elem.objectIdA], m_transforms[elem.objectIdB], m_shapes[elem.objectIdB]);

				if (!m_contacts.contains(elem))
				{
					if (contacts.ids[0] != m_contacts[elem].pair.ids[0] 
						|| contacts.ids[1] != m_contacts[elem].pair.ids[1])
					{
						m_contacts[elem].contacts[0].accumulatedNormalImpulse = 0.0f;
						m_contacts[elem].contacts[0].accumulatedTangentImpulse = 0.0f;
						m_contacts[elem].contacts[1].accumulatedNormalImpulse = 0.0f;
						m_contacts[elem].contacts[1].accumulatedTangentImpulse = 0.0f;
					}
				}

				Contact contact;
				contact.count = contacts.count / 2;
				contact.pair = contacts;
				m_contacts[elem] = contact;
			}
		}

		// setup solver

		m_objectGraph.clearGraph();
		m_objectGraph.addEnableColorRepeated(m_landId);
		m_objectGraph.buildGraph(pairs);
		

		// integrate velocities

		// solve constraints

		// integrate positions

		// solve position constraints

		// update broad phase

		// disable all contacts

		for(auto& value : m_contacts | std::views::values)
			value.count = 0;

	}

	void PhysicsScene::onRender(Renderer2D& renderer)
	{
		ZoneScopedN("[BroadphaseScene] On Render");

		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			if (m_showObjectID)
			{
				std::string id = std::to_string(m_objectIds[i]);
				renderer.text(m_transforms[i].position, Palette::Gray, id);
			}

			if (m_showTransform)
				renderer.orientation(m_transforms[i]);

			if (m_showObject)
				renderer.shape(m_transforms[i], m_shapes[i], Palette::Green);

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
			if(m_showContactNormal)
			{
				
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

		ImGui::Checkbox("Show Object", &m_showObject);
		ImGui::Checkbox("Show Object Id", &m_showObjectID);
		ImGui::Checkbox("Show Transform", &m_showTransform);
		ImGui::Checkbox("Show Joint", &m_showJoint);

		ImGui::Checkbox("Show AABB", &m_showAABB);
		ImGui::Checkbox("Show Grid", &m_showGrid);
		ImGui::Checkbox("Show DBVT", &m_showDBVT);
		ImGui::Checkbox("Show Graph Coloring", &m_showGraphColor);

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
					t.position.set({ i * (1.0f + xSpacing) + offset, j * (1.0f + ySpacing) + 0.26f });
					t.rotation = 0;

					auto id = m_objectIdPool.getNewId();
					m_objectIds.push_back(id);

					m_transforms.push_back(t);
					m_velocities.emplace_back();
					m_angularVelocities.emplace_back(0.0f);
					m_forces.emplace_back(0.0f);
					m_torques.emplace_back(0.0f);
					m_masses.emplace_back(1.0f);
					m_inertias.emplace_back(1.0f);
					m_invMasses.emplace_back(1.0f);
					m_invInertias.emplace_back(1.0f);
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

			m_transforms.push_back(trans);
			m_velocities.emplace_back();
			m_angularVelocities.emplace_back(0.0f);
			m_forces.emplace_back(0.0f);
			m_torques.emplace_back(0.0f);
			m_masses.emplace_back(1.0f);
			m_inertias.emplace_back(1.0f);
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
		m_objectIds.clear();

		m_transforms.clear();
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
}


