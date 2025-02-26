// simpleadd.cpp: 定义应用程序的入口点。
//
#include <iostream>
#include <print>

void __declspec(naked) foo() {
	__asm
	{
		ret
	}
}

int add2(int a, int b) {
	int c = a + b;
	std::println("sum is {}", c);
	return c;
}
int main()
{
	foo();
	int r = add2(2, 3);
	return 0;
}
