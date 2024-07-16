#include "SweepAndPrune.h"

namespace ST
{
	std::vector<std::pair<ShapePrimitive*, ShapePrimitive*>> SweepAndPrune::generate(const std::vector<ShapePrimitive*>& bodyList)
	{
		std::vector<std::pair<ShapePrimitive*, ShapePrimitive*>> result;
		//sort by x axis
		std::vector<std::pair<ShapePrimitive*, AABB>> bodyBoxPairList;
		bodyBoxPairList.reserve(bodyList.size());

		for (auto&& elem : bodyList)
		{
			AABB aabb = AABB::fromShape(*elem);
			bodyBoxPairList.emplace_back(std::make_pair(elem, aabb));
		}

		std::vector<std::pair<ShapePrimitive*, AABB>> sortXAxis = bodyBoxPairList;
		std::vector<std::pair<ShapePrimitive*, AABB>> sortYAxis = bodyBoxPairList;
		std::sort(sortXAxis.begin(), sortXAxis.end(), [](const std::pair<ShapePrimitive*, AABB>& left, const std::pair<ShapePrimitive*, AABB>& right)
			{
				return left.second.minimumX() < right.second.minimumX();
			});
		std::sort(sortYAxis.begin(), sortYAxis.end(), [](const std::pair<ShapePrimitive*, AABB>& left, const std::pair<ShapePrimitive*, AABB>& right)
			{
				return left.second.minimumY() < right.second.minimumY();
			});


		std::vector<ShapePair> xPairs;
		std::vector<ShapePair> yPairs;

		for (auto before = sortXAxis.begin(); before != sortXAxis.end(); ++before)
		{
			for (auto next = std::next(before); next != sortXAxis.end(); ++next)
			{
				const real minBefore = before->second.minimumX();
				const real maxBefore = before->second.maximumX();
				const real minNext = next->second.minimumX();
				const real maxNext = next->second.maximumX();

				if (!(maxBefore < minNext || maxNext < minBefore))
				{
					xPairs.emplace_back(ShapePair::makeShapePair(before->first, next->first));
				}
				else
					break;
			}
		}

		for (auto before = sortYAxis.begin(); before != sortYAxis.end(); ++before)
		{
			for (auto next = std::next(before); next != sortYAxis.end(); ++next)
			{
				const real minBefore = before->second.minimumY();
				const real maxBefore = before->second.maximumY();
				const real minNext = next->second.minimumY();
				const real maxNext = next->second.maximumY();

				if (!(maxBefore < minNext || maxNext < minBefore))
				{
					yPairs.emplace_back(ShapePair::makeShapePair(before->first, next->first));
				}
				else
					break;
			}
		}

		std::sort(xPairs.begin(), xPairs.end(), [](const ShapePair& left, const ShapePair& right)
			{
				return left.id < right.id;
			});
		std::sort(yPairs.begin(), yPairs.end(), [](const ShapePair& left, const ShapePair& right)
			{
				return left.id < right.id;
			});

		//double pointer check
		auto xPair = xPairs.begin();
		auto yPair = yPairs.begin();
		while (xPair != xPairs.end() && yPair != yPairs.end())
		{
			if (xPair->id == yPair->id && (xPair->bodyA->userData.bitmask & xPair->bodyB->userData.bitmask))
			{
				result.emplace_back(xPair->bodyA, xPair->bodyB);
				xPair = std::next(xPair);
				yPair = std::next(yPair);
			}
			else if (xPair->id > yPair->id)
				yPair = std::next(yPair);
			else
				xPair = std::next(xPair);
		}


		return result;
	}

	std::vector<ShapePrimitive*> SweepAndPrune::query(const std::vector<ShapePrimitive*>& bodyList, const AABB& region)
	{
		std::vector<ShapePrimitive*> result;

		std::vector<std::pair<ShapePrimitive*, AABB>> bodyBoxPairList;
		bodyBoxPairList.reserve(bodyList.size());

		for (auto&& elem : bodyList)
		{
			AABB aabb = AABB::fromShape(*elem);
			bodyBoxPairList.emplace_back(elem, aabb);
		}

		//brute search
		for (auto&& elem : bodyBoxPairList)
			if (region.collide(elem.second))
				result.emplace_back(elem.first);



		return result;
	}
}
