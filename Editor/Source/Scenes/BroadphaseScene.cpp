#include "BroadphaseScene.h"



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

		createShapes();

		m_dbvtStack.reserve(512);

	}

	void BroadphaseScene::onUnLoad()
	{

	}

	void BroadphaseScene::onUpdate(float deltaTime)
	{

	}

	void BroadphaseScene::onRender(sf::RenderWindow& window)
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
				RenderSFMLImpl::renderAngleLine(window, *m_settings.camera, m_transforms[i]);
			}

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
				RenderSFMLImpl::renderAABB(window, *m_settings.camera, m_aabbs[i], RenderConstant::Cyan);
			}
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
					else if (m_dbvt.m_nodes[index].isLeaf())
						RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Cyan);
					else
						RenderSFMLImpl::renderAABB(window, *m_settings.camera, aabb, RenderConstant::Cyan);
				}


				m_dbvtStack.push_back(m_dbvt.m_nodes[index].left);
				m_dbvtStack.push_back(m_dbvt.m_nodes[index].right);

			}

		}

	}

	void BroadphaseScene::onRenderUI()
	{
		ZoneScopedN("[BroadphaseScene] On RenderUI");

		ImGui::Begin("Broad-phase");

		int count = m_count;
		ImGui::DragInt("Count", &count, 2, 5, 5000);
		if (m_count != count)
		{
			m_count = count;
			createShapes();
		}

		ImGui::Checkbox("Show AABB", &m_showAABB);
		ImGui::Checkbox("Show Grid", &m_showGrid);
		ImGui::Checkbox("Show BVT", &m_showBVT);
		ImGui::Checkbox("Show Transform", &m_showTransform);
		ImGui::Checkbox("Show Object Id", &m_showObjectId);

		ImGui::SliderInt("Render BVH Height", &m_currentHeight, 0, m_maxHeight);
		ImGui::DragFloat("Expand Ratio", &m_expandRatio, 0.01f, 0.0f, 1.0f);

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

		if(ImGui::Button("Query Overlaps"))
		{
			auto pairs = m_dbvt.queryOverlaps();

			//std::ranges::sort(pairs, [](const auto& a, const auto& b)
			//	{
			//		if (a.objectIdA < b.objectIdA)
			//			return true;
			//		if (a.objectIdA == b.objectIdA)
			//			return a.objectIdB < b.objectIdB;

			//	return false;
			//	});
			//std::string str;
			//for(auto&& elem: pairs)
			//	str += std::format("({0}, {1}) ", elem.objectIdA, elem.objectIdB);
			//
			//CORE_INFO("Overlaps: {}", str);
		}

		ImGui::End();
	}

	void BroadphaseScene::onMousePress(sf::Event& event)
	{
		AbstractScene::onMousePress(event);
	}

	void BroadphaseScene::onMouseRelease(sf::Event& event)
	{
		AbstractScene::onMouseRelease(event);
	}

	void BroadphaseScene::onMouseMove(sf::Event& event)
	{
		AbstractScene::onMouseMove(event);
	}

	void BroadphaseScene::onMouseDoubleClick(sf::Event& event)
	{
		AbstractScene::onMouseDoubleClick(event);
	}

	void BroadphaseScene::onKeyRelease(sf::Event& event)
	{
		AbstractScene::onKeyRelease(event);
	}

	void BroadphaseScene::onKeyPressed(sf::Event& event)
	{
		AbstractScene::onKeyPressed(event);
	}

	void BroadphaseScene::createShapes()
	{
		ZoneScopedN("[BroadphaseScene] Create Shapes");

		m_dbvt.clearAllObjects();
		m_aabbs.clear();
		m_bitmasks.clear();
		m_transforms.clear();
		m_shapes.clear();
		m_objectIds.clear();
		m_objectIdPool.reset();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist1(-3.0f, 3.0f);
		std::uniform_int_distribution<> dist2(0, m_shapesArray.size() - 1);
		std::uniform_real_distribution<> dist3(-Constant::Pi, Constant::Pi);
		std::uniform_real_distribution<> dist4(-3.0f, 3.0f);
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
			m_dbvt.addObject(binding);

			//position += dir;
		}

	}
}


