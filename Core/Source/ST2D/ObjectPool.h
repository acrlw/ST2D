#pragma once


#include "ST2D/Log.h"

namespace ST
{
	template <typename T>
	class ST_API ObjectPool
	{
	public:

		T* acquire()
		{
			if (m_freeIndices.empty())
			{
				m_objects.push_back(std::make_unique<T>());
				return m_objects.back().get();
			}
			else
			{
				int index = m_freeIndices.back();
				m_freeIndices.pop_back();
				return m_objects[index].get();
			}
		}

		void release(T* object)
		{
			for (int i = 0; i < m_objects.size(); i++)
			{
				if (m_objects[i] == object)
				{
					m_freeIndices.push_back(i);
					return;
				}
			}
		}

		T* operator[](int index)
		{
			return m_objects[index].get();
		}

		int size() const
		{
			return m_objects.size();
		}

		

	private:
		std::vector<std::unique_ptr<T>> m_objects;
		std::vector<int> m_freeIndices;
	};
}