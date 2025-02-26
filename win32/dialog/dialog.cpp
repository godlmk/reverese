// dialog.cpp : 定义应用程序的入口点。
//
#include "dialog.h"
#include <Windows.h>

#include <stdio.h>
BOOL  CALLBACK DialogProc(
	HWND    hWnd,
	UINT    Msg,
	WPARAM  wParam,
	LPARAM  lParam
);
// 全局变量:
HINSTANCE hInst;                                // 当前实例
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
	hInst = hInstance;
	PCWSTR className = L"My First Window";
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	return 0;
}

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HICON hBigIcon, hSmallIcon;
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		//MessageBox(NULL, TEXT("WM_INITDIALOG"), TEXT("INIT"), MB_OK);
		// 获取图标
		hBigIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_BIG));
		hSmallIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_SMALL));
		// 设置图标
		SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (DWORD)hBigIcon);
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (DWORD)hSmallIcon);
		return TRUE;
	}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case   IDC_BUTTON_OK:
		{
			// 获得文本框句柄
			// 通过句柄得到内容
			HWND hEditUser = GetDlgItem(hwndDlg, IDC_EDIT_USERNAME);
			HWND hEditPassword = GetDlgItem(hwndDlg, IDC_EDIT_PASSWORD);
			TCHAR szUserBuffer[0x50];
			GetWindowText(hEditUser, szUserBuffer, 0x50);
			DbgPrintfW(L"user is %s\n", szUserBuffer);
			GetWindowText(hEditUser, szUserBuffer, 0x50);
			DbgPrintfW(L"password is %s\n", szUserBuffer);

			MessageBox(NULL, TEXT("IDC_BUTTON_OK"), TEXT("OK"), MB_OK);

			return TRUE;
		}
		case   IDC_BUTTON_ERROR:
		{
			MessageBox(NULL, TEXT("IDC_BUTTON_OUT"), TEXT("OUT"), MB_OK);

			EndDialog(hwndDlg, 0);

			return TRUE;
		}
		}
		break;
	}
	}
	return false;
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


