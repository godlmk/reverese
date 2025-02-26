// linkedlist.cpp: 定义应用程序的入口点。
//

#include "linkedlist.h"

using namespace std;
template<class T>
LinkedList<T>::LinkedList() : m_head{ new Node(-1) }, m_size{ 0 }
{
}

template<class T>
LinkedList<T>::~LinkedList()
{
	clear();
	delete m_head;
}

template<class T>
bool LinkedList<T>::empty()
{
	return m_head->next == nullptr;
}

template<class T>
void LinkedList<T>::clear()
{
	auto cur = m_head->next;
	while (cur != nullptr) {
		auto ne = cur->next;
		delete cur;
		cur = ne;
	}
	m_size = 0;
	m_head->next = nullptr;
}

template<class T>
int LinkedList<T>::GetElement(int index, T& element)
{
	if (index < 1 || index > m_size) return INDEX_IS_ERROR;
	auto cur = m_head->next;
	element = getIndexCurrentNode(index)->data;
	return SUCCESS;
}

template<class T>
int LinkedList<T>::GetElement(const T& element)
{
	auto cur = m_head->next;
	int ans = 1;
	while (cur) {
		if (cur->data == element) {
			return ans;
		}
		++ans;
		cur = cur->next;
	}
	return ERROR;
}

template<class T>
int LinkedList<T>::insert(const int index, const T& element)
{
	if (index < 1 || index > m_size) return INDEX_IS_ERROR;
	auto pre = getIndexPreviousNode(index);
	auto temp = new Node(element, pre->next);
	pre->next = temp;
	++m_size;
	return SUCCESS;
}

template<class T>
int LinkedList<T>::insert(const T& element)
{
	auto temp = new Node(element, m_head->next);
	m_head->next = temp;
	++m_size;
	return SUCCESS;
}

template<class T>
int LinkedList<T>::del(const int index)
{
	if (index < 1 || index > m_size)return INDEX_IS_ERROR;
	auto pre = getIndexPreviousNode(index);
	auto cur = pre->next;
	pre->next = cur->next;
	delete cur;
	--m_size;
	return SUCCESS;
}

template<class T>
int LinkedList<T>::size()
{
	return m_size;
}

template<class T>
LinkedList<T>::PNode LinkedList<T>::getIndexCurrentNode(int index)
{
	auto cur = m_head->next;
	if (index == 1) return m_head;
	for (int i = 1; i < index; ++i) {
		cur = cur->next;
	}
	return cur;
}

template<class T>
LinkedList<T>::PNode LinkedList<T>::getIndexPreviousNode(int index)
{
	auto cur = m_head->next;
	for (int i = 1; i < index - 1; ++i) {
		cur = cur->next;
	}
	return cur;
}

template<class T>
LinkedList<T>::PNode LinkedList<T>::getIndexNettNode(int index)
{
	auto cur = m_head->next;
	for (int i = 1; i <= index; ++i) {
		cur = cur->next;
	}
	return cur;
}

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}
