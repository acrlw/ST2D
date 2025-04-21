#include "ContinuousScene.h"

namespace STEditor
{
	ContinuousScene::~ContinuousScene()
	{
	}

	void ContinuousScene::onLoad()
	{
		bullet.set(4.0f, 1.0f);
		triangle.set({ Vector2(-1.0f, -2.0f), Vector2(1.0f, -2.0f), Vector2(0.0f, 2.0f) });
		rect.set(1.0f, 1.0f);
		land.set(400.0f, 0.1f);
		capsule.set(1.0f, 2.0f);
		ellipse.set(1.0f, 2.0f);
		circle.setRadius(0.25f);
		edge.set(Vector2(-10.0f, -0.5f), Vector2(10.0f, -0.5f));
		polygon.set({ Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f), Vector2(1.0f, 1.0f) });

		createObjects();
	}

	void ContinuousScene::onUnLoad()
	{

	}

	void ContinuousScene::onUpdate(float deltaTime)
	{
		if (m_simulate)
		{
			float dt = 1.0f / static_cast<float>(m_frequency);
			step(dt);
		}
	}

	void ContinuousScene::onRender(Renderer2D& renderer)
	{
		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			Transform transform{ m_positions[i], m_rotations[i], 1.0f };

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


			Vector2 end = m_positions[i] + m_velocities[i];

			real mag = m_velocities[i].length();

			if (m_showVelocityMagnitude)
			{
				std::string str = std::format("{:.3f}", mag);
				renderer.text(m_positions[i] + Vector2(0.1f, 0.1f), Palette::LightCyan, str);
			}

			if (m_showVelocityColormap)
			{
				Color color = jetColorMap(mag / m_maxVelocityColormap);
				renderer.shape(transform, m_shapes[i], color);
			}

			if (m_showVelocityArrow)
			{
				if (m_showVelocityNormal)
				{
					real length = m_velocities[i].length();
					if (length > 0.01f)
					{
						Vector2 normal = m_velocities[i].normal();
						renderer.arrow(m_positions[i], m_positions[i] + normal, Palette::LightCyan);
					}
				}
				else
				{
					renderer.arrow(m_positions[i], end, Palette::LightCyan);
				}
			}


			if (m_showAngularVelocity)
			{
				std::string str = std::format("{:.3f}", m_angularVelocities[i]);
				renderer.text(m_positions[i] - Vector2(0.1f, 0.1f), Palette::Teal, str);
			}

			if (m_showBulletTrajPrediction)
			{
				if (i != m_bulletId)
					continue;

				Transform tf;
				Transform objectTf{ m_positions[i], m_rotations[i], 1.0f };
				AABB localAABB = AABB::fromShape(tf, m_shapes[i]);
				

				real minX = std::numeric_limits<real>::max();
				real maxX = -std::numeric_limits<real>::max();
				real minY = std::numeric_limits<real>::max();
				real maxY = -std::numeric_limits<real>::max();

				std::vector<Vector2> points = static_cast<ST::Polygon*>(m_shapes[i])->vertices();
				m_maxTrajIndex = points.size() - 1;

				for (int j = 0; j < points.size(); ++j)
				{
					Vector2 point = objectTf.translatePoint(points[j]);
					renderer.point(point, Palette::Yellow);
					minX = Math::min(minX, point.x);
					maxX = Math::max(maxX, point.x);
					minY = Math::min(minY, point.y);
					maxY = Math::max(maxY, point.y);
				}
				Transform stepTf;
				stepTf.position = m_positions[i];
				stepTf.rotation = m_rotations[i];
				Vector2 stepVelocity = m_velocities[i] * m_timeStep;
				real stepRotation = m_angularVelocities[i] * m_timeStep;
				stepTf.position += stepVelocity;
				stepTf.rotation += stepRotation;

				Transform specificTf;
				// lerp
				specificTf.position = m_positions[i] + (stepTf.position - m_positions[i]) * m_t;
				specificTf.rotation = m_rotations[i] + (stepTf.rotation - m_rotations[i]) * m_t;
				specificTf.scale = 1.0f;

				renderer.shape(specificTf, m_shapes[i], Palette::LightBlue);

				if (m_showFeature)
				{
					Transform tfB;
					tfB.position = m_positions[m_objectId];
					tfB.rotation = m_rotations[m_objectId];
					tfB.scale = 1.0f;
					auto distInfo = Narrowphase::distance(objectTf, m_shapes[m_bulletId], tfB, m_shapes[m_objectId]);
					renderer.point(distInfo.pair.pointA, Palette::Yellow);
					renderer.point(distInfo.pair.pointB, Palette::Cyan);

					renderer.dashedLine(distInfo.pair.pointA, distInfo.pair.pointB, Palette::LightGray);
				}

				for (int j = 0; j < points.size(); ++j)
				{
					Vector2 point = stepTf.translatePoint(points[j]);
					renderer.point(point, Palette::Yellow);
					minX = Math::min(minX, point.x);
					maxX = Math::max(maxX, point.x);
					minY = Math::min(minY, point.y);
					maxY = Math::max(maxY, point.y);
				}
				

				for (int j = 0; j < points.size(); ++j)
				{
					Vector2 beforeStepPoint = objectTf.translatePoint(points[j]);
					Vector2 afterStepPoint = stepTf.translatePoint(points[j]);
					renderer.point(afterStepPoint, Palette::Red);
					std::string str = std::format("idx: {}", j);
					
					renderer.dashedArrow(beforeStepPoint, afterStepPoint, Palette::Red);
					renderer.text(beforeStepPoint, Palette::Red, str);

					real x0 = m_positions[i].x;
					real y0 = m_positions[i].y;
					real vx = m_velocities[i].x;
					real vy = m_velocities[i].y;
					real w = m_angularVelocities[i];
					real theta_0 = m_rotations[i];
					real lx = points[j].x;
					real ly = points[j].y;

					auto pt = [](real t, real x0, real y0, real theta0, real vx, real vy, real w, real lx, real ly)
						{
							real x = x0 + vx * t + lx * cos(theta0 + w * t) - ly * sin(theta0 + w * t);
							real y = y0 + vy * t + lx * sin(theta0 + w * t) + ly * cos(theta0 + w * t);
							return Vector2(x, y);
						};


					//render trajectory
					std::vector<Vector2> trajectory;
					std::vector<Vector2> trajX;
					std::vector<Vector2> trajY;
					trajectory.reserve(m_trajSampleCount);
					trajX.reserve(m_trajSampleCount);
					trajY.reserve(m_trajSampleCount);
					for (int k = 0; k <= m_trajSampleCount; ++k)
					{
						real t = static_cast<real>(k) * m_timeStep / static_cast<float>(m_trajSampleCount);
						Vector2 p = pt(t, x0, y0, theta_0, vx, vy, w, lx, ly);
						trajectory.push_back(p);
						trajX.emplace_back(t * 1000.0f, p.x);
						trajY.emplace_back(t * 1000.0f, p.y);
					}

					if (j == m_showTrajIndex)
					{
						renderer.polyLines(trajectory, Palette::Cyan);
						renderer.polyLines(trajX, Palette::Red);
						renderer.polyLines(trajY, Palette::Green);
					}

					if (abs(w) < Constant::TrignometryEpsilon) // no rotation
						continue;

					real Rl = sqrt(lx * lx + ly * ly);
					real period = Constant::DoublePi / abs(w);
					std::vector<real> t_list;

					// solve for dp_x(t)/dt
					real phi = atan2(lx, ly);
					real Phi = acos(vx / (abs(w) * Rl));
					real t_0 = (phi - theta_0 + Phi) / w;
					real t_1 = (phi - theta_0 - Phi) / w;


					auto searchT = [](real t, real period, real timestep, std::vector<real>& t_list)
						{
							int k_min = static_cast<int>(ceil(-t / period));
							int k_max = static_cast<int>(floor((timestep - t) / period));
							for (int k = k_min; k <= k_max; ++k)
							{
								real t_critical = t + k * period;
								if (t_critical > Constant::Epsilon && t_critical < timestep - Constant::Epsilon)
									t_list.emplace_back(t_critical);
							}
						};

					searchT(t_0, period, m_timeStep, t_list);
					searchT(t_1, period, m_timeStep, t_list);

					// solve for dp_y(t)/dt
					phi = atan2(-ly, lx);
					Phi = acos(vy / (abs(w) * Rl));
					t_0 = (phi - theta_0 + Phi) / w;
					t_1 = (phi - theta_0 - Phi) / w;

					searchT(t_0, period, m_timeStep, t_list);
					searchT(t_1, period, m_timeStep, t_list);


					for (int idxT = 0; idxT < t_list.size(); ++idxT)
					{
						real t = t_list[idxT];
						Vector2 p = pt(t, x0, y0, theta_0, vx, vy, w, lx, ly);

						minX = Math::min(minX, p.x);
						maxX = Math::max(maxX, p.x);
						minY = Math::min(minY, p.y);
						maxY = Math::max(maxY, p.y);

						std::string str = std::format("idx: {}, t: {}", j, idxT);
						if (j == m_showTrajIndex)
						{
							renderer.point(p, Palette::Cyan);
							renderer.text(p, Palette::Cyan, str);
						}
						Vector2 q(t * 1000.0f, p.x);
						if (j == m_showTrajIndex)
						{
							renderer.point(q, Palette::Red);
							renderer.text(q, Palette::Red, str);
						}
						q = Vector2(t * 1000.0f, p.y);
						if (j == m_showTrajIndex)
						{
							renderer.point(q, Palette::Green);
							renderer.text(q, Palette::Green, str);
						}
					}

				}


				AABB trajAABB(Vector2(minX, maxY), Vector2(maxX, minY));
				renderer.dashedAABB(trajAABB, Palette::Cyan);
			}

		}
	}

	void ContinuousScene::onRenderUI()
	{
		ImGui::Begin("Continuous");

		ImGui::Checkbox("Simulate", &m_simulate);

		ImGui::SameLine();

		if (ImGui::Button("Step"))
		{
			m_simulate = false;
			step(m_timeStep);
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			m_stepCount = 0;
			reset();
		}

		ImGui::Text("Step Count: %d, Frame Time: %f", m_stepCount, m_currentFrameTime);

		ImGui::DragInt("Frequency", &m_frequency, 1, 240);


		ImGui::Columns(1);
		ImGui::SeparatorText("Visibility");
		ImGui::DragFloat("Max Velocity Colormap", &m_maxVelocityColormap, 0.5f, 1.0f, 100.0f);

		ImGui::Columns(2);
		ImGui::Checkbox("Object", &m_showObject);
		ImGui::Checkbox("Object Id", &m_showObjectID);
		ImGui::Checkbox("Transform", &m_showTransform);
		ImGui::Checkbox("AABB", &m_showAABB);
		ImGui::Checkbox("Grid", &m_showGrid);
		ImGui::NextColumn();
		ImGui::Checkbox("Vel Arrow", &m_showVelocityArrow);
		ImGui::Checkbox("Vel Mag", &m_showVelocityMagnitude);
		ImGui::Checkbox("Vel Colormap", &m_showVelocityColormap);
		ImGui::Checkbox("Vel Normal", &m_showVelocityNormal);
		ImGui::Checkbox("Angular Vel", &m_showAngularVelocity);
		ImGui::Columns(1);

		ImGui::Separator();
		ImGui::Checkbox("Bullet Traj Prediction", &m_showBulletTrajPrediction);
		ImGui::SliderInt("Traj Index", &m_showTrajIndex, 0, m_maxTrajIndex);
		ImGui::Checkbox("Show Feature", &m_showFeature);
		ImGui::DragInt("Traj Sample Count", &m_trajSampleCount, 10, 10, 1000);
		ImGui::DragFloat("Time", &m_t, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat2("Init Vel", &m_initVelocity.x, 10, 0, 5000);
		ImGui::DragFloat("Init Angular Vel", &m_initAngularVelocity, 10.0f, 0, 5000);
		ImGui::End();
	}

	void ContinuousScene::reset()
	{
		m_stepCount = 0;
		m_positions.clear();
		m_rotations.clear();
		m_velocities.clear();
		m_angularVelocities.clear();
		m_shapes.clear();
		m_aabbs.clear();
		m_objectIds.clear();
		m_idIndex = 0;
		createObjects();
	}

	void ContinuousScene::createObjects()
	{
		// create island
		Transform tf{ Vector2(0.0f, 0.0f), 0.0f, 1.0f };
		//m_landId = m_idIndex++;
		//m_objectIds.push_back(m_landId);
		//m_positions.push_back(Vector2(0.0f, 0.0f));
		//m_rotations.push_back(0.0f);
		//m_velocities.push_back(Vector2(0.0f, 0.0f));
		//m_angularVelocities.push_back(0.0f);
		//m_shapes.push_back(&land);
		//m_aabbs.emplace_back(AABB::fromShape(tf, &land));

		// create bullet
		m_objectIds.push_back(m_idIndex++);
		m_bulletId = m_idIndex - 1;
		m_positions.emplace_back(-18.0f, 4.0f);
		m_rotations.push_back(Math::radians(45));
		m_velocities.emplace_back(m_initVelocity);
		m_angularVelocities.push_back(m_initAngularVelocity);
		m_shapes.push_back(&triangle);
		tf.position = m_positions.back();
		tf.rotation = m_rotations.back();
		m_aabbs.emplace_back(AABB::fromShape(tf, &triangle));

		// create rectangle
		m_objectIds.push_back(m_idIndex++);
		m_objectId = m_idIndex - 1;
		m_positions.emplace_back(-4.0f, 4.0f);
		m_rotations.push_back(Math::radians(45));
		m_velocities.emplace_back(0.0f, 1.0f);
		m_angularVelocities.push_back(0.0f);
		m_shapes.push_back(&rect);
		tf.position = m_positions.back();
		tf.rotation = m_rotations.back();
		m_aabbs.emplace_back(AABB::fromShape(tf, &rect));

	}
	void ContinuousScene::step(real dt)
	{
		if (m_enableGravity)
		{
			for (int i = 0; i < m_objectIds.size(); ++i)
			{
				m_velocities[i].y -= 9.8f * dt;
			}
		}
		if (m_enableDamping)
		{
			for (int i = 0; i < m_objectIds.size(); ++i)
			{
				m_velocities[i] *= m_linearVelocityDamping;
				m_angularVelocities[i] *= m_angularVelocityDamping;
			}
		}
		integratePos(dt);
		m_stepCount++;

	}
	void ContinuousScene::integratePos(real dt)
	{
		for (int i = 0; i < m_objectIds.size(); ++i)
		{
			m_positions[i] += m_velocities[i] * dt;
			m_rotations[i] += m_angularVelocities[i] * dt;
			Transform tf{ m_positions[i], m_rotations[i], 1.0f };
			m_aabbs[i] = AABB::fromShape(tf, m_shapes[i]);
		}
	}
}
