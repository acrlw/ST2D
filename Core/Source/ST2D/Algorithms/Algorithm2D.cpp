#include "Algorithm2D.h"

#include "ST2D/Math/Complex.h"
#include "ST2D/Math/Matrix2x2.h"

namespace ST
{
	std::vector<Vector2> Clipper::sutherlandHodgmentPolygonClipping(const std::vector<Vector2>& polygon, const std::vector<Vector2>& clipRegion)
	{
		std::vector<Vector2> result = polygon;
		for (auto iter = clipRegion.begin(); iter != clipRegion.end(); ++iter)
		{
			auto next = iter + 1;
			if (next == clipRegion.end())
				next = clipRegion.begin();

			Vector2 clipPoint1 = *iter;
			Vector2 clipPoint2 = *next;

			if (++next == clipRegion.end())
				next = clipRegion.begin();

			Vector2 clipDirectionPoint = *next;

			std::vector<bool> testResult;
			testResult.reserve(polygon.size());

			for (auto it = result.begin(); it != result.end(); ++it)
				testResult.emplace_back(Algorithm2D::checkPointsOnSameSide(clipPoint1, clipPoint2, clipDirectionPoint, *it));

			std::vector<Vector2> newPolygon;
			newPolygon.reserve(result.size());

			for (auto last = testResult.begin(); last != testResult.end(); ++last)
			{
				auto curr = last + 1;
				if (curr == testResult.end())
					curr = testResult.begin();

				auto idxLast = std::distance(testResult.begin(), last);
				auto idxCurr = std::distance(testResult.begin(), curr);

				//last inside and current outside
				if (*last && !*curr)
				{
					//push last point
					newPolygon.emplace_back(result[idxLast]);
					//push intersection point
					Vector2 p = Algorithm2D::lineIntersection(clipPoint1, clipPoint2, result[idxLast], result[idxCurr]);
					newPolygon.emplace_back(p);
				}
				//last outside and current inside
				else if (!*last && *curr)
				{
					//push intersection point first
					Vector2 p = Algorithm2D::lineIntersection(clipPoint1, clipPoint2, result[idxLast], result[idxCurr]);
					newPolygon.emplace_back(p);
				}
				//last outside and current outside
				else if (!*last && !*curr)
				{
					//do nothing
				}
				else if (*last && *curr)
				{
					newPolygon.emplace_back(result[idxLast]);
				}
			}

			result = newPolygon;
		}
		return result;
	}

	RaycastHit Algorithm2D::raycastCircle(const Vector2& p, const Vector2& dir,
		const Vector2& center, const real& radius)
	{
		RaycastHit result;
		if ((p - center).lengthSquare() < radius * radius)
		{
			return result;
		}


		
		return result;
	}

	RaycastHit Algorithm2D::raycastEllipse(const Vector2& p, const Vector2& dir,
		const Vector2& center, const real& a, const real& b)
	{
		RaycastHit result;


		return result;
	}

	RaycastHit Algorithm2D::raycastCapsule(const Vector2& p, const Vector2& dir,
		const Vector2& center, const real& halfWidth, const real& halfHeight)
	{
		RaycastHit result;


		return result;
	}

	RaycastHit Algorithm2D::raycastSegment(const Vector2& p, const Vector2& dir,
		const Vector2& a, const Vector2& b)
	{
		RaycastHit result;


		return result;
	}

	RaycastHit Algorithm2D::raycastPolygon(const Vector2& p, const Vector2& dir,
		const std::vector<Vector2>& vertices)
	{
		RaycastHit result;


		return result;
	}

	Vector2 Algorithm2D::axialSymmetry(const Vector2& center, const Vector2& dir, const Vector2& point)
	{
		return 2.0 * ((point - center).dot(dir) * dir + center - point) + point;
	}

	bool Algorithm2D::checkCollinear(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		//triangle area = 0 then collinear
		return realEqual(std::fabs((a - b).cross(a - c)), 0);
	}

