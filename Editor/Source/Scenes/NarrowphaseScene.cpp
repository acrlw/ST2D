#include "NarrowphaseScene.h"

namespace STEditor
{
	NarrowphaseScene::~NarrowphaseScene()
	{

	}

	void NarrowphaseScene::onLoad()
	{
		rect.set(1.0f, 1.0f);
		ellipse.set(1.0f, 2.0f);
		capsule.set(1.0f, 2.0f);
		circle.setRadius(1.0f);

		real innerRadius = ellipse.A();
		real outerRadius = ellipse.B();

		if (innerRadius > outerRadius)
		{
			innerRadius = ellipse.B();
			outerRadius = ellipse.A();
		}

		int pointCount = 120;
		real step = Constant::DoublePi / static_cast<float>(pointCount);

		for (real radian = 0; radian <= Constant::DoublePi; radian += step)
		{
			Vector2 point(outerRadius * Math::cosx(radian), innerRadius * Math::sinx(radian));
			m_ellipseVertices.push_back(point);
		}

		m_ellipseVertices.push_back(m_ellipseVertices.front());
		discreteEllipse.set(m_ellipseVertices);

		//bug: -0.122633040, -0.174755722
		tf1.position.set(-0.122633040f, -0.174755722f);
		tf2.position.set(-1.0f, -1.0f);
		tf1.rotation = Math::radians(45.0f);
		tf2.rotation = Math::radians(100.0f);

		shape1 = &rect;
		shape2 = &ellipse;
	}

	void NarrowphaseScene::onUnLoad()
	{
	}

	void NarrowphaseScene::onUpdate(float deltaTime)
	{
		//tf1.rotation += Math::radians(45.0f) * deltaTime;
		//tf1.rotation = std::fmod(tf1.rotation, Constant::DoublePi);
	}

