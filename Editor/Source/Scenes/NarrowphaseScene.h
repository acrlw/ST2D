#pragma once

#include "AbstractScene.h"

namespace STEditor
{
	class NarrowphaseScene : public AbstractScene
	{
	public:
		explicit NarrowphaseScene()
			: AbstractScene("NarrowphaseScene")
		{
		}

		~NarrowphaseScene() override;
		void onLoad() override;
		void onUnLoad() override;
		void onUpdate(float deltaTime) override;
		void onRender(Renderer2D& renderer) override;
		void onRenderUI() override;
		void onKeyButton(GLFWwindow* window, Renderer2D& renderer, int key, int scancode, int action, int mods) override;
		void onMouseButton(GLFWwindow* window, Renderer2D& renderer, int button, int action, int mods) override;
		void onMouseMoved(GLFWwindow* window, Renderer2D& renderer, double xpos, double ypos) override;

	private:
		ST::Polygon polygon1;
		ST::Polygon polygon2;
		ST::Rectangle rect;
		ST::Ellipse ellipse;
		ST::Capsule capsule;
		ST::Capsule capsule2;
		ST::Circle circle;

		AbstractShape* shape1 = nullptr;
		AbstractShape* shape2 = nullptr;
		Transform2D tf1, tf2;
		Transform2D* selectedTransform = nullptr;
		Transform2D oldTransform;
		Vector2 mouseStart;

		//int m_currentPolytopeIndex = 0;
		//int m_maxPolytopeIndex = 0;
		bool m_showSimplex = false;
		bool m_showPolytope = false;
		bool m_showGJKSimplex = false;
		bool m_enableLinearSweep = false;
		bool m_enableLinearSweepCast = true;
		std::vector<Vector2> m_ellipseVertices;

		ST::Circle radius1;
		ST::Circle radius2;
		std::array<AbstractShape*, 7> m_shapes;
		std::array<ShapeBase*, 5> m_shapeBaseArray;

	};
}