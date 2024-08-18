#pragma once

#include "ST2D/Log.h"

namespace ST
{
	class ST_API IdPool
	{
	public:
		IdPool() = default;
		~IdPool() = default;

		int getNewId();
		void freeId(int id);
		void reset();
	private:
		int m_nextId = 0;
		std::vector<int> m_freeIds;

	};
}