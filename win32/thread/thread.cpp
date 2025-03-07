// thread.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "thread.h"
#include <stdio.h>


// 全局变量:
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明:
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadAddProc(LPVOID lpParameter);
DWORD WINAPI ThreadDecProc(LPVOID lpParameter);
void __cdecl OutputDebugStringF(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);

#ifdef _DEBUG  
#define DbgPrintfA   OutputDebugStringF  
#define DbgPrintfW   OutputDebugStringFW
#else  
#define DbgPrintfA
#define DbgPrintfW
#endif 

HWND hEdit1;
HWND hEdit2;
HANDLE h1, h2;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInst = hInstance;

	// TODO: 在此处放置代码。
	int ans = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProcMain);
	DWORD err = GetLastError();
	//DbgPrintfW(L"%s", GetLastError());
	// 初始化全局字符串
	DbgPrintfW(L"DialogBox failed with error %d", err);
	if (err == ERROR_INVALID_WINDOW_HANDLE) {
		DbgPrintfW(L"Invalid window handle.");
	}

	return 0;
}

BOOL CALLBACK    DialogProcMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		// 为两个edit赋初值
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SetWindowText(hEdit1, TEXT("1000"));
		SetWindowText(hEdit2, TEXT("0"));
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_START:
		{
			h1 = CreateThread(NULL, 0,
				ThreadDecProc, NULL, 0, NULL);
			h2 = CreateThread(NULL, 0,
				ThreadAddProc, NULL, 0, NULL);
			return TRUE;
		}
		case IDC_BUTTON_SUSPEND:
		{
			::SuspendThread(h1);
			::SuspendThread(h2);
			return TRUE;
		}
		case IDC_BUTTON_RESUME:
		{
			::ResumeThread(h1);
			::ResumeThread(h2);
			return TRUE;
		}
		case IDC_BUTTON_KILL:
		{
			::TerminateThread(h1, 3);
			::WaitForSingleObject(h1, INFINITE);
			::TerminateThread(h2, 3);
			::WaitForSingleObject(h2, INFINITE);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

DWORD __stdcall ThreadDecProc(LPVOID lpParameter)
{
	int value{ 0 };
	do {
		TCHAR szBuffer[0x20] = { 0 };
		// 获取值
		GetWindowText(hEdit1, szBuffer, 0x20);
		value = _tstoi(szBuffer);
		--value;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), value);
		SetWindowText(hEdit1, szBuffer);
		Sleep(10);
	} while (value > 0);
	return 0;
}

DWORD __stdcall ThreadAddProc(LPVOID lpParameter)
{
	int value{ 0 };
	do {
		TCHAR szBuffer[0x20] = { 0 };
		GetWindowText(hEdit2, szBuffer, 0x20);
		_stscanf_s(szBuffer, _T("%d"), &value);
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), ++value);
		SetWindowText(hEdit2, szBuffer);
		Sleep(10);
	} while (value < 1000);
	return 0;
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
