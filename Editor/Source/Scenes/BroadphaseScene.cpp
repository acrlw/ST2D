#include "BroadphaseScene.h"

#include <ranges>
#include <unordered_set>


namespace STEditor
{
	BroadphaseScene::~BroadphaseScene()
	{

	}

	void BroadphaseScene::onLoad()
	{
		ZoneScopedN("[BroadphaseScene] On Load");

		m_land.set(40.0f, 0.1f);
		m_rectangle.set(0.5f, 0.5f);
		m_circle.setRadius(0.15f);
		m_capsule.set(0.4f, 0.2f);
		m_triangle.append({ {-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, Math::sqrt(2.0f)} });
		m_polygon.append({
			{0.0f, 4.0f}, {-3.0f, 3.0f}, {-4.0f, 0.0f}, {-3.0f, -3.0f}, {0, -4.0f},
			{3.0f, -3.0f}, {4.0f, 0.0f}, {3.0f, 3.0f}
			});
		m_triangle.scale(0.15f);
		m_polygon.scale(0.05f);

		m_queryAABB.position = Vector2(0.0f, 0.0f);
		m_queryAABB.width = 6;
		m_queryAABB.height = 6;

		m_queryRayOrigin.set(-9.85f, -9.5f);
		m_queryRayDirection.set(1.0f, 1.5f).normalize();


		createShapes();

		m_dbvtStack.reserve(512);

	}

	void BroadphaseScene::onUnLoad()
	{

	}

	void BroadphaseScene::onUpdate(float deltaTime)
	{

	}


