#include "IdPool.h"

namespace ST
{
	int IdPool::getNewId()
	{
		if (m_freeIds.empty())
			return m_nextId++;

		int result = m_freeIds.back();
		m_freeIds.pop_back();
		return result;
	}
	void IdPool::freeId(int id)
	{
		m_freeIds.push_back(id);
	}

	void IdPool::reset()
	{
		m_nextId = 0;
		m_freeIds.clear();
	}
}
