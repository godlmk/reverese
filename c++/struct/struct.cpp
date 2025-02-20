// struct.cpp: 定义应用程序的入口点。
//

#include "struct.h"
#include <print>
#include <stdio.h>

using namespace std;
struct Calc {
	int add(int a, int b) { return a + b; }
	int max() { return a > b ? a : b; }
	int a, b;
	void f1() { println("f1"); }
	void f2() { a = 10; printf("f2()%x\n"); }
};
int max(int a, int b) { return  a > b ? a : b; }
void show_this_call()
{
	Calc calc{ 1,2 };
	calc.add(3, 4);
	calc.max();
	max(1, 2);
}
void show_nullptr()
{
	Calc* calc = nullptr;
	calc->f1();
	calc->f2();
}
int main()
{
	show_this_call();
	return 0;
}
