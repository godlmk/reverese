#include "pch.h"
#include "MyDll.h"
int Add(int a, int b)
{
	return a + b;
}
int Sub(int a, int b)
{
	return a - b;
}
int Mul(int a, int b)
{
	return a * b;
}
int Div(int a, int b)
{
	return a / b;
}
void Init()
{
	OutputDebugString(L"Init function called\n");
	MessageBox(0, L"Init", L"Init", MB_OK | MB_SYSTEMMODAL);
	// 简单的消息循环
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
void Destroy()
{
	MessageBox(0, L"Destroy", L"Destroy", 0);
}
