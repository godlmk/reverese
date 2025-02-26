// linkedlist.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <algorithm>
#include <print>

#define SUCCESS           1 // 执行成功				
#define ERROR            -1 // 执行失败				
#define INDEX_IS_ERROR   -2 // 错误的索引号				
#define BUFFER_IS_EMPTY  -3 // 缓冲区已空				

template<class T>
class LinkedList
{
public:
	LinkedList();
	~LinkedList();
	bool empty();
	void clear();
	int GetElement(int index, T& element);
	int GetElement(const T& element);
	int insert(const int index, const T& element);
	int insert(const T& element);
	int del(const int index);
	int size();
	typedef struct Node {
		T data;
		Node* next;
		Node(T val) :data{ val }, next{ nullptr } {}
		Node(T val, T* next) :data{ val }, this.next{ next } {}
	}Node;
	using PNode = Node*;
private:
	Node* getIndexCurrentNode(int index);
	Node* getIndexPreviousNode(int index);
	Node* getIndexNettNode(int index);
private:
	PNode m_head;
	int m_size;
};

