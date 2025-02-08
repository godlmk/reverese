#include <iostream>

#pragma pack(4)
struct st {
	int i;
	__int64 l;
	int ch;
	char c;
};
#pragma pack()
void aligin() {
	std::cout << offsetof(st, i) << std::endl;
	std::cout << offsetof(st, l) << std::endl;
	std::cout << offsetof(st, ch) << std::endl;
	std::cout << offsetof(st, c) << std::endl;
	std::cout << "size is: " << sizeof(st) << '\n';
}
struct s1 {
	char c;
	double d;
};
struct s2 {
	char c1;
	s1 s;
	char c2;
	char c3;
};
void aligin2() {
	std::cout << offsetof(s2, c1) << std::endl;
	std::cout << offsetof(s2, s) << std::endl;
	std::cout << offsetof(s2, c2) << std::endl;
	std::cout << offsetof(s2, c3) << std::endl;
	std::cout << "size is: " << sizeof(s2) << '\n';
}
int main() {
	//aligin();
	aligin2();
	return 0;
}