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
		capsule.set(2.0f, 4.0f);
		capsule2.set(0.5f, 1.0f);
		circle.setRadius(0.5f);
		radius1.setRadius(0.025f);
		radius2.setRadius(0.05f);

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
		//tf1.position.set(-0.122633040f, -0.174755722f);
		tf1.position.set(0, -0.25);
		tf2.position.set(-0.0f, -1.0f);
		tf1.rotation = Math::radians(15);
		tf2.rotation = Math::radians(90);

		shape1 = &capsule;
		shape2 = &capsule2;
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
		renderer.text(tf1.position, Palette::Yellow, "A", 1);
		renderer.text(tf2.position, Palette::Cyan, "B", 1);

		Color polytopeColor = Palette::Teal;
		Color penetrationColor = Palette::Yellow;
		polytopeColor.a = 150.0f / 255.0f;
		
		auto result = Narrowphase2D::gjk(tf1, shape1, tf2, shape2);

		Color simplexColor = result.isContainOrigin ? Palette::Green : Palette::Purple;

		auto vertices = rect.vertices();
		if (shape1->type() == ShapeType::Polygon)
		{
			Transform transform;
			for (int i = 0;i < vertices.size();++i)
			{
				transform.position = tf1.translatePoint(vertices[i]);
				renderer.circle(transform, &radius1, Palette::Yellow);
			}
		}
		if (shape2->type() == ShapeType::Polygon)
		{
			Transform transform;
			for (int i = 0; i < vertices.size(); ++i)
			{
				transform.position = tf2.translatePoint(vertices[i]);
				renderer.circle(transform, &radius2, Palette::Cyan);
			}
		}

		if (m_showGJKSimplex)
		{
			for (size_t i = 0; i < result.count; ++i)
			{
				renderer.pointFixedSize(result.m[i].p, simplexColor, 6.0f);
			}
			if (result.count == 2)
			{
				Vector2 dir = Narrowphase2D::getDirection(result.m[0].p, result.m[1].p, true);
				Vector2 p = (result.m[0].p + result.m[1].p) * 0.5f;
				renderer.line(p, p + dir, simplexColor);
			}
			else if (result.count == 3)
			{
				Color fillColor = simplexColor * 0.3f;
				fillColor.a = 150.0f / 255.0f;
				renderer.fillAndStroke({ result.m[0].p, result.m[1].p, result.m[2].p }, fillColor, simplexColor);
			}
		}
		if (m_showPolytope)
		{
			if (result.isContainOrigin)
			{
				auto epaResult = Narrowphase2D::epa(result, tf1, shape1, tf2, shape2);
				if (epaResult.state != EpaState::InvalidInput)
				{
					// draw polytope vertices
					for (uint32_t i = 0; i < epaResult.polytope.nVertex; ++i)
					{
						renderer.pointFixedSize(epaResult.polytope.vertices[i].p, polytopeColor, 6.0f);
					}

					// draw polytope active faces
					for (uint32_t i = 0; i < epaResult.polytope.nFace; ++i)
					{
						if (epaResult.polytope.activeFaces[i] == 0)
							continue;
						auto v1 = epaResult.polytope.vertices[epaResult.polytope.faces[i][0]].p;
						auto v2 = epaResult.polytope.vertices[epaResult.polytope.faces[i][1]].p;
						renderer.line(v1, v2, polytopeColor);
						// draw face normal
						//Vector2 faceCenter = (v1 + v2) * 0.5f;
						//Vector2 faceDir = (v2 - v1).normal();
						//Vector2 normalEnd = faceCenter + faceDir * 0.3f;
						//renderer.line(faceCenter, normalEnd, polytopeColor);
					}

					// draw penetration normal
					Vector2 penEnd = epaResult.normal * epaResult.penetration;
					renderer.arrow({}, penEnd, penetrationColor, 0.3f, 30.0f);

					// get witness points
					Vector2 pA = epaResult.simplex.m[2].v[0].v;
					Vector2 pB = epaResult.simplex.m[2].v[1].v;
					//renderer.pointFixedSize(pA, Palette::Yellow, 6.0f);
					//renderer.pointFixedSize(pB, Palette::Cyan, 6.0f);
					//renderer.line(pA, pB, Palette::LightRed);
					
					//renderer.pointFixedSize(epaResult.simplex.m[0].v[0].v, Palette::Yellow, 8.0f);
					//renderer.pointFixedSize(epaResult.simplex.m[1].v[0].v, Palette::Yellow, 8.0f);
					//renderer.pointFixedSize(epaResult.simplex.m[0].v[1].v, Palette::Cyan, 8.0f);
					//renderer.pointFixedSize(epaResult.simplex.m[1].v[1].v, Palette::Cyan, 8.0f);

					auto contacts = Narrowphase2D::generateContacts(epaResult, tf1, shape1, tf2, shape2);
					if (contacts.count > 0)
					{
						for (uint8_t i = 0;i < contacts.count; ++i)
						{
							//real separationCore = Vector2::dot(contacts.pB[i] - contacts.pA[i], contacts.normal);
							//real separationActual = separationCore - (radius1.radius() + radius2.radius());
							//Vector2 contactPoint = contacts.pB[i] + 0.5 * (radius1.radius() - radius2.radius() - separationCore) * contacts.normal;
							//renderer.pointFixedSize(contactPoint, Palette::Green, 6.0f);
							renderer.pointFixedSize(contacts.pA[i], Palette::Yellow, 6.0f);
							renderer.pointFixedSize(contacts.pB[i], Palette::Cyan, 6.0f);
							renderer.thickLine(contacts.pA[i], contacts.pB[i], Palette::Red, 8);
						}
					}
				}
			}
			else
			{
				//do distance check
				//auto distanceResult = Narrowphase2D::distanceRound(tf1, shape1, tf2, shape2, 
				//	radius1.radius(), radius2.radius());
				auto distanceResult = Narrowphase2D::distance(tf1, shape1, tf2, shape2);
				auto& simplex = distanceResult.simplex;
				for (size_t i = 0; i < simplex.count; ++i)
				{
					std::string num = std::format("{}", i);
					renderer.pointFixedSize(simplex.m[i].p, simplexColor, 8.0f);
				}
				if (simplex.count == 2)
				{
					renderer.line(simplex.m[0].p, simplex.m[1].p, simplexColor);
				}
				else if (simplex.count == 3)
				{
					Color fillColor = simplexColor * 0.3f;
					fillColor.a = 150.0f / 255.0f;
					renderer.fillAndStroke({ simplex.m[0].p, simplex.m[1].p, simplex.m[2].p }, fillColor, simplexColor);
				}

				if (distanceResult.distance > 0)
				{
					auto v1 = distanceResult.closestPoints[0];
					auto v2 = distanceResult.closestPoints[1];
					renderer.pointFixedSize(v1, Palette::Yellow, 6.0f);
					renderer.pointFixedSize(v2, Palette::Cyan, 6.0f);
					renderer.line(v1, v2, Palette::LightRed);
				}

			}
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
		float r1 = radius1.radius();
		float r2 = radius2.radius();
		ImGui::SliderFloat("Radius 1", &r1, 0.01, 0.1);
		ImGui::SliderFloat("Radius 2", &r2, 0.01, 0.1);
		radius1.setRadius(r1);
		radius2.setRadius(r2);
		float deg1 = Math::degree(tf1.rotation);
		float deg2 = Math::degree(tf2.rotation);
		ImGui::SliderFloat("Degree 1", &deg1, 0, 360, "%.1f");
		ImGui::SliderFloat("Degree 2", &deg2, 0, 360, "%.1f");
		tf1.rotation = Math::radians(deg1);
		tf2.rotation = Math::radians(deg2);

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
			if (shape1->contains(p1))
			{
				selectedTransform = &tf1;
				oldTransform = tf1;
				mouseStart = mousePos;
			}
			else if (shape2->contains(p2))
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
