// EatAlpha.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "EatAlpha.h"
#include <tchar.h>

#define LENTH 0x40
TCHAR szResBuffer[LENTH];

// 全局变量:
HINSTANCE hInst;                                // 当前实例
BOOL CALLBACK DialogProcMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 初始化线程
DWORD WINAPI ThreadInit(LPVOID lpParameter);

DWORD WINAPI ThreadCache(LPVOID lpParameter);

DWORD WINAPI ThreadEat(LPVOID lpParameter);

HWND hEditInput;
HWND hEdit1;
HWND hEdit2;
HWND hEdit3;
HWND hEdit4;
HWND hEditCache1, hEditCache2;
HANDLE g_hSemaphore_read, g_hSemaphore_eat;
DWORD index = 0;
int originlen = 0;
CRITICAL_SECTION cs_index;
CRITICAL_SECTION cs_edit;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;

	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProcMain);
	// TODO: 在此处放置代码。

	return 0;
}

BOOL CALLBACK DialogProcMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hEditInput = GetDlgItem(hDlg, IDC_EDIT_INPUT);
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);
		hEdit4 = GetDlgItem(hDlg, IDC_EDIT4);
		hEditCache1 = GetDlgItem(hDlg, IDC_EDIT_CACHE1);
		hEditCache2 = GetDlgItem(hDlg, IDC_EDIT_CACHE2);
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
			SetWindowText(hEditCache1, TEXT("0"));
			SetWindowText(hEditCache2, TEXT("0"));
			::CreateThread(NULL, 0,
				ThreadInit, NULL, 0, NULL);
			return TRUE;
		}
		}
	}
	}
	return 0;
}

DWORD __stdcall ThreadInit(LPVOID lpParameter)
{
	GetWindowText(hEditInput, szResBuffer, LENTH);
	g_hSemaphore_eat = CreateSemaphore(NULL, 0, 4, NULL);
	g_hSemaphore_read = CreateSemaphore(NULL, 2, 2, NULL);
	InitializeCriticalSection(&cs_index);
	InitializeCriticalSection(&cs_edit);
	TCHAR szBuffer[LENTH];
	GetWindowText(hEditInput, szBuffer, LENTH);
	originlen = _tcsnlen(szBuffer, LENTH);
	HANDLE hHandle[6];
	hHandle[0] = CreateThread(NULL, 0, ThreadCache,
		(void*)&hEditCache1, 0, NULL);
	hHandle[1] = CreateThread(NULL, 0, ThreadCache,
		(void*)&hEditCache2, 0, NULL);
	hHandle[2] = CreateThread(NULL, 0, ThreadEat,
		(void*)&hEdit1, 0, NULL);
	hHandle[3] = CreateThread(NULL, 0, ThreadEat,
		(void*)&hEdit2, 0, NULL);
	hHandle[4] = CreateThread(NULL, 0, ThreadEat,
		(void*)&hEdit3, 0, NULL);
	hHandle[5] = CreateThread(NULL, 0, ThreadEat,
		(void*)&hEdit4, 0, NULL);
	WaitForMultipleObjects(6, hHandle, TRUE, INFINITE);
	::CloseHandle(g_hSemaphore_eat);
	::CloseHandle(g_hSemaphore_read);
	::DeleteCriticalSection(&cs_edit);
	::DeleteCriticalSection(&cs_index);
	return 0;
}


DWORD __stdcall ThreadCache(LPVOID lpParameter)
{
	TCHAR res[LENTH] = { 0 };
	for (;;)
	{
		WaitForSingleObject(g_hSemaphore_read, INFINITE);
		EnterCriticalSection(&cs_index);
		if (index >= originlen)
		{
			LeaveCriticalSection(&cs_index);
			EnterCriticalSection(&cs_edit);
			SetWindowText(*(HWND*)lpParameter, TEXT("-"));
			LeaveCriticalSection(&cs_edit);
			ReleaseSemaphore(g_hSemaphore_eat, 4, NULL);
			return 0;
		}
		TCHAR ch[2] = { szResBuffer[index], '\0' };
		EnterCriticalSection(&cs_edit);
		GetWindowText(*(HWND*)lpParameter, res, LENTH);
		if (_tcscmp(res, TEXT("0")) != 0)
		{
			LeaveCriticalSection(&cs_edit);
			LeaveCriticalSection(&cs_index);
			continue;
		}
		SetWindowText(*(HWND*)lpParameter, ch);
		LeaveCriticalSection(&cs_edit);
		Sleep(1000);
		++index;
		LeaveCriticalSection(&cs_index);
		ReleaseSemaphore(g_hSemaphore_eat, 1, NULL);
	}
	return 0;
}

DWORD __stdcall ThreadEat(LPVOID lpParameter)
{
	TCHAR szBuffer1[LENTH] = { 0 };
	TCHAR szBuffer2[LENTH] = { 0 };
	TCHAR szBuffer[LENTH];
	while (1)
	{
		WaitForSingleObject(g_hSemaphore_eat, INFINITE);
		EnterCriticalSection(&cs_edit);
		GetWindowText(hEditCache1, szBuffer1, LENTH);
		GetWindowText(hEditCache2, szBuffer2, LENTH);
		LeaveCriticalSection(&cs_edit);
		GetWindowText(*(HWND*)lpParameter, szBuffer, LENTH);
		_tcscat_s(szBuffer, LENTH, TEXT("-"));
		int len1 = _tcsnlen(szBuffer1, LENTH);
		int len2 = _tcsnlen(szBuffer2, LENTH);
		if (_tcscmp(szBuffer1, TEXT("0")) == 0 && _tcscmp(szBuffer2, TEXT("0")) == 0)
		{
			ReleaseSemaphore(g_hSemaphore_eat, 1, NULL);
			break;
		}

		if (_tcscmp(szBuffer1, TEXT("0")) == 0) {
			SetWindowText(hEditCache2, TEXT("0"));
			_tcscat_s(szBuffer, LENTH, szBuffer2);
		}
		else {
			SetWindowText(hEditCache1, TEXT("0"));
			_tcscat_s(szBuffer, LENTH, szBuffer1);
		}

		SetWindowText(*(HWND*)lpParameter, szBuffer);
		Sleep(1000);
		ReleaseSemaphore(g_hSemaphore_read, 1, NULL);
	}
	return *(int*)lpParameter;
}