	void NarrowphaseScene::onRender(Renderer2D& renderer)
	{
		renderer.shape(tf1, shape1, Palette::Yellow);
		renderer.shape(tf2, shape2, Palette::Cyan);

		Color simplexColor = Palette::Purple;

		Color polytopeColor = Palette::Teal;
		polytopeColor.a = 150.0f / 255.0f;

		auto gjkSimplex = Narrowphase::gjk(tf1, shape1, tf2, shape2);

		if(selectedTransform != nullptr)
		{
			if (selectedTransform == &tf1)
			{
				Vector2 diff = selectedTransform->position - oldTransform.position;
				if (diff.length() > 0.0f)
				{
					if (m_enableLinearSweep)
					{
						auto volume = Narrowphase::linearSweep(oldTransform, *selectedTransform, shape1);
						renderer.shape(oldTransform, shape1, Palette::Gray);

						Color fill = Palette::Yellow;
						fill.a = 100.0f / 255.0f;

						renderer.fillAndStroke(volume.vertices(), fill, Palette::Yellow);
					}
					else if(m_enableLinearSweepCast)
					{
						Vector2 start = oldTransform.position;
						Vector2 end = selectedTransform->position;
						Vector2 direction = end - start;
						direction.normalize();
						real maxDistance = 10.0f;

						Transform maxTransform, result;
						maxTransform.position = start + direction * maxDistance;
						maxTransform.rotation = oldTransform.rotation;

						bool hit = Narrowphase::linearSweepCast(oldTransform, shape1, tf2, shape2, direction, maxDistance, result);

						renderer.shape(oldTransform, shape1, Palette::Gray);
						renderer.shape(maxTransform, shape1, Palette::LightCyan);
						if(hit)
						{
							renderer.shape(result, shape1, Palette::LightRed);
						}

					}
				}

			}
			else if (selectedTransform == &tf2)
			{
				Vector2 diff = selectedTransform->position - oldTransform.position;
				if (diff.length() > 0.0f)
				{
					if (m_enableLinearSweep)
					{
						auto volume = Narrowphase::linearSweep(oldTransform, *selectedTransform, shape2);
						renderer.shape(oldTransform, shape2, Palette::Gray);

						Color fill = Palette::Cyan;
						fill.a = 150.0f / 255.0f;

						renderer.fillAndStroke(volume.vertices(), fill, Palette::Cyan);
					}
					else if (m_enableLinearSweepCast)
					{
						Vector2 start = oldTransform.position;
						Vector2 end = selectedTransform->position;
						Vector2 direction = end - start;
						direction.normalize();
						real maxDistance = 10.0f;

						Transform maxTransform, result;
						maxTransform.position = start + direction * maxDistance;
						maxTransform.rotation = oldTransform.rotation;

						bool hit = Narrowphase::linearSweepCast(oldTransform, shape2, tf1, shape1, direction, maxDistance, result);

						renderer.shape(oldTransform, shape2, Palette::Gray);
						renderer.shape(maxTransform, shape2, Palette::LightCyan);
						if (hit)
						{
							renderer.shape(result, shape2, Palette::LightRed);
						}
					}
				}
			}
		}

		if (m_showGJKSimplex)
			renderer.simplex(gjkSimplex, Palette::DarkRed);

		if(gjkSimplex.isContainOrigin)
		{
			{
				auto info = Narrowphase::epa(gjkSimplex, tf1, shape1, tf2, shape2);

				auto contacts = Narrowphase::generateContacts(info, tf1, shape1, tf2, shape2);

				renderer.point(contacts.points[0], Palette::Yellow);
				renderer.point(contacts.points[2], Palette::Cyan);
				renderer.dashedLine(contacts.points[0], contacts.points[2], Palette::LightGray);

				//renderer.arrow({}, info.normal, Palette::Red);
				if (contacts.count == 2)
				{
					renderer.point(contacts.points[1], Palette::Yellow);
					renderer.point(contacts.points[3], Palette::Cyan);
					renderer.dashedLine(contacts.points[1], contacts.points[3], Palette::LightGray);
				}

				if (m_showPolytope)
				{
					std::vector<Vector2> points;
					for (auto&& elem : info.polytope)
						points.push_back(elem.vertex.result);

					renderer.polytope(points, polytopeColor);
				}

				if (m_showSimplex)
				{
					renderer.simplex(info.simplex, simplexColor);
				}

			}
		}
		else
		{

			auto distInfo = Narrowphase::distance(tf1, shape1, tf2, shape2);

			renderer.point(distInfo.pair.pointA, Palette::Yellow);
			renderer.point(distInfo.pair.pointB, Palette::Cyan);

			renderer.dashedLine(distInfo.pair.pointA, distInfo.pair.pointB, Palette::LightGray);

			if (m_showPolytope)
			{
				std::vector<Vector2> points;
				for (auto&& elem : distInfo.polytope)
					points.push_back(elem.vertex.result);

				renderer.polytope(points, polytopeColor);
			}

			if (m_showSimplex)
				renderer.simplex(distInfo.simplex, simplexColor);

		}

	}

	void NarrowphaseScene::onRenderUI()
	{
		ImGui::Begin("Narrowphase Scene");

		//ImGui::SliderInt("Polytope Index", &m_currentPolytopeIndex, 0, m_maxPolytopeIndex);
		ImGui::Checkbox("Show Simplex", &m_showSimplex);
		ImGui::Checkbox("Show Polytope", &m_showPolytope);
		ImGui::Checkbox("Show GJK Simplex", &m_showGJKSimplex);
		ImGui::Checkbox("Enable Linear Sweep", &m_enableLinearSweep);
		ImGui::Checkbox("Enable Linear Sweep Cast", &m_enableLinearSweepCast);
		ImGui::End();
	}

	void NarrowphaseScene::onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods)
	{
		

	}

	void NarrowphaseScene::onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			Vector2 mousePos = renderer.screenToWorld({ static_cast<real>(xpos) , static_cast<real>(ypos)});
			Vector2 p1 = tf1.inverseTranslatePoint(mousePos);
			Vector2 p2 = tf2.inverseTranslatePoint(mousePos);
			if (rect.contains(p1))
			{
				selectedTransform = &tf1;
				oldTransform = tf1;
				mouseStart = mousePos;
			}
			else if (ellipse.contains(p2))
			{
				selectedTransform = &tf2;
				oldTransform = tf2;
				mouseStart = mousePos;
			}
		}
		if (action == GLFW_RELEASE)
			selectedTransform = nullptr;
	}

	void NarrowphaseScene::onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos)
	{
		if (selectedTransform != nullptr)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			Vector2 currentMousePos = renderer.screenToWorld({ static_cast<real>(xpos) , static_cast<real>(ypos) });

			selectedTransform->position = oldTransform.position + (currentMousePos - mouseStart);
		}
	}
}