	bool Algorithm2D::checkPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		const Vector2 ab = b - a;
		const Vector2 ac = c - a;
		const Vector2 bc = c - b;
		return !Math::sameSign(ac.dot(ab), bc.dot(ab)) && checkCollinear(a, b, c);
	}

	bool Algorithm2D::fuzzyCheckPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c,
		const real& epsilon)
	{
		return fuzzyRealEqual(pointToLineSegment(a, b, c).lengthSquare(), epsilon);
	}
	bool Algorithm2D::fuzzyCheckCollinear(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		return (c.x <= Math::max(a.x, b.x) && c.x >= Math::min(a.x, b.x) &&
			c.y <= Math::max(a.y, b.y) && c.y >= Math::min(a.y, b.y));
	}
	bool Algorithm2D::lineSegmentIntersection(const Vector2& a, const Vector2& b,
		const Vector2& c, const Vector2& d, Vector2& result)
	{
		const Vector2 ab = b - a;
		const Vector2 ac = c - a;
		const Vector2 ad = d - a;
		const Vector2 bc = c - b;
		Vector2 ba = a - b;
		const Vector2 bd = d - b;
		const real ab_length = ab.length();

		if (realEqual(ab_length, 0.0))
		{
			if (fuzzyCheckCollinear(c, d, a))
			{
				result = a;
				return true;
			}
			return false;
		}
		const real ab_length_inv = 1 / ab_length;
		const real cc_proj = ab.cross(ac) * ab_length_inv;
		const real dd_proj = ba.cross(bd) * ab_length_inv;
		const real ad_proj = ad.dot(ab.normal());
		const real bc_proj = bc.dot(ba.normal());
		const real cproj_dproj = ab_length - ad_proj - bc_proj;

		if (realEqual(cc_proj, 0.0))
			return false;

		const real denominator = (1 + (dd_proj / cc_proj));
		if (realEqual(denominator, 0.0))
			return false;

		const real cp = cproj_dproj / denominator;
		const Vector2 bp = ba.normalize() * (bc_proj + cp);
		if (realEqual(bp.length(), 0))
			return false;

		Vector2 p = bp + b;

		if (fuzzyCheckCollinear(a, b, p) && fuzzyCheckCollinear(c, d, p))
		{
			result = p;
			return true;
		}

		return false;

	}

	Vector2 Algorithm2D::lineIntersection(const Vector2& p1, const Vector2& p2, const Vector2& q1,
		const Vector2& q2)
	{
		const real d = (p1.x - p2.x) * (q1.y - q2.y) - (p1.y - p2.y) * (q1.x - q2.x);
		if (realEqual(d, 0))
			return {};
		const real x = ((p1.x * p2.y - p1.y * p2.x) * (q1.x - q2.x) - (q1.x * q2.y - q1.y * q2.x) * (p1.x - p2.x)) / d;
		const real y = ((p1.x * p2.y - p1.y * p2.x) * (q1.y - q2.y) - (q1.x * q2.y - q1.y * q2.x) * (p1.y - p2.y)) / d;
		return { x, y };
	}

	bool Algorithm2D::checkConvexPolygon(const std::vector<Vector2>& vertices)
	{
		if (vertices.size() == 3)
			return true;

		int positive = 0;
		int zero = 0;
		int negative = 0;
		for (auto itLast = vertices.begin(); itLast != vertices.end(); ++itLast)
		{
			auto itCurr = itLast + 1;

			if (itCurr == vertices.end())
				itCurr = vertices.begin();

			auto itNext = itCurr + 1;

			if (itNext == vertices.end())
				itNext = vertices.begin();

			Vector2 ab = *itCurr - *itLast;
			Vector2 bc = *itNext - *itCurr;
			const real res = Vector2::crossProduct(ab, bc);
			if (realEqual(res, 0))
				zero++;
			else if (res < 0)
				negative++;
			else
				positive++;

		}
		return !(positive != 0 && negative != 0);
	}

	std::vector<Vector2> Algorithm2D::grahamScan(const std::vector<Vector2>& vertices)
	{
		std::vector<Vector2> points = vertices;
		std::vector<uint32_t> stack;
		std::ranges::sort(points, [](const Vector2& a, const Vector2& b)
		{
			if (atan2l(a.y, a.x) != atan2l(b.y, b.x))
				return atan2l(a.y, a.x) < atan2l(b.y, b.x);
			return a.x < b.x;
		});

		uint32_t targetIndex = 0;
		real targetX = points[0].x;
		for (int i = 1; i < points.size(); ++i)
		{
			if (points[i].x < targetX)
			{
				targetIndex = i;
				targetX = points[i].x;
			}
			if (realEqual(points[i].x, targetX))
				if (points[i].y < points[targetIndex].y)
					targetIndex = i;

		}

		stack.emplace_back(targetIndex);
		stack.emplace_back((targetIndex + 1) % points.size());
		uint32_t k = ((targetIndex + 1) % points.size() + 1) % points.size();
		while (true)
		{
			uint32_t i = stack[stack.size() - 2];
			uint32_t j = stack[stack.size() - 1];
			if (j == targetIndex)
				break;

			k %= points.size();

			Vector2 ab = points[j] - points[i];
			Vector2 bc = points[k] - points[j];
			if (ab.cross(bc) < 0) {
				stack.pop_back();
				if (stack.size() < 2)
				{
					stack.emplace_back(k);
					k++;
				}
				continue;
			}
			stack.emplace_back(k);
			k++;
		}
		std::vector<Vector2> convex;
		convex.reserve(stack.size() - 1);
		for (auto iter = stack.begin(); iter != stack.end() - 1; ++iter)
			convex.emplace_back(points[*iter]);

		return convex;
	}


	Vector2 Algorithm2D::shortestLengthPointOfEllipse(const real& a, const real& b, const Vector2& p,
		const real& epsilon)
	{
		if (realEqual(a, 0) || realEqual(b, 0))
			return {};

		if (realEqual(p.x, 0))
		{
			return p.y > 0
				? Vector2{ 0, b }
			: Vector2{ 0, -b };
		}
		if (realEqual(p.y, 0))
		{
			return p.x > 0
				? Vector2{ a, 0 }
			: Vector2{ -a, 0 };
		}

		real x_left, x_right;
		Vector2 t0, t1;
		const int sgn = p.y > 0 ? 1 : -1;
		if (p.x < 0)
		{
			x_left = -a;
			x_right = 0;
		}
		else
		{
			x_left = 0;
			x_right = a;
		}
		int iteration = 0;
		while (++iteration)
		{
			real temp_x = (x_left + x_right) * 0.5f;
			real temp_y = sgn * sqrt(pow(b, 2.0f) - pow(b / a, 2.0f) * pow(temp_x, 2.0f));
			Vector2 t0(temp_x, temp_y);
			t0.set(temp_x, temp_y);
			real t1_x = temp_x + 1;
			real t1_y = (pow(b, 2.0f) - pow(b / a, 2.0f) * t1_x * temp_x) / temp_y;
			t1.set(t1_x, t1_y);
			Vector2 t0t1 = t1 - t0;
			Vector2 t0p = p - t0;

			const real result = t0t1.dot(t0p);
			if (std::fabs(result) < epsilon)
				break;

			if (result > 0) // acute angle
				x_left = temp_x;
			else
				x_right = temp_x; //obtuse angle
		}
		return t0;
	}

	Vector2 Algorithm2D::triangleCentroid(const Vector2& a1, const Vector2& a2, const Vector2& a3)
	{
		return Vector2(a1 + a2 + a3) / 3.0f;
	}

	real Algorithm2D::triangleArea(const Vector2& a1, const Vector2& a2, const Vector2& a3)
	{
		return std::fabs(Vector2::crossProduct(a1 - a2, a1 - a3)) / 2.0f;
	}

	Vector2 Algorithm2D::computeCenter(const std::vector<Vector2>& vertices)
	{
		if (vertices.size() >= 3)
		{
			Vector2 pos;
			real area = 0;
			for (auto itLast = vertices.begin(); itLast != vertices.end() - 1; ++itLast)
			{
				auto itCurr = itLast + 1;
				auto itNext = itCurr + 1;
				if (itNext == vertices.end())
					itNext = vertices.begin();

				real a = triangleArea(*itLast, *itCurr, *itNext);
				Vector2 p = triangleCentroid(*itLast, *itCurr, *itNext);
				pos += p * a;
				area += a;

			}
			pos /= area;
			return pos;
		}
		return Vector2();
	}

	Vector2 Algorithm2D::computeCenter(const std::list<Vector2>& vertices)
	{
		if (vertices.size() >= 3)
		{
			Vector2 pos;
			real area = 0;
			auto end = std::prev(vertices.end());
			for (auto itLast = vertices.begin(); itLast != end; ++itLast)
			{
				auto itCurr = std::next(itLast);
				auto itNext = std::next(itCurr);
				if (itNext == vertices.end())
					itNext = vertices.begin();
				real a = triangleArea(*itLast, *itCurr, *itNext);
				Vector2 p = triangleCentroid(*itLast, *itCurr, *itNext);
				pos += p * a;
				area += a;
			}
			pos /= area;
			return pos;
		}
		return Vector2();
	}

	Vector2 Algorithm2D::computeCenter(Vector2* vertices, const uint32_t& count)
	{
		if (vertices == nullptr || count < 3)
			return {};

		Vector2 pos;
		real area = 0;
		for (int i = 0; i < count - 1; ++i)
		{
			const Vector2& a1 = vertices[i];
			const Vector2& a2 = vertices[i + 1];
			const Vector2& a3 = vertices[(i + 2) % count];
			real a = triangleArea(a1, a2, a3);
			Vector2 p = triangleCentroid(a1, a2, a3);
			pos += p * a;
			area += a;
		}

		pos /= area;
		return pos;
	}

	PointsResult Algorithm2D::shortestLengthLineSegmentEllipse(
		const real& a, const real& b, const Vector2& p1, const Vector2& p2)
	{
		Vector2 p_line;
		Vector2 p_ellipse;
		if (realEqual(p1.y, p2.y))
		{
			if (!((p1.x > 0 && p2.x > 0) || (p1.x < 0 && p2.x < 0))) // different quadrant
			{
				p_ellipse.set(0, p1.y > 0 ? b : -b);
				p_line.set(0, p1.y);
			}
			else
			{
				p_line.set(std::fabs(p1.x) > std::fabs(p2.x) ? p2.x : p1.x, p1.y);
				p_ellipse = shortestLengthPointOfEllipse(a, b, p_line);
			}
		}
		else if (realEqual(p1.x, p2.x))
		{
			if (!((p1.y > 0 && p2.y > 0) || (p1.y < 0 && p2.y < 0))) // different quadrant
			{
				p_ellipse.set(p1.x > 0 ? a : -a, 0);
				p_line.set(p1.x, 0);
			}
			else
			{
				p_line.set(p1.x, std::fabs(p1.y) > std::fabs(p2.y) ? p2.y : p1.y);
				p_ellipse = shortestLengthPointOfEllipse(a, b, p_line);
			}
		}
		else
		{
			//calculate tangent line
			const real k = (p2.y - p1.y) / (p2.x - p1.x);
			const real k2 = k * k;
			const real a2 = a * a;
			const real b2 = b * b;
			const real f_x2 = (k2 * a2 * a2 / b2) / (1 + a2 * k2 / b2);
			const real f_y2 = b2 - b2 * f_x2 / a2;
			const real f_x = sqrt(f_x2);
			const real f_y = sqrt(f_y2);
			Vector2 f;
			const Vector2 p1p2 = (p2 - p1).normal();

			//Check which quadrant does nearest point fall in
			{
				Vector2 f_arr[4];
				f_arr[0].set(f_x, f_y);
				f_arr[1].set(-f_x, f_y);
				f_arr[2].set(-f_x, -f_y);
				f_arr[3].set(f_x, -f_y);
				real min = Vector2::crossProduct(p1p2, f_arr[0] - p1);
				for (int i = 1; i < 4; i++)
				{
					const real value = Vector2::crossProduct(p1p2, f_arr[i] - p1);
					if (min > value)
					{
						f = f_arr[i];
						min = value;
					}
				}
			}

			const Vector2 p1f = f - p1;
			const Vector2 p1_fp = p1p2 * p1p2.dot(p1f);
			const Vector2 f_proj = p1 + p1_fp;

			if (fuzzyCheckCollinear(a, b, f_proj))
			{
				p_ellipse = f;
				p_line = f_proj;
			}
			else
			{
				const Vector2 p1_p = shortestLengthPointOfEllipse(a, b, p1);
				const Vector2 p2_p = shortestLengthPointOfEllipse(a, b, p2);
				if ((p1 - p1_p).lengthSquare() > (p2 - p2_p).lengthSquare())
				{
					p_ellipse = p2_p;
					p_line = p2;
				}
				else
				{
					p_ellipse = p1_p;
					p_line = p1;
				}
			}
		}
		return { p_ellipse, p_line };
	}

	bool Algorithm2D::raycast(const Vector2& p, const Vector2& dir, const Vector2& a,
		const Vector2& b, Vector2& result)
	{
		const real denominator = (p.x - dir.x) * (a.y - b.y) - (p.y - dir.y) * (a.x - b.x);

		if (realEqual(denominator, 0))
			return false;

		const real t = ((p.x - a.x) * (a.y - b.y) - (p.y - a.y) * (a.x - b.x)) / denominator;
		const real u = ((dir.x - p.x) * (p.y - a.y) - (dir.y - p.y) * (p.x - a.x)) / denominator;
		if (t >= 0 && u <= 1.0 && u >= 0)
		{
			result = p + t * (dir - p);
			return true;
		}
		return false;
	}


	bool Algorithm2D::raycastAABB(const Vector2& p, const Vector2& dir, const Vector2& topLeft, const Vector2& bottomRight, Vector2& enter, Vector2& exit)
	{
		const real xmin = topLeft.x;
		const real xmax = bottomRight.x;
		const real ymin = bottomRight.y;
		const real ymax = topLeft.y;
		real txmin, txmax, tymin, tymax;
		real txenter, txexit, tyenter, tyexit;
		real tenter, texit;
		if (realEqual(dir.x, 0) && !realEqual(dir.y, 0))
		{
			tymin = (ymin - p.y) / dir.y;
			tymax = (ymax - p.y) / dir.y;
			tenter = Math::min(tymin, tymax);
			texit = Math::max(tymin, tymax);
		}
		else if (!realEqual(dir.x, 0) && realEqual(dir.y, 0))
		{

			txmin = (xmin - p.x) / dir.x;
			txmax = (xmax - p.x) / dir.x;
			tenter = Math::min(txmin, txmax);
			texit = Math::max(txmin, txmax);
		}
		else
		{
			txmin = (xmin - p.x) / dir.x;
			txmax = (xmax - p.x) / dir.x;
			tymin = (ymin - p.y) / dir.y;
			tymax = (ymax - p.y) / dir.y;
			txenter = Math::min(txmin, txmax);
			txexit = Math::max(txmin, txmax);
			tyenter = Math::min(tymin, tymax);
			tyexit = Math::max(tymin, tymax);
			tenter = Math::max(txenter, tyenter);
			texit = Math::min(txexit, tyexit);
		}
		if (tenter < 0 && texit < 0)
			return false;

		enter = p + tenter * dir;
		exit = p + texit * dir;

		return true;

	}

	bool Algorithm2D::checkPointInsideAABB(const Vector2&pos, const Vector2& topLeft, const Vector2& bottomRight)
	{
		return !(pos.x > bottomRight.x || pos.x < topLeft.x && pos.y > topLeft.y || pos.y < bottomRight.y);
	}

	bool Algorithm2D::checkPointOnAABB(const Vector2& p, const Vector2& topLeft, const Vector2& bottomRight)
	{
		return Math::isInRange(p.x, topLeft.x, bottomRight.x) &&
			Math::isInRange(p.y, bottomRight.y, topLeft.y);
	}

	Vector2 Algorithm2D::rotateAround(const Vector2& p, const Vector2& center, const real& radians)
	{
		return Complex(radians).multiply(p - center) + center;
	}

	Vector2 Algorithm2D::computeEllipseProjectionPoint(const real& a, const real& b, const Vector2& direction)
	{
		Vector2 target;
		if (realEqual(direction.x, 0))
		{
			const int sgn = direction.y < 0 ? -1 : 1;
			target.set(0, sgn * b);
		}
		else if (realEqual(direction.y, 0))
		{
			const int sgn = direction.x < 0 ? -1 : 1;
			target.set(sgn * a, 0);
		}
		else
		{
			const real k = direction.y / direction.x;
			//line offset constant d
			const real a2 = pow(a, 2.0f);
			const real b2 = pow(b, 2.0f);
			const real k2 = pow(k, 2.0f);
			real d = sqrt((a2 + b2 * k2) / k2);
			if (Vector2::dotProduct(Vector2(0, d), direction) < 0)
				d = d * -1;
			const real x1 = k * d - (b2 * k2 * k * d) / (a2 + b2 * k2);
			const real y1 = (b2 * k2 * d) / (a2 + b2 * k2);
			target.set(x1, y1);
		}
		return target;
	}
	Vector2 Algorithm2D::computeCapsuleProjectionPoint(const real& halfWidth, const real& halfHeight, const Vector2& direction)
	{
		Vector2 target;
		if (halfWidth >= halfHeight) // Horizontal
		{
			const real radius = halfHeight;
			const real offset = direction.x >= 0 ? halfWidth - radius : radius - halfWidth;
			target = direction.normal() * radius;
			target.x += offset;
		}
		else // Vertical
		{
			const real radius = halfWidth;
			const real offset = direction.y >= 0 ? halfHeight - radius : radius - halfHeight;
			target = direction.normal() * radius;
			target.y += offset;
		}
		return target;
	}

	Vector2 Algorithm2D::computeSectorProjectionPoint(const real& startRadian, const real& spanRadian,
		const real& radius, const Vector2& direction)
	{
		Vector2 result;
		auto clampRadian = [](const real& radian)
			{
				real result = radian;
				result -= std::floor(result / Constant::DoublePi) * Constant::DoublePi;
				if (result < 0)
					result += Constant::DoublePi;
				return result;
			};

		const real clampStart = clampRadian(startRadian);
		const real clampEnd = clampRadian(startRadian + spanRadian);
		const real originStart = clampRadian(startRadian - Constant::HalfPi);
		const real originEnd = clampRadian(startRadian + spanRadian + Constant::HalfPi);

		const real originTheta = direction.theta();
		real theta = clampRadian(originTheta);

		if (originStart > originEnd)
		{
			//does not fall in zero area
			if (!Math::isInRange(theta, originEnd, originStart))
			{
				if (theta > originStart)
					theta = originTheta;

				//clamp theta to sector area

				result = Matrix2x2(Math::clamp(theta, clampStart, clampEnd)).multiply(Vector2{ 1, 0 }) * radius;
			}
		}
		if (originStart < originEnd)
		{
			if (Math::isInRange(originTheta, originStart, originEnd))
			{
				//clamp theta to sector area
				result = Matrix2x2(Math::clamp(theta, clampStart, clampEnd)).multiply(Vector2{ 1, 0 }) * radius;
			}
		}
		//special case for half circle
		if (fuzzyRealEqual(originStart, originEnd))
		{
			if (!fuzzyRealEqual(theta, originStart))
			{
				if (theta > originStart)
					theta = originTheta;
				if (clampStart > clampEnd)
					result = Matrix2x2(Math::clamp(theta, clampStart - Constant::DoublePi, clampEnd)).multiply(Vector2{ 1, 0 }) * radius;
				else
					result = Matrix2x2(Math::clamp(theta, clampStart, clampEnd)).multiply(Vector2{ 1, 0 }) * radius;
			}
		}
		return result;
	}

	bool Algorithm2D::checkOriginInTriangle(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		real ra = (b - a).cross(-a);
		real rb = (c - b).cross(-b);
		real rc = (a - c).cross(-c);
		return Math::sameSign(ra, rb, rc);
	}
	bool Algorithm2D::checkPointsOnSameSide(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refPoint, const Vector2 targetPoint)
	{
		Vector2 u = edgePoint2 - edgePoint1;
		Vector2 v = refPoint - edgePoint1;
		Vector2 w = targetPoint - edgePoint1;
		//same side or on the edge
		return Math::sameSign(u.cross(v), u.cross(w));
	}
	Vector2 Algorithm2D::lineSegmentNormal(const Vector2& edgePoint1, const Vector2& edgePoint2, const Vector2& refDirection)
	{
		Vector2 normal = (edgePoint2 - edgePoint1).normal().perpendicular();
		if (refDirection.dot(normal) < 0)
			normal.negate();
		return normal;
	}
	Vector2 Algorithm2D::pointToLineSegment(const Vector2& a, const Vector2& b, const Vector2& p)
	{
		if (a == b)
			return a;

		const Vector2 ab = b - a;
		const Vector2 ap = p - a;
		const Vector2 bp = p - b;

		if (ap.dot(ab) <= 0)
			return a;

		if (bp.dot(ab) >= 0)
			return b;

		const Vector2 ab_normal = ab.normal();
		const real proj = ap.dot(ab_normal);
		const Vector2 ap_proj = proj * ab_normal;
		const Vector2 op_proj = a + ap_proj;
		//return point p_proj
		return op_proj;
	}
	Vector2 Algorithm2D::rayRayIntersectionUnsafe(const Vector2& p1, const Vector2& dir1, const Vector2& p2, const Vector2& dir2)
	{
		//https://stackoverflow.com/a/2932601

		const real det = dir1.y * dir2.x - dir1.x * dir2.y;
		//assert(!realEqual(det, 0));
		const real u = (dir2.x * (p2.y - p1.y) - dir2.y * (p2.x - p1.x)) / det;
		return p1 + dir1 * u;
	}
}
