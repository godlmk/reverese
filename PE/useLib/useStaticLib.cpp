// useStaticLib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
//#include "Calc.h"
#include <print>
// 通过#pragma comment(lib, "Calc.lib")引入静态库
//#pragma comment(lib, "StaticLib.lib")

// 使用静态绑定
// 通过#pragma comment(lib, "MyDll.lib")引入动态库
#pragma comment(lib, "MyDll.lib")
 //申明在动态库中的函数
extern "C" __declspec(dllimport) int Add(int a, int b);
extern "C" __declspec(dllimport) int Sub(int a, int b);
extern "C" __declspec(dllimport) int Mul(int a, int b);
extern "C" __declspec(dllimport) int Div(int a, int b);
void useStaticBound()
{
	int sum = Add(1, 2);
	std::println("sum is {}", sum);
}

// 使用动态绑定
using lpAdd = int(*)(int, int);
using lpSub = int(*)(int, int);
using lpMul = int(*)(int, int);
using lpDiv = int(*)(int, int);

void useDynamicBound()
{
	lpAdd myadd;
	HMODULE hMoudle = LoadLibrary(L"C:/reverse/PE/useLib/dynamicLib/MyDll.dll");
	if (hMoudle == NULL)
	{
		std::println("LoadLibrary failed");
		return;
	}
	// 保持消息循环以处理消息
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	std::println("LoadLibrary success, id is {:d}", (int)hMoudle);
	myadd = (lpAdd)GetProcAddress(hMoudle, "Add");
	int sum = myadd(3, 4);
	std::println("{}", sum);
	FreeLibrary(hMoudle);
}

int main()
{
	useStaticBound();
	useDynamicBound();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
