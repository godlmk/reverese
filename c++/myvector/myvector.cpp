// myvector.cpp: 定义应用程序的入口点。
//

#include "myvector.h"
#include <vector>

using namespace std;
struct Person {
	Person() :a{ 0 }, b{ 0 } {}
	Person(int x) { a = x, b = x; }
	int a, b;
};
void test_myvector()
{
	println("in myvector");
	myvector<Person> v;
	println("default consruct size is {}, capacity is {}", v.size(), v.capacity());
	for (int i = 0; i < 100; ++i) {
		Person p(i);
		v.push_back(p);
		std::println("index is {}, value is {}", i, v.at(i).a);
		std::println("size is {}, capacity is {}", v.size(), v.capacity());
	}
}
void test_capacity()
{
	println("in std::vector");
	vector<int> vec;
	println("default consruct size is {}, capacity is {}", vec.size(), vec.capacity());
	for (int i = 0; i < 100; ++i) {
		vec.emplace_back(i);
		println("size is {}, capacity is {}", vec.size(), vec.capacity());
	}
}

int main()
{
	test_myvector();
	test_capacity();
	return 0;
}
