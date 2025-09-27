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
		shape2 = &capsule;
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


		Color polytopeColor = Palette::Teal;
		Color penetrationColor = Palette::Yellow;
		polytopeColor.a = 150.0f / 255.0f;
		
		auto result = Narrowphase2D::gjk(tf1, shape1, tf2, shape2);

		Color simplexColor = result.isContainOrigin ? Palette::Green : Palette::Purple;

		if (m_showGJKSimplex)
		{
			for (size_t i = 0; i < result.count; ++i)
			{
				renderer.pointFixedSize(result.m[i].p, simplexColor, 6.0f);
				renderer.line(result.m[i].p, result.m[i].p, simplexColor);
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
