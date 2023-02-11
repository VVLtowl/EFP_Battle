#pragma once
#include <list>
using namespace std;

template <typename T>
class LoopList
{
public:
	list<T> m_List;
	typename list<T>::iterator m_Iterator;

public:
	T& First();
	T& Next();
};

template<typename T>
inline T& LoopList<T>::First()
{
	m_Iterator = m_List.begin();
	return (*m_Iterator);
}


template<typename T>
inline T& LoopList<T>::Next()
{
	m_Iterator++;
	
	if(m_Iterator == m_List.end())
	{
		m_Iterator = m_List.begin();
	}

	return (*m_Iterator);
}
