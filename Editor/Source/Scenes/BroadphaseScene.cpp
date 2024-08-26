#include "BroadphaseScene.h"

#include <deque>


namespace STEditor
{
	BroadphaseScene::~BroadphaseScene()
	{

	}

	void BroadphaseScene::onLoad()
	{
		m_rectangle.set(0.5f, 0.5f);
		m_circle.setRadius(0.5f);
		m_capsule.set(1.5f, 0.5f);
		m_triangle.append({ {-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, Math::sqrt(2.0f)} });
		m_polygon.append({
			{0.0f, 4.0f}, {-3.0f, 3.0f}, {-4.0f, 0.0f}, {-3.0f, -3.0f}, {0, -4.0f},
			{3.0f, -3.0f}, {4.0f, 0.0f}, {3.0f, 3.0f}
			});
		m_triangle.scale(0.5f);
		m_polygon.scale(0.1f);

		createShapes();


	}

	void BroadphaseScene::onUnLoad()
	{

	}

	void BroadphaseScene::onUpdate(float deltaTime)
	{

	}

	void BroadphaseScene::onRender(sf::RenderWindow& window)
	{
		for(int i = 0; i < m_count; ++i)
		{
			if (m_showObjectId)
			{
				RenderSFMLImpl::renderInt(window, *m_settings.camera, m_transforms[i].position, 
					*m_settings.font, m_objectIds[i], RenderConstant::Gray, 18, {});
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

			if(m_showBVT)
			{
				std::deque<int> queue;
				queue.push_back(m_dbvt.m_rootIndex);
				while(!queue.empty())
				{
					int index = queue.front();
					queue.pop_front();

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


					queue.push_back(m_dbvt.m_nodes[index].left);
					queue.push_back(m_dbvt.m_nodes[index].right);

				}

			}

		}

	}

	void BroadphaseScene::onRenderUI()
	{
		ImGui::Begin("Broad-phase");

		int count = m_count;
		ImGui::DragInt("Count", &count, 2, 10, 1000);
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

		if(ImGui::Button("Rebuild Tree"))
		{
			m_dbvt.rebuildTree();
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
		m_dbvt.clearAllObjects();
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
		std::uniform_real_distribution<> dist4(-29.0f, 29.0f);
		real rotation = 0.0f;
		Vector2 position(1.0f, 4.0f);
		Vector2 dir(1.0f, 0.0f);

		for (int i = 0; i < m_count; ++i)
		{
			Transform t;
			//t.position = Vector2(dist4(gen), dist1(gen));
			//t.rotation = dist3(gen);

			//int shapeIndex = dist2(gen);

			position.x = static_cast<real>(i % 12) + 1.0f;
			position.y = static_cast<real>(i / 12);
			position += Vector2(1, 4);

			t.position = position;
			t.rotation = rotation;


			int shapeIndex = 0;

			m_transforms.push_back(t);
			m_shapes.push_back(m_shapesArray[shapeIndex]);
			m_bitmasks.push_back(1);
			m_aabbs.push_back(AABB::fromShape(t, m_shapesArray[shapeIndex]));
			m_objectIds.push_back(m_objectIdPool.getNewId());

			BroadphaseObjectBinding binding;
			binding.aabb = m_aabbs[i];
			binding.bitmask = 0;
			binding.objectId = m_objectIds[i];
			m_dbvt.addObject(binding);

		}

	}
}


