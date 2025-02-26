// template.cpp: 定义应用程序的入口点。
//

#include "template.h"
#include <vector>
#include <random>
#include <print>
template<class T>
void swap(T* a, T* b)
{
	T temp = *a;
	*a = *b;
	*b = temp;
}
template<class T>
void bubble_sort(T* a, int len)
{
	for (int i = 0; i < len - 1; ++i)
		for (int j = 0; j < len - 1 - i; ++j) {
			if (a[j] > a[j + 1])
				swap(&a[j], &a[j + 1]);
		}
}
std::vector<int> generate_random_int(size_t length, int min, int max)
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution distribution(min, max);
	std::vector<int> random_array(length);
	while (length--) {
		std::println("{}", length);
		random_array[length] = distribution(generator);
	}
	return random_array;
}
void test_sort(int len)
{
	auto arr = generate_random_int(len, 1, 100);
	int* array = new int[len];
	for (int i = 0; i < len; ++i)
	{
		*(array + i) = arr[i];
	}
	std::println("befor sort is:{}", arr);
	bubble_sort(array, arr.size());
	for (int i = 0; i < len; ++i)
	{
		std::print("[{}] is {}, ", i, array[i]);
	}
	delete[]array;
}
int main()
{
	constexpr int len = 10;
	test_sort(len);
	return 0;
}
