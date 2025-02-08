// array.cpp: 定义应用程序的入口点。
//
#include<stdio.h>

void print() {
	printf("Hello World!\n");
}
void func() {
	int arr[5] = { 1, 2, 3, 4, 5 };
	//arr[6] = (int)print;
}
void arr2d() {
	int arr[3][4] = {
		{1,2},
		{3},
		{4}
	};
}
int main()
{
	//func();
	arr2d();
	return 0;
}
