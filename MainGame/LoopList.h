#pragma once

#include <list>


template <typename T>
class LoopList
{
private:
	std::list<T> m_List;
	typename std::list<T>::iterator m_Iterator;

public:
	T& First();
	T& Now();
	T& Prev();
	T& Next();

	void Clear();
	void Add(T t) { m_List.push_back(t); };
	void Remove(T t) { m_List.remove(t); };
	std::list<T>* GetList() { return &m_List; };
};

template<typename T>
inline T& LoopList<T>::First()
{
	m_Iterator = m_List.begin();
	return (*m_Iterator);
}

template<typename T>
inline T& LoopList<T>::Now()
{
	return (*m_Iterator);
}

template<typename T>
inline T& LoopList<T>::Prev()
{
	if (m_Iterator == m_List.begin())
	{
		m_Iterator = m_List.end();
	}

	m_Iterator--;

	return (*m_Iterator);
}


template<typename T>
inline T& LoopList<T>::Next()
{
	m_Iterator++;

	if (m_Iterator == m_List.end())
	{
		m_Iterator = m_List.begin();
	}

	return (*m_Iterator);
}

template<typename T>
inline void LoopList<T>::Clear()
{
	m_Iterator = m_List.end();
	m_List.clear();
}