	void BroadphaseScene::onRender(Renderer2D& renderer)
	{
		ZoneScopedN("[BroadphaseScene] On Render");

		Vector2 pos(1.0f, 1.0f);

		//renderer.text(pos, DarkPalette::Green, "This is a sample text.", 0.25f, true);

		for(int i = 0; i < m_objectIds.size(); ++i)
		{
			if(m_showObjectId)
			{
				std::string id = std::to_string(m_objectIds[i]);
				renderer.text(m_transforms[i].position, Palette::Gray, id);
			}

			if (m_showTransform)
				renderer.orientation(m_transforms[i]);

			if(m_showObject)
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

		if (m_showBVT)
		{
			m_isBoxCollide = false;
			if(m_canDraw)
			{
				m_isBoxCollide = m_dbvt.nodes()[m_boxIndex1].aabb.collide(m_dbvt.nodes()[m_boxIndex2].aabb);
			}

			m_dbvtStack.push_back(m_dbvt.rootIndex());
			while (!m_dbvtStack.empty())
			{
				int index = m_dbvtStack.back();
				m_dbvtStack.pop_back();

				if (index == -1)
					continue;

				AABB aabb = m_dbvt.nodes()[index].aabb;
				aabb.expand(m_expandRatio * static_cast<real>(1 + m_dbvt.nodes()[index].height));

				if (m_dbvt.nodes()[index].height <= m_currentHeight)
				{
					if (index == m_dbvt.rootIndex())
						renderer.aabb(aabb, Palette::Red);
					else
						renderer.aabb(aabb, Palette::Cyan);
				}

				if(m_canDraw)
				{
					if(index == m_boxIndex1)
					{
						renderer.aabb(aabb, Palette::Orange);
					}
					else if(index == m_boxIndex2)
					{
						renderer.aabb(aabb, Palette::Purple);
					}
				}


				m_dbvtStack.push_back(m_dbvt.nodes()[index].left);
				m_dbvtStack.push_back(m_dbvt.nodes()[index].right);

			}


		}

		if(m_idsObject.size() > 0)
		{
			for (auto&& id : m_idsObject)
				renderer.shape(m_transforms[id], m_shapes[id], Palette::Red);
		}

		if (m_idsAABB.size() > 0)
		{
			for (auto&& id : m_idsAABB)
			{
				AABB aabb = m_aabbs[id];
				aabb.expand(m_expandRatio * static_cast<real>(1));
				renderer.aabb(aabb, Palette::Red);
			}
		}

		if(m_idsRaycast.size() > 0)
		{
			for (auto&& id : m_idsRaycast)
			{
				AABB aabb = m_aabbs[id];
				aabb.expand(m_expandRatio * static_cast<real>(1));
				renderer.aabb(aabb, Palette::Red);
			}
		}

		if (m_showQueryRay)
		{
			renderer.line(m_queryRayOrigin, m_queryRayOrigin + m_queryRayDirection * m_rayMaxDistance, Palette::Yellow);
			
			Vector2 start = m_queryRayOrigin;

			Vector2 end = m_queryRayOrigin + m_queryRayDirection * m_rayMaxDistance;

			CellIndex rStart, rEnd, cStart, cEnd;

			m_grid.getGridIndicesFromVector(start, rStart, cStart);
			m_grid.getGridIndicesFromVector(end, rEnd, cEnd);

			int dR = std::abs(rEnd - rStart);
			int dC = std::abs(cEnd - cStart);

			int stepR = rStart < rEnd ? 1 : -1;
			int stepC = cStart < cEnd ? 1 : -1;

			std::unordered_set<CellPosition, CellPositionHash> uniqueCells;

			if (dC != 0)
			{
				for (CellIndex c = cStart; ; c += stepC)
				{
					real x = m_grid.gridShift().x + static_cast<real>(c) * m_grid.cellWidth();
					real t = (x - start.x) / m_queryRayDirection.x;

					if (t < 0.0f)
						continue;

					if (std::abs(t) > m_rayMaxDistance)
						break;

					Vector2 p = start + m_queryRayDirection * t;
					CellPosition cp;
					cp.row = static_cast<CellIndex>(std::floor((p.y - m_grid.gridShift().y) / m_grid.cellHeight()));
					cp.col = c;
					uniqueCells.insert(cp);

					if (stepR > 0 && stepC < 0)
					{
						cp.col += stepC;
						uniqueCells.insert(cp);
					}
					else if (stepR < 0 && stepC > 0)
					{
						cp.col -= stepC;
						uniqueCells.insert(cp);
					}
				}
			}

			if (dR != 0)
			{
				for (CellIndex r = rStart; ; r += stepR)
				{
					real y = m_grid.gridShift().y + static_cast<real>(r) * m_grid.cellHeight();
					real t = (y - start.y) / m_queryRayDirection.y;

					if (t < 0.0f)
						continue;

					if (std::abs(t) > m_rayMaxDistance)
						break;

					Vector2 p = start + m_queryRayDirection * t;
					CellPosition cp;
					cp.row = r;
					cp.col = static_cast<CellIndex>(std::floor((p.x - m_grid.gridShift().x) / m_grid.cellWidth()));
					uniqueCells.insert(cp);

				}
			}

			uniqueCells.insert(CellPosition{ rEnd, cEnd });

			for (auto&& elem : uniqueCells)
			{
				real row = static_cast<real>(elem.row);
				real col = static_cast<real>(elem.col);

				Vector2 rayTopLeft = m_grid.gridShift() + Vector2(col * m_grid.cellWidth(), row * m_grid.cellHeight());
				Vector2 rayBottom = m_grid.gridShift() + Vector2((col + 1.0f) * m_grid.cellWidth(), (row + 1.0f) * m_grid.cellHeight());
				AABB aabb = AABB::fromBox(rayTopLeft, rayBottom);

				renderer.aabb(aabb, Palette::Blue);
			}

			real row = static_cast<real>(rStart);
			real col = static_cast<real>(cStart);

			Vector2 rayTopLeft = m_grid.gridShift() + Vector2(col * m_grid.cellWidth(), row * m_grid.cellHeight());
			Vector2 rayBottom = m_grid.gridShift() + Vector2((col + 1.0f) * m_grid.cellWidth(), (row + 1.0f) * m_grid.cellHeight());
			AABB aabb = AABB::fromBox(rayTopLeft, rayBottom);

			renderer.aabb(aabb, Palette::Blue);

			row = static_cast<real>(rEnd);
			col = static_cast<real>(cEnd);

			rayTopLeft = m_grid.gridShift() + Vector2(col * m_grid.cellWidth(), row * m_grid.cellHeight());
			rayBottom = m_grid.gridShift() + Vector2((col + 1.0f) * m_grid.cellWidth(), (row + 1.0f) * m_grid.cellHeight());
			aabb = AABB::fromBox(rayTopLeft, rayBottom);

			renderer.aabb(aabb, Palette::Blue);

		}

		if(!m_graphColorPoints.empty())
		{
			for (auto&& [color, points] : m_graphColorPoints)
			{
				float value = static_cast<float>(color) / static_cast<float>(m_graphColorPoints.size());
				for (auto&& point : points)
				{
					renderer.point(point, gistRainbowColormap(value), 5.0f);
				}
			}
		}

		if (m_showQueryAABB)
		{
			renderer.aabb(m_queryAABB, Palette::Yellow);
		}
	}

	void BroadphaseScene::onRenderUI()
	{
		ZoneScopedN("[BroadphaseScene] On Render UI");

		ImGui::Begin("Broad-phase");

		int count = m_count;
		ImGui::DragInt("Count", &count, 2, 4, 5000);
		if (m_count != count)
		{
			m_count = count;
			createShapes();
		}

		ImGui::Checkbox("BVT Only Insert", &m_onlyInsert);
		m_dbvt.setOnlyInsert(m_onlyInsert);

		ImGui::Checkbox("Show Object", &m_showObject);
		ImGui::Checkbox("Show AABB", &m_showAABB);
		ImGui::Checkbox("Show Grid", &m_showGrid);
		ImGui::Checkbox("Show BVT", &m_showBVT);
		ImGui::Checkbox("Show Transform", &m_showTransform);
		ImGui::Checkbox("Show Object Id", &m_showObjectId);
		ImGui::Checkbox("Show Query Ray", &m_showQueryRay);
		ImGui::Checkbox("Show Query AABB", &m_showQueryAABB);

		ImGui::SliderInt("Render BVH Height", &m_currentHeight, 0, m_maxHeight);
		ImGui::DragFloat("Expand Ratio", &m_expandRatio, 0.01f, 0.0f, 1.0f);

		float maxDist = static_cast<float>(m_rayMaxDistance);
		ImGui::DragFloat("Ray Max Distance", &maxDist, 0.1f, 1.0f, 100.0f);
		m_rayMaxDistance = maxDist;

		float theta = m_theta;
		ImGui::DragFloat("Theta", &theta, 0.1f, -180.0f, 180.0f);
		m_theta = theta;

		m_queryRayDirection.set(Math::cosx(Math::radians(m_theta)), Math::sinx(Math::radians(m_theta)));


		if (m_dbvt.rootIndex() != -1)
			m_maxHeight = std::max(0, m_dbvt.nodes()[m_dbvt.rootIndex()].height);
		else
			m_maxHeight = 0;



		if(ImGui::Button("Check Height"))
		{
			m_dbvt.checkHeight();
		}

		ImGui::SameLine();

		if(ImGui::Button("Rebuild Tree"))
		{
			m_dbvt.rebuildTree();
		}

		ImGui::SameLine();

		if(ImGui::Button("Print Tree"))
		{
			m_dbvt.printTree();
		}

		if(ImGui::Button("Query Overlaps(DBVT)"))
		{
			auto pairs = m_dbvt.queryOverlaps();

			std::ranges::sort(pairs, [](const auto& a, const auto& b)
				{
					if (a.objectIdA < b.objectIdA)
						return true;
					if (a.objectIdA == b.objectIdA)
						return a.objectIdB < b.objectIdB;

					return false;
				});
			std::string str;
			for (auto iter = pairs.begin(); iter != pairs.end(); ++iter)
			{
				str += std::format("({0}, {1})", iter->objectIdA, iter->objectIdB);
				if (iter != pairs.end() - 1)
					str += ", ";
			}

			CORE_INFO("Overlaps Count: {}", pairs.size());
			CORE_INFO("Overlaps: {}", str);

		}

		ImGui::SameLine();
		if (ImGui::Button("Query AABB(DBVT)"))
		{
			m_idsAABB = m_dbvt.queryAABB(m_queryAABB);

			//std::string str;
			//for(auto&& elem: ids)
			//	str += std::format("{0} ", elem);
			//
			//CORE_INFO("AABB: {}", str);
		}

		ImGui::SameLine();
		if(ImGui::Button("Query Raycast(DBVT)"))
		{
			m_idsRaycast = m_dbvt.queryRay(m_queryRayOrigin, m_queryRayDirection, m_rayMaxDistance);
		}

		if(ImGui::Button("Query Overlaps(Grid)"))
		{
			m_idsObject.clear();
			auto pairs = m_grid.queryOverlaps();

			std::ranges::sort(pairs, [](const auto& a, const auto& b)
				{
					if (a.objectIdA < b.objectIdA)
						return true;
					if (a.objectIdA == b.objectIdA)
						return a.objectIdB < b.objectIdB;

					return false;
				});

			std::string str;
			for (auto iter = pairs.begin(); iter != pairs.end(); ++iter)
			{
				str += std::format("({0}, {1})", iter->objectIdA, iter->objectIdB);
				if (iter != pairs.end() - 1)
					str += ", ";
			}

			CORE_INFO("Overlaps Count: {}", pairs.size());
			CORE_INFO("Overlaps: {}", str);

			//std::unordered_set<int> uniqueObjects;

			//for (auto&& elem : pairs)
			//{
			//	if (!uniqueObjects.contains(elem.objectIdA))
			//	{
			//		uniqueObjects.insert(elem.objectIdA);
			//		m_idsObject.push_back(elem.objectIdA);
			//	}

			//	if (!uniqueObjects.contains(elem.objectIdB))
			//	{
			//		uniqueObjects.insert(elem.objectIdB);
			//		m_idsObject.push_back(elem.objectIdB);
			//	}

			//}
		}

		ImGui::SameLine();

		if (ImGui::Button("Query AABB(Grid)"))
		{
			m_idsAABB = m_grid.queryAABB(m_queryAABB);

			//std::string str;
			//for(auto&& elem: ids)
			//	str += std::format("{0} ", elem);
			//
			//CORE_INFO("AABB: {}", str);
		}

		ImGui::SameLine();

		if (ImGui::Button("Query Raycast(Grid)"))
		{
			m_idsRaycast = m_grid.queryRay(m_queryRayOrigin, m_queryRayDirection, m_rayMaxDistance);
		}

		if(ImGui::Button("Edge Coloring"))
		{
			auto pairs = m_dbvt.queryOverlaps();

			m_objectGraph.buildGraph(pairs);
			m_graphColorPoints.clear();

			for (auto& [color, edges] : m_objectGraph.m_colorToEdges)
			{
				for (auto&& edge : edges)
				{
					auto simplex = Narrowphase::gjk(m_transforms[edge.objectIdA], m_shapes[edge.objectIdA], m_transforms[edge.objectIdB], m_shapes[edge.objectIdB]);
					if (simplex.containsOrigin())
					{
						auto collisionInfo = Narrowphase::epa(simplex, m_transforms[edge.objectIdA], m_shapes[edge.objectIdA], m_transforms[edge.objectIdB], m_shapes[edge.objectIdB]);
						auto contacts = Narrowphase::generateContacts(collisionInfo, m_transforms[edge.objectIdA], m_shapes[edge.objectIdA], m_transforms[edge.objectIdB], m_shapes[edge.objectIdB]);

						m_graphColorPoints[color].push_back(contacts.points[0]);
						m_graphColorPoints[color].push_back(contacts.points[1]);
						if (contacts.count == 4)
						{
							m_graphColorPoints[color].push_back(contacts.points[2]);
							m_graphColorPoints[color].push_back(contacts.points[3]);
						}
					}
				}
			}
		}

		ImGui::SameLine();


		if (ImGui::Button("Update Object"))
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dist1(-9.0f, 9.0f);

			int index = 0;
			Transform t;
			t.position = Vector2(dist1(gen), dist1(gen));
			m_transforms[index] = t;
			m_aabbs[index] = AABB::fromShape(t, m_shapes[index]);
			BroadphaseObjectBinding binding;
			binding.aabb = m_aabbs[index];
			binding.bitmask = m_bitmasks[index];
			binding.objectId = m_objectIds[index];
			m_grid.updateObject(binding);
			m_dbvt.updateObject(binding);
		}

		ImGui::DragInt("Index1", &m_boxIndex1, 1, 0, 1000);
		ImGui::DragInt("Index2", &m_boxIndex2, 1, 0, 1000);

		ImGui::Checkbox("Can Draw", &m_canDraw);

		ImGui::SameLine();
		ImGui::Text("-- Is Collide: %s", m_isBoxCollide ? "True" : "False");

		ImGui::End();
	}

