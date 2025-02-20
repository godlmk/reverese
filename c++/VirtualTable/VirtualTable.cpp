// VirtualTable.cpp: 定义应用程序的入口点。
//

#include "VirtualTable.h"
#include <print>
using namespace std;

class Base {
	int a, b;
public:
	Base() :a{ 1 }, b{ 2 } {}
	virtual	void func1() { std::println("func1"); }
	virtual void func2() { std::println("func2"); }
};

struct Father {
	virtual	void func1() { std::println("Father:func1"); }
	virtual void func2() { std::println("Father:func2"); }
	virtual void func3() { std::println("Father:func3"); }
};
struct Son1 :Father {
	virtual	void func4() { std::println("Son1:func4"); }
	virtual void func5() { std::println("Son1:func5"); }
	virtual void func6() { std::println("Son1:func6"); }
};
struct Son2 :Father {
	virtual	void func1() { std::println("Son2:func1"); }
	virtual void func2() { std::println("Son2:func2"); }
	virtual void func6() { std::println("Son2:func6"); }
};

void use_virtual_table()
{
	Base base{};
	using pFunc = void(*)();
	int virtual_table_addr = *(int*)(&base);
	pFunc p = (pFunc)(*(int*)virtual_table_addr);
	p();
}
void use_virtalbe_function()
{
	Base base{};
	std::println("Base' size is {}", sizeof(Base));
	base.func1();
	base.func2();
	Base* pb = &base;
	pb->func1();
	pb->func2();
}
using pFunc = void(*)();
void print_virtual_table1()
{
	Son1 s1;
	// 这里是虚表的地址
	int virtual_table_addr = *(int*)(&s1);
	pFunc p;
	// 虚表里保存的是虚函数的地址
	int virtual_function_addr;
	int i = 0;
	while (*((int*)virtual_table_addr + i) != 0) {
		virtual_function_addr = *((int*)virtual_table_addr + i);
		p = (pFunc)virtual_function_addr;
		p();
		println("第{}个函数虚函数地址为：{:X}", i + 1, virtual_function_addr);
		++i;
	}
}
void print_virtual_table2()
{
	Son2 s2;
	// 这里是虚表的地址
	int virtual_table_addr = *(int*)(&s2);
	pFunc p;
	// 虚表里保存的是虚函数的地址
	int virtual_function_addr;
	int i = 0;
	while (*((int*)virtual_table_addr + i) != 0) {
		virtual_function_addr = *((int*)virtual_table_addr + i);
		p = (pFunc)virtual_function_addr;
		p();
		println("第{}个函数虚函数地址为：{:X}", i + 1, virtual_function_addr);
		++i;
	}
}

int main()
{
	//use_virtual_table();
	print_virtual_table1();
	print_virtual_table2();
	return 0;
}
