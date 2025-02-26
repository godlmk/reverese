// myvector.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once


#define SUCCESS           			 1 // 成功						
#define ERROR            			 -1 // 失败						
#define MALLOC_ERROR			 -2 // 申请内存失败						
#define INDEX_ERROR		 	 -3 // 错误的索引号						
#include <algorithm>
#include <print>


template <class T>
class myvector
{
public:
	myvector();
	myvector(int dwSize);
	~myvector();
public:
	T	at(int dwIndex);					//根据给定的索引得到元素		
	void    push_back(T Element);						//将元素存储到容器最后一个位置		
	void	pop_back();					//删除最后一个元素		
	int	insert(int dwIndex, T Element);					//向指定位置新增一个元素		
	int	capacity();					//返回在不增容的情况下，还能存储多少元素		
	void	clear();					//清空所有元素		
	bool	empty();					//判断Vector是否为空 返回true时为空		
	int	erase(int dwIndex);					//删除指定元素		
	int	size();					//返回Vector元素数量的大小		
private:
	bool	expand();
private:
	int  m_iSize;						//当前容器的长度		
	int  m_iCapacity;						//默认初始化大小		
	T* m_pVector;						//容器指针		
};

// TODO: 在此处引用程序需要的其他标头。

template<class T>
inline myvector<T>::myvector() :m_iCapacity{ 0 }, m_iSize{ 0 }
{
	m_pVector = nullptr;
}

template<class T>
inline myvector<T>::myvector(int size) : m_iSize{ size }, m_iCapacity{ size }
{
	m_pVector = new T[m_iCapacity];
}

template<class T>
inline myvector<T>::~myvector()
{
	delete[]m_pVector;
	m_iSize = 0;
	m_iCapacity = 0;
}

template<class T>
inline T myvector<T>::at(int index)
{
	if (index < 0 || index >= m_iSize) {
		return INDEX_ERROR;
	}
	return m_pVector[index];
}

template<class T>
inline void myvector<T>::push_back(T element)
{
	if (m_iSize == m_iCapacity)
	{
		expand();
	}
	m_pVector[m_iSize++] = element;
}

template<class T>
inline void myvector<T>::pop_back()
{
	if (m_iSize > 0) {
		--m_iSize;
		m_pVector[m_iSize].~T();
	}
}

template<class T>
inline int myvector<T>::insert(int index, T element)
{
	if (index < 0 || index > m_iSize) return INDEX_ERROR;
	if (m_iSize == m_iCapacity) {
		if (!expand()) {
			return MALLOC_ERROR;
		}
	}
	for (int i = m_iSize; i > index; --i) {
		m_pVector[i] = m_pVector[i - 1];
	}
	m_pVector[index] = element;
	++m_iSize;
	return SUCCESS;
}

template<class T>
inline int myvector<T>::capacity()
{
	return m_iCapacity;
}

template<class T>
inline void myvector<T>::clear()
{
	for (int i = 0; i < m_iSize; ++i)
	{
		m_pVector[i].~T();
	}
	m_iSize = 0;
}

template<class T>
inline bool myvector<T>::empty()
{
	return m_iSize == 0;
}

template<class T>
inline int myvector<T>::erase(int index)
{
	if (index < 0 || index >= m_iSize) return INDEX_ERROR;

	// 析构
	m_pVector[index].~T();
	// 覆盖
	for (int i = index; i < m_iSize - 1; ++i) {
		m_pVector[i] = m_pVector[i + 1];
	}
	--m_iSize;
	return SUCCESS;
}

template<class T>
inline int myvector<T>::size()
{
	return m_iSize;
}

template<class T>
inline bool myvector<T>::expand()
{
	int half = m_iCapacity / 2;
	if (half == 0) half = 1;
	T* new_p = new T[m_iCapacity + half];
	std::copy(m_pVector, m_pVector + m_iCapacity, new_p);
	m_iCapacity += half;
	delete m_pVector;
	m_pVector = new_p;
	return SUCCESS;
}
