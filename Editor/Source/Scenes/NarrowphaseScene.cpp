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

		int pointCount = 60;
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
		renderer.shape(tf1, &rect, Palette::Yellow);
		renderer.shape(tf2, &ellipse, Palette::Cyan);

		Color simplexColor = Palette::Purple;

		Color polytopeColor = Palette::Teal;
		polytopeColor.a = 150.0f / 255.0f;

		auto gjkSimplex = Narrowphase::gjk(tf1, &rect, tf2, &ellipse);

		if (m_showGJKSimplex)
			renderer.simplex(gjkSimplex, Palette::Green);

		if(gjkSimplex.isContainOrigin)
		{
			{
				auto info = Narrowphase::findClosestSimplex(gjkSimplex, tf1, &rect, tf2, &ellipse);

				auto contacts = Narrowphase::generateContacts(info, tf1, &rect, tf2, &ellipse);

				renderer.point(contacts.points[0], Palette::Yellow);
				renderer.point(contacts.points[1], Palette::Cyan);
				renderer.dashedLine(contacts.points[0], contacts.points[1], Palette::LightGray);

				//renderer.arrow({}, info.normal, Palette::Red);
				if (contacts.count == 4)
				{
					renderer.point(contacts.points[2], Palette::Yellow);
					renderer.point(contacts.points[3], Palette::Cyan);
					renderer.dashedLine(contacts.points[2], contacts.points[3], Palette::LightGray);
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

			//{
			//	CollisionInfo info;
			//	info.simplex = gjkSimplex;
			//	info.simplex.removeEnd();

			//	renderer.simplex(info.simplex, simplexColor);

			//	for (Index iter = 0; iter < 1; ++iter)
			//	{
			//		//indices of closest edge are set to 0 and 1
			//		const Vector2 direction = Narrowphase::findDirectionByEdge(info.simplex.vertices[0], info.simplex.vertices[1], false);

			//		const SimplexVertex vertex = Narrowphase::support(tf1, &rect, tf2, &discreteEllipse, direction);

			//		renderer.point(vertex.result, Palette::Orange);
			//		renderer.arrow({}, direction, Palette::Red);

			//		//cannot find any new vertex
			//		if (info.simplex.contains(vertex))
			//			break;

			//		//check if new vertex is located in support direction

			//		bool validSide = Algorithm2D::checkPointsOnSameSide(info.simplex.vertices[0].result,
			//			info.simplex.vertices[1].result, info.simplex.vertices[0].result + direction,
			//			vertex.result);

			//		Vector2 ab = info.simplex.vertices[1].result - info.simplex.vertices[0].result;
			//		Vector2 ac = vertex.result - info.simplex.vertices[0].result;
			//		Vector2 bc = vertex.result - info.simplex.vertices[1].result;

			//		bool validVoronoi = ab.dot(ac) > 0.0f && -ab.dot(bc) > 0.0f;

			//		if (!validSide || !validVoronoi)
			//			break;


			//	}
			//}
		}
		else
		{

			auto distInfo = Narrowphase::distance(tf1, &rect, tf2, &ellipse);

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

		//std::vector<std::vector<Vector2>> polytopes;
		//std::vector<Simplex> simplexes;

		//{
		//	VertexPair result;
		//	CollisionInfo info;

		//	Vector2 direction = tf2.position - tf1.position;

		//	if (direction.fuzzyEqual({ 0, 0 }))
		//		direction.set(1, 1);
		//	//first
		//	SimplexVertex vertex = Narrowphase::support(tf1, &rect, tf2, &ellipse, direction);
		//	info.simplex.addSimplexVertex(vertex);
		//	//second
		//	direction.negate();
		//	vertex = Narrowphase::support(tf1, &rect, tf2, &ellipse, direction);
		//	info.simplex.addSimplexVertex(vertex);
		//	//third
		//	direction = direction.perpendicular();
		//	vertex = Narrowphase::support(tf1, &rect, tf2, &ellipse, direction);
		//	info.simplex.addSimplexVertex(vertex);

		//	Narrowphase::reconstructSimplexByVoronoi(info.simplex);
		//	info.originalSimplex = info.simplex;

		//	simplexes.push_back(info.simplex);
		//	//renderer.simplex(info.simplex, Palette::Purple);

		//	//[DEBUG]
		//	std::list<SimplexVertexWithOriginDistance>& polytope = info.polytope;
		//	//std::list<SimplexVertexWithOriginDistance> polytope;

		//	Narrowphase::buildPolytopeFromSimplex(polytope, info.simplex);

		//	auto iterStart = polytope.begin();
		//	auto iterEnd = polytope.end();
		//	auto iterTemp = polytope.begin();

		//	int errorCount = 0;

		//	auto reindexSimplex = [&info, &polytope, &iterStart, &iterEnd, &iterTemp]
		//		{
		//			std::swap(info.simplex.vertices[1], info.simplex.vertices[2]);
		//			std::swap(info.simplex.vertices[0], info.simplex.vertices[1]);
		//			polytope.clear();
		//			Narrowphase::buildPolytopeFromSimplex(polytope, info.simplex);
		//			iterStart = polytope.begin();
		//			iterEnd = polytope.end();
		//			iterTemp = polytope.begin();
		//		};

		//	int sameDistCount = 0;

		//	std::vector<Vector2> points;
		//	for (auto&& elem : info.polytope)
		//		points.push_back(elem.vertex.result);

		//	polytopes.push_back(points);

		//	for (Index iter = 0; iter < 20; ++iter)
		//	{
		//		//indices of closest edge are set to 0 and 1
		//		direction = Narrowphase::findDirectionByEdge(info.simplex.vertices[0], info.simplex.vertices[1], true);

		//		vertex = Narrowphase::support(tf1, &rect, tf2, &ellipse, direction);

		//		//cannot find any new vertex
		//		if (info.simplex.contains(vertex))
		//		{
		//			if (sameDistCount == 1)
		//			{
		//				//check edge case
		//				Narrowphase::polytopeIterNext(iterStart, polytope);
		//				iterEnd = iterStart;
		//				Narrowphase::polytopeIterPrev(iterEnd, polytope);
		//				iterTemp = iterStart;
		//				Narrowphase::polytopeIterNext(iterTemp, polytope);

		//				info.simplex.vertices[0] = iterStart->vertex;
		//				info.simplex.vertices[1] = iterTemp->vertex;
		//				iter--;
		//				//do not process anymore
		//				sameDistCount = -1;
		//				continue;
		//			}
		//			if (polytope.size() >= 4) //polytope has been expanded, terminate the loop
		//				break;

		//			if (errorCount == 3) //fail to rewind simplex, terminate
		//				break;

		//			reindexSimplex();

		//			iter--;

		//			errorCount++;

		//			continue;
		//		}
		//		//convex test, make sure polytope is always convex

		//		auto itA = iterStart;

		//		auto itB = itA;
		//		Narrowphase::polytopeIterNext(itB, polytope);

		//		auto itC = itB;
		//		Narrowphase::polytopeIterNext(itC, polytope);

		//		const Vector2 ab = itB->vertex.result - itA->vertex.result;
		//		const Vector2 bc = itC->vertex.result - itB->vertex.result;
		//		const real res1 = Vector2::crossProduct(ab, bc);

		//		const Vector2 an = vertex.result - itA->vertex.result;
		//		const Vector2 nb = itB->vertex.result - vertex.result;
		//		const real res2 = Vector2::crossProduct(an, nb);

		//		const real res3 = Vector2::crossProduct(nb, bc);

		//		const bool validConvexity = Math::sameSignStrict(res1, res2, res3);

		//		if (!validConvexity) //invalid vertex
		//		{
		//			if (polytope.size() >= 4) //if polytope is expanded, terminate the loop
		//				break;

		//			if (errorCount == 3) //fail to rewind simplex, terminate
		//				break;

		//			//try to rewind
		//			reindexSimplex();
		//			errorCount++;
		//			iter--;
		//			continue;
		//		}

		//		//then insert new vertex

		//		SimplexVertexWithOriginDistance pair;
		//		pair.vertex = vertex;
		//		const Vector2 t1 = Algorithm2D::pointToLineSegment(itA->vertex.result, vertex.result, { 0, 0 });
		//		const real dist1 = t1.lengthSquare();
		//		const Vector2 t2 = Algorithm2D::pointToLineSegment(vertex.result, itB->vertex.result, { 0, 0 });
		//		const real dist2 = t2.lengthSquare();

		//		itA->distance = dist1;
		//		pair.distance = dist2;
		//		polytope.insert(itB, pair);

		//		points.clear();
		//		for (auto&& elem : info.polytope)
		//			points.push_back(elem.vertex.result);

		//		polytopes.push_back(points);

		//		//TODO: if dist1 == dist2, and dist1 cannot be extended and dist2 can be extended.
		//		if (dist1 == dist2)
		//			sameDistCount++;
		//		

		//		//set to begin
		//		iterTemp = iterStart;

		//		//find shortest distance and set iterStart
		//		real minDistance = Constant::Max;
		//		auto iterTarget = iterStart;
		//		while (true)
		//		{
		//			if (iterTemp->distance < minDistance)
		//			{
		//				minDistance = iterTemp->distance;
		//				iterTarget = iterTemp;
		//			}
		//			Narrowphase::polytopeIterNext(iterTemp, polytope);
		//			if (iterTemp == iterStart)
		//				break;
		//		}
		//		iterStart = iterTarget;
		//		iterEnd = iterTarget;
		//		Narrowphase::polytopeIterPrev(iterEnd, polytope);

		//		//set to begin
		//		iterTemp = iterStart;
		//		Narrowphase::polytopeIterNext(iterTemp, polytope);
		//		//reset simplex
		//		info.simplex.vertices[0] = iterStart->vertex;
		//		info.simplex.vertices[1] = iterTemp->vertex;

		//		simplexes.push_back(info.simplex);

		//		errorCount = 0;
		//	}


		//	info.simplex.removeEnd();
		//	//Convex combination for calculating distance points
		//	//https://dyn4j.org/2010/04/gjk-distance-closest-points/

		//	Vector2& A_0 = info.simplex.vertices[0].point[0];
		//	Vector2& B_0 = info.simplex.vertices[0].point[1];
		//	Vector2& A_1 = info.simplex.vertices[1].point[0];
		//	Vector2& B_1 = info.simplex.vertices[1].point[1];

		//	Vector2 M_0 = info.simplex.vertices[0].result;
		//	Vector2 M_1 = info.simplex.vertices[1].result;

		//	Vector2 M0_M1 = M_1 - M_0;
		//	real M0_O_proj = -M_0.dot(M0_M1);
		//	real M0_M1_proj = M0_M1.dot(M0_M1);
		//	real v = M0_O_proj / M0_M1_proj;
		//	real u = 1 - v;

		//	result.pointA.set(u * A_0 + v * A_1);
		//	result.pointB.set(u * B_0 + v * B_1);

		//	if (M0_M1.fuzzyEqual({ 0, 0 }) || v < 0)
		//	{
		//		result.pointA.set(A_0);
		//		result.pointB.set(B_0);
		//	}
		//	else if (u < 0)
		//	{
		//		result.pointA.set(A_1);
		//		result.pointB.set(B_1);
		//	}
		//	info.pair = result;
		//	info.penetration = (result.pointA - result.pointB).length();
		//}



		//m_maxPolytopeIndex = polytopes.size() - 1;

		//if (m_maxPolytopeIndex < 0)
		//	m_maxPolytopeIndex = 0;

		//if(m_showPolytope)
		//	renderer.polytope(polytopes[m_currentPolytopeIndex], Palette::Teal);

		//if (m_showSimplex)
		//	renderer.simplex(simplexes[m_currentPolytopeIndex], simplexColor);

		//std::vector<Vector2> points;
		//for(auto&& elem: info.polytope)
		//	points.push_back(elem.vertex.result);
		//
		//renderer.polytope(points, polytopeColor);

		//renderer.simplex(info.simplex, simplexColor);



	}

	void NarrowphaseScene::onRenderUI()
	{
		ImGui::Begin("Narrowphase Scene");

		//ImGui::SliderInt("Polytope Index", &m_currentPolytopeIndex, 0, m_maxPolytopeIndex);
		ImGui::Checkbox("Show Simplex", &m_showSimplex);
		ImGui::Checkbox("Show Polytope", &m_showPolytope);
		ImGui::Checkbox("Show GJK Simplex", &m_showGJKSimplex);

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