	void BroadphaseScene::createShapes()
	{
		ZoneScopedN("[BroadphaseScene] Create Shapes");

		{
			ZoneScopedN("[BroadphaseScene] Clear All Objects");
			m_idsAABB.clear();
			m_idsRaycast.clear();
			m_idsObject.clear();
			m_dbvt.clearAllObjects();
			m_grid.clearAllObjects();
			m_aabbs.clear();
			m_bitmasks.clear();
			m_transforms.clear();
			m_shapes.clear();
			m_objectIds.clear();
			m_objectIdPool.reset();
			m_objectGraph.clearGraph();
		}

		//std::random_device rd;
		//std::mt19937 gen(rd());
		//std::uniform_real_distribution<> dist1(-9.0f, 9.0f);
		//std::uniform_int_distribution<> dist2(0, m_shapesArray.size() - 1);
		//std::uniform_real_distribution<> dist3(-Constant::Pi, Constant::Pi);
		//std::uniform_real_distribution<> dist4(-9.0f, 9.0f);

		//real rotation = 0.0f;
		//Vector2 position;
		//Vector2 dir(1.0f, 1.0f);

		std::vector<BroadphaseObjectBinding> bindings;
		bindings.reserve(m_count);

		{
			ZoneScopedN("[BroadphaseScene] Create Shapes - Add to List");

			real offset = 0.6f;
			real max = 15.0;
			real xSpacing = 0.2f;
			real ySpacing = -0.01f;
			for (real j = 0; j < max; j += 0.5f)
			{
				for (real i = 0.0; i < max - j; i += 0.5f)
				{
					Transform t;
					t.position.set({  i * (1.0f + xSpacing) + offset, j * (1.0f + ySpacing) + 0.25f});
					t.rotation = 0;
					int shapeIndex = 0;

					m_transforms.push_back(t);
					m_shapes.push_back(m_shapesArray[shapeIndex]);
					m_bitmasks.push_back(1);
					m_aabbs.push_back(AABB::fromShape(t, m_shapesArray[shapeIndex]));

					auto id = m_objectIdPool.getNewId();
					m_objectIds.push_back(id);

					bindings.emplace_back(m_objectIds.back(), 1, m_aabbs.back());

				}
				offset += 0.3f;
			}

			offset = max + 6.0f;

			for (real j = 0; j < max; j += 0.5f)
			{
				for (real i = 0.0; i < max - j; i += 0.5f)
				{
					Transform t;
					t.position.set({ i * (1.0f + xSpacing) + offset, j * (1.0f + ySpacing) + 0.25f });
					t.rotation = 0;
					int shapeIndex = 0;

					m_transforms.push_back(t);
					m_shapes.push_back(m_shapesArray[shapeIndex]);
					m_bitmasks.push_back(1);
					m_aabbs.push_back(AABB::fromShape(t, m_shapesArray[shapeIndex]));

					auto id = m_objectIdPool.getNewId();
					m_objectIds.push_back(id);

					bindings.emplace_back(m_objectIds.back(), 1, m_aabbs.back());

				}
				offset += 0.3f;
			}

			//for (int i = 0; i < m_count; ++i)
			//{
			//	Transform t;
			//	int shapeIndex = 0;

			//	//t.position = Vector2(dist4(gen), dist1(gen));
			//	//t.rotation = dist3(gen);
			//	//shapeIndex = dist2(gen);

			//	//position.x = static_cast<real>(i % 12);
			//	//position.y = static_cast<real>(i / 12);
			//	//position = Vector2(1, 1);

			//	//t.position = position;
			//	//t.rotation = rotation;



			//	//int shapeIndex = 0;


			//	m_transforms.push_back(t);
			//	m_shapes.push_back(m_shapesArray[shapeIndex]);
			//	m_bitmasks.push_back(1);
			//	m_aabbs.push_back(AABB::fromShape(t, m_shapesArray[shapeIndex]));
			//	m_objectIds.push_back(m_objectIdPool.getNewId());

			//	bindings.emplace_back(m_objectIds[i], 1, m_aabbs[i]);

			//	//position += dir;
			//}
		}
		{

			Transform trans;
			trans.position.set(20.0f, -0.045);

			m_transforms.push_back(trans);
			m_shapes.push_back(&m_land);
			m_bitmasks.push_back(1);
			m_aabbs.push_back(AABB::fromShape(trans, &m_land));
			auto landId = m_objectIdPool.getNewId();
			m_objectIds.push_back(landId);

			bindings.emplace_back(landId, 1, m_aabbs.back());
			m_objectGraph.addEnableColorRepeated(landId);
		}

		{
			ZoneScopedN("[BroadphaseScene] Create Shapes - Add to Grid");
			for (auto&& binding : bindings)
				m_grid.addObject(binding);
		}

		{
			ZoneScopedN("[BroadphaseScene] Create Shapes - Add to DBVT");
			for (auto&& binding : bindings)
				m_dbvt.addObject(binding);
			
		}
	}
}


