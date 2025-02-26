// firstwindows.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "firstwindows.h"
#include <stdio.h>
LRESULT CALLBACK WindowProc(
	HWND    hWnd,
	UINT    Msg,
	WPARAM  wParam,
	LPARAM  lParam
);
void __cdecl OutputDebugStringF(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);

#ifdef _DEBUG  
#define DbgPrintfA   OutputDebugStringF  
#define DbgPrintfW   OutputDebugStringFW
#else  
#define DbgPrintfA
#define DbgPrintfW
#endif 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	PCWSTR className = L"My First Window";
	// 创建一个窗口类
	WNDCLASS wndclass = { 0 };
	wndclass.hbrBackground = (HBRUSH)COLOR_MENU;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.lpszClassName = className;
	wndclass.hInstance = hInstance;
	// 注册窗口
	RegisterClass(&wndclass);
	// 创建窗口
	HWND hwnd = CreateWindow(
		className,
		TEXT("我的第一个窗口"),
		WS_OVERLAPPEDWINDOW,
		10,
		10,
		600,
		300,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
		return 0;
	// 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	// 消息循环
	MSG msg;
	while (GetMessage(&msg, hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {

	case WM_CREATE:
	{
		DbgPrintfA("WM_CRATE %d %d\n", wParam, lParam);
		CREATESTRUCT* createst = (CREATESTRUCT*)lParam;
		DbgPrintfW(L"CREATESTRUCT %s\n", createst->lpszClass);
		return 0;
	}
	case WM_MOVE:
	{
		DbgPrintfA("WM_MOVE %d %d\n", wParam, lParam);
		POINTS points = MAKEPOINTS(lParam);
		DbgPrintfA("X Y %d %d \n", points.x, points.y);
		return 0;
	}
	case WM_SIZE:
	{
		DbgPrintfA("WM_SIZE %d %d\n", wParam, lParam);
		int newWidth = (int)(short)LOWORD(lParam);
		int newHeight = (int)(short)HIWORD(lParam);
		DbgPrintfA("width is %d  height is %d \n", newWidth, newHeight);
		return 0;
	}
	case WM_DESTROY:
	{
		DbgPrintfA("WM_DESTROY %d %d\n", wParam, lParam);
		PostQuitMessage(0);
		return 0;
	}
	// 键盘松开
	case WM_KEYUP:
	{
		DbgPrintfA("WM_KEYUP %d %d\n", wParam, lParam);
		return 0;
	}
	// 键盘按下
	case WM_KEYDOWN:
	{
		DbgPrintfA("WM_DOWN %d %d\n", wParam, lParam);
		return 0;
	}
	// 鼠标按下
	case WM_LBUTTONDOWN:
	{
		DbgPrintfA("WM_LBUTTONDOWN %d %d\n", wParam, lParam);
		return 0;
	}

	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}
void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

	if (strBuffer == NULL) {
		return; // Handle allocation failure
	}

	va_start(vlArgs, format);
	_vsnprintf_s(strBuffer, 4096, _TRUNCATE, format, vlArgs);
	va_end(vlArgs);
	strcat_s(strBuffer, 4096, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}
void __cdecl OutputDebugStringFW(const wchar_t* format, ...)
{
	va_list vlArgs;
	wchar_t* strBuffer = (wchar_t*)GlobalAlloc(GPTR, 4096 * sizeof(wchar_t));
	if (strBuffer == NULL) { return; }

	va_start(vlArgs, format);
	_vsnwprintf_s(strBuffer, 4096, _TRUNCATE, format, vlArgs);
	va_end(vlArgs);
	wcscat_s(strBuffer, 4096, L"\n");
	OutputDebugStringW(strBuffer);
	return;
}
