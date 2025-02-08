// union.cpp: 定义应用程序的入口点。
//

#include "union.h"

#include <print>

using namespace std;
union tu {
	char c;
	int i;
};

int main()
{
	tu t;
	t.i = 0x01234568;
	std::print("{:X}", t.c);
	return 0;
}
