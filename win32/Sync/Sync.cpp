// Sync.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Sync.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明:
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI Thread1(LPVOID lpParameter);
DWORD WINAPI Thread2(LPVOID lpParameter);
DWORD WINAPI Thread3(LPVOID lpParameter);
DWORD WINAPI ThreadInit(LPVOID lpParameter);

HANDLE hSemaphore;
HANDLE hThread[4];

HWND hEditReset;
HWND hEdit1;
HWND hEdit2;
HWND hEdit3;
HWND hEdit4;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;
	// TODO: 在此处放置代码。
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProcMain);
	// 初始化全局字符串
	return 0;
}

BOOL CALLBACK DialogProcMain(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		hEditReset = GetDlgItem(hDlg, IDC_EDIT_SET);
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);
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
		case IDC_BUTTON:
		{
			SetWindowText(hEdit1, TEXT("0"));
			SetWindowText(hEdit2, TEXT("0"));
			SetWindowText(hEdit3, TEXT("0"));
			CreateThread(NULL, 0, ThreadInit, NULL, 0, NULL);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

DWORD __stdcall Thread1(LPVOID lpParameter)
{
	TCHAR szBuffer[0x10];
	int iTimer = 0;
	WaitForSingleObject(hSemaphore, INFINITE);
	while (iTimer < 100)
	{
		Sleep(100);
		ZeroMemory(szBuffer, 0x10);
		GetWindowText(hEdit1, szBuffer, 0x10);
		iTimer = _tstoi(szBuffer);
		++iTimer;
		_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), iTimer);
		SetWindowText(hEdit1, szBuffer);

	}
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return 0;
}

DWORD __stdcall Thread2(LPVOID lpParameter)
{
	TCHAR szBuffer[0x10];
	int iTimer = 0;
	WaitForSingleObject(hSemaphore, INFINITE);
	while (iTimer < 100)
	{
		Sleep(100);
		ZeroMemory(szBuffer, 0x10);
		GetWindowText(hEdit2, szBuffer, 0x10);
		iTimer = _tstoi(szBuffer);
		++iTimer;
		_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), iTimer);
		SetWindowText(hEdit2, szBuffer);

	}
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return 0;
}

DWORD __stdcall Thread3(LPVOID lpParameter)
{
	TCHAR szBuffer[0x10];
	int iTimer = 0;
	WaitForSingleObject(hSemaphore, INFINITE);
	while (iTimer < 100)
	{
		Sleep(100);
		ZeroMemory(szBuffer, 0x10);
		GetWindowText(hEdit3, szBuffer, 0x10);
		iTimer = _tstoi(szBuffer);
		++iTimer;
		_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), iTimer);
		SetWindowText(hEdit3, szBuffer);

	}
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return 0;
}

DWORD __stdcall ThreadInit(LPVOID lpParameter)
{
	TCHAR szBuffer[0x10];
	int iMoney = 0;
	hSemaphore = CreateSemaphore(NULL, 0, 3, NULL);
	hThread[0] = CreateThread(NULL, 0, Thread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, Thread2, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, Thread3, NULL, 0, NULL);
	while (iMoney < 1000)
	{
		ZeroMemory(szBuffer, 0x10);
		GetWindowText(hEditReset, szBuffer, 0x10);
		iMoney = _tstoi(szBuffer);
		++iMoney;
		_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), iMoney);
		SetWindowText(hEditReset, szBuffer);
	}
	ReleaseSemaphore(hSemaphore, 3, NULL);

	::WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
	::CloseHandle(hSemaphore);
	return 0;
}
