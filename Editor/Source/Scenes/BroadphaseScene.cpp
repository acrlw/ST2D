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

		m_rectangle.set(0.2f, 0.2f);
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

	void BroadphaseScene::onDraw(sf::RenderWindow& window)
	{
		ZoneScopedN("[BroadphaseScene] On Render");

		for(int i = 0; i < m_count; ++i)
		{
			if (m_showObjectId)
			{
				RenderSFMLImpl::renderInt(window, *m_settings.camera, m_transforms[i].position, 
					*m_settings.font, m_objectIds[i], RenderConstant::Gray, 12, {});
			}

			if(m_showTransform)
			{
				RenderSFMLImpl::renderPoint(window, *m_settings.camera, m_transforms[i].position, RenderConstant::Yellow, 2.0f);
				RenderSFMLImpl::renderOrientation(window, *m_settings.camera, m_transforms[i]);
			}

			if(m_showObject)
				RenderSFMLImpl::renderShape(window, *m_settings.camera, m_transforms[i], m_shapes[i], RenderConstant::Green);

			if (m_showAABB)
			{
				//std::vector<Vector2> points;
				//points.emplace_back(m_aabbs[i].topLeft());
				//points.emplace_back(m_aabbs[i].topRight());
				//points.emplace_back(m_aabbs[i].bottomRight());
				//points.emplace_back(m_aabbs[i].bottomLeft());
				//points.emplace_back(m_aabbs[i].topLeft());
				//RenderSFMLImpl::renderPolyDashedLine(window, *m_settings.camera, points, RenderConstant::Yellow);
				RenderSFMLImpl::renderAABB(window, *m_settings.camera, m_aabbs[i], RenderConstant::Green);

			}

		}

		if (m_showGrid)
		{
			auto color = RenderConstant::Yellow;
			color.a = 50;
			for (const auto& [key, value] : m_grid.m_usedCells)
			{
				if (value.empty())
					continue;

				real row = static_cast<real>(key.row);
				real col = static_cast<real>(key.col);

				Vector2 start = m_grid.m_gridShift + Vector2(col * m_grid.m_cellWidth, row * m_grid.m_cellHeight);
				Vector2 end = m_grid.m_gridShift + Vector2((col + 1.0f) * m_grid.m_cellWidth, (row + 1.0f) * m_grid.m_cellHeight);
				AABB aabb;
				aabb.position = (start + end) * 0.5f;
				aabb.width = m_grid.m_cellWidth;
				aabb.height = m_grid.m_cellHeight;

				RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, color);

			}
			//color = RenderConstant::Yellow;

			//for(auto&& elem: m_grid.m_objects)
			//{
			//	AABB aabb = elem.binding.aabb;
			//	Vector2 topLeft = aabb.topLeft();
			//	Vector2 bottomRight = aabb.bottomRight();
			//	int rowStart = static_cast<int>(std::floor((m_grid.m_gridTopLeft.y - topLeft.y) / m_grid.m_cellHeight));
			//	int rowEnd = static_cast<int>(std::floor((m_grid.m_gridTopLeft.y - bottomRight.y) / m_grid.m_cellHeight));
			//	int colStart = static_cast<int>(std::floor((topLeft.x - m_grid.m_gridTopLeft.x) / m_grid.m_cellWidth));
			//	int colEnd = static_cast<int>(std::floor((bottomRight.x - m_grid.m_gridTopLeft.x) / m_grid.m_cellWidth));
			//	
			//	for (int i = rowStart; i <= rowEnd; ++i)
			//	{
			//		for (int j = colStart; j <= colEnd; ++j)
			//		{
			//			Vector2 start = m_grid.m_gridTopLeft + Vector2(j * m_grid.m_cellWidth, -i * m_grid.m_cellHeight);
			//			Vector2 end = start + Vector2(m_grid.m_cellWidth, 0.0f);
			//			RenderSFMLImpl::renderLine(window, *m_settings.camera, start, end, color);
			//			end = start + Vector2(0.0f, -m_grid.m_cellHeight);
			//			RenderSFMLImpl::renderLine(window, *m_settings.camera, start, end, color);
			//		}
			//	}
			//	Vector2 start = m_grid.m_gridTopLeft + Vector2((colEnd + 1) * m_grid.m_cellWidth, -rowStart * m_grid.m_cellHeight);
			//	Vector2 end = start + Vector2(0.0f, -m_grid.m_cellHeight * (rowEnd - rowStart + 1));
			//	RenderSFMLImpl::renderLine(window, *m_settings.camera, start, end, color);

			//	start = m_grid.m_gridTopLeft + Vector2(colStart * m_grid.m_cellWidth, -(rowEnd + 1) * m_grid.m_cellHeight);
			//	end = start + Vector2(m_grid.m_cellWidth * (colEnd - colStart + 1), 0.0f);
			//	RenderSFMLImpl::renderLine(window, *m_settings.camera, start, end, color);
			//}
			


		}

		if (m_showBVT)
		{
			m_dbvtStack.push_back(m_dbvt.m_rootIndex);
			while (!m_dbvtStack.empty())
			{
				int index = m_dbvtStack.back();
				m_dbvtStack.pop_back();

				if (index == -1)
					continue;

				AABB aabb = m_dbvt.m_nodes[index].aabb;
				aabb.expand(m_expandRatio * static_cast<real>(1 + m_dbvt.m_nodes[index].height));

				if (m_dbvt.m_nodes[index].height <= m_currentHeight)
				{
					if (index == m_dbvt.m_rootIndex)
						RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Red);
					else
						RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Cyan);
				}


				m_dbvtStack.push_back(m_dbvt.m_nodes[index].left);
				m_dbvtStack.push_back(m_dbvt.m_nodes[index].right);

			}

		}


		

		if(m_idsObject.size() > 0)
		{
			for (auto&& id : m_idsObject)
			{
				RenderSFMLImpl::renderShape(window, *m_settings.camera, m_transforms[id], m_shapes[id], RenderConstant::Red);
			}
		}

		if (m_idsAABB.size() > 0)
		{
			for (auto&& id : m_idsAABB)
			{
				AABB aabb = m_aabbs[id];
				aabb.expand(m_expandRatio * static_cast<real>(1));
				RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Red);
			}
		}

		if(m_idsRaycast.size() > 0)
		{
			for (auto&& id : m_idsRaycast)
			{
				AABB aabb = m_aabbs[id];
				aabb.expand(m_expandRatio * static_cast<real>(1));
				RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Red);
			}
		}

		if (m_showQueryRay)
		{
			RenderSFMLImpl::renderLine(window, *m_settings.camera, m_queryRayOrigin, m_queryRayOrigin + m_queryRayDirection * m_rayMaxDistance, RenderConstant::Yellow);

			Vector2 start = m_queryRayOrigin;

			Vector2 end = m_queryRayOrigin + m_queryRayDirection * m_rayMaxDistance;

			CellIndex rStart, rEnd, cStart, cEnd;

			m_grid.getGridIndicesFromVector(start, rStart, cStart);
			m_grid.getGridIndicesFromVector(end, rEnd, cEnd);

			int dR = std::abs(rEnd - rStart);
			int dC = std::abs(cEnd - cStart);

			int stepR = rStart < rEnd ? 1 : -1;
			int stepC = cStart < cEnd ? 1 : -1;

			bool addNew = stepR * stepC > 0;

			std::unordered_set<CellPosition, CellPositionHash> uniqueCells;

			if (dC != 0)
			{
				for (CellIndex c = cStart; ; c += stepC)
				{
					real x = m_grid.m_gridShift.x + static_cast<real>(c) * m_grid.m_cellWidth;
					real t = (x - start.x) / m_queryRayDirection.x;

					if (t < 0.0f)
						continue;

					if (std::abs(t) > m_rayMaxDistance)
						break;

					Vector2 p = start + m_queryRayDirection * t;
					CellPosition cp;
					cp.row = static_cast<CellIndex>(std::floor((p.y - m_grid.m_gridShift.y) / m_grid.m_cellHeight));
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
					real y = m_grid.m_gridShift.y + static_cast<real>(r) * m_grid.m_cellHeight;
					real t = (y - start.y) / m_queryRayDirection.y;

					if (t < 0.0f)
						continue;

					if (std::abs(t) > m_rayMaxDistance)
						break;

					Vector2 p = start + m_queryRayDirection * t;
					CellPosition cp;
					cp.row = r;
					cp.col = static_cast<CellIndex>(std::floor((p.x - m_grid.m_gridShift.x) / m_grid.m_cellWidth));
					uniqueCells.insert(cp);

				}
			}

			uniqueCells.insert(CellPosition{ rEnd, cEnd });

			for (auto&& elem : uniqueCells)
			{
				real row = static_cast<real>(elem.row);
				real col = static_cast<real>(elem.col);

				Vector2 rayTopLeft = m_grid.m_gridShift + Vector2(col * m_grid.m_cellWidth, row * m_grid.m_cellHeight);
				Vector2 rayBottom = m_grid.m_gridShift + Vector2((col + 1.0f) * m_grid.m_cellWidth, (row + 1.0f) * m_grid.m_cellHeight);
				AABB aabb = AABB::fromBox(rayTopLeft, rayBottom);

				RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Blue);
			}

			real row = static_cast<real>(rStart);
			real col = static_cast<real>(cStart);

			Vector2 rayTopLeft = m_grid.m_gridShift + Vector2(col * m_grid.m_cellWidth, row * m_grid.m_cellHeight);
			Vector2 rayBottom = m_grid.m_gridShift + Vector2((col + 1.0f) * m_grid.m_cellWidth, (row + 1.0f) * m_grid.m_cellHeight);
			AABB aabb = AABB::fromBox(rayTopLeft, rayBottom);

			RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Blue);


			row = static_cast<real>(rEnd);
			col = static_cast<real>(cEnd);

			rayTopLeft = m_grid.m_gridShift + Vector2(col * m_grid.m_cellWidth, row * m_grid.m_cellHeight);
			rayBottom = m_grid.m_gridShift + Vector2((col + 1.0f) * m_grid.m_cellWidth, (row + 1.0f) * m_grid.m_cellHeight);
			aabb = AABB::fromBox(rayTopLeft, rayBottom);

			RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Blue);

		}

		if (m_showQueryAABB)
		{
			RenderSFMLImpl::renderAABB(window, *m_settings.camera, m_queryAABB, RenderConstant::Yellow);
		}

	}

	void BroadphaseScene::onRender(GLFWwindow* window, Renderer2D* renderer)
	{
		ZoneScopedN("[BroadphaseScene] On Render");


	}

	void BroadphaseScene::onRenderUI()
	{
		ZoneScopedN("[BroadphaseScene] On RenderUI");

		ImGui::Begin("Broad-phase");

		int count = m_count;
		ImGui::DragInt("Count", &count, 2, 4, 5000);
		if (m_count != count)
		{
			m_count = count;
			createShapes();
		}

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


		if (m_dbvt.m_rootIndex != -1)
			m_maxHeight = std::max(0, m_dbvt.m_nodes[m_dbvt.m_rootIndex].height);
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
			for(auto&& elem: pairs)
				str += std::format("({0}, {1}) ", elem.objectIdA, elem.objectIdB);
			
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
			for (auto&& elem : pairs)
				str += std::format("({0}, {1}) ", elem.objectIdA, elem.objectIdB);

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

		if(ImGui::Button("Update Object"))
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


		ImGui::End();
	}

	void BroadphaseScene::createShapes()
	{
		ZoneScopedN("[BroadphaseScene] Create Shapes");
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

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist1(-9.0f, 9.0f);
		std::uniform_int_distribution<> dist2(0, m_shapesArray.size() - 1);
		std::uniform_real_distribution<> dist3(-Constant::Pi, Constant::Pi);
		std::uniform_real_distribution<> dist4(-9.0f, 9.0f);

		real rotation = 0.0f;
		Vector2 position;
		Vector2 dir(1.0f, 1.0f);

		for (int i = 0; i < m_count; ++i)
		{
			Transform t;
			t.position = Vector2(dist4(gen), dist1(gen));
			t.rotation = dist3(gen);

			int shapeIndex = dist2(gen);

			//position.x = static_cast<real>(i % 12);
			//position.y = static_cast<real>(i / 12);
			//position = Vector2(1, 1);

			//t.position = position;
			//t.rotation = rotation;



			//int shapeIndex = 0;

			m_transforms.push_back(t);
			m_shapes.push_back(m_shapesArray[shapeIndex]);
			m_bitmasks.push_back(1);
			m_aabbs.push_back(AABB::fromShape(t, m_shapesArray[shapeIndex]));
			m_objectIds.push_back(m_objectIdPool.getNewId());

			BroadphaseObjectBinding binding;
			binding.aabb = m_aabbs[i];
			binding.bitmask = 1;
			binding.objectId = m_objectIds[i];
			m_grid.addObject(binding);
			m_dbvt.addObject(binding);

			//position += dir;
		}

	}
}


