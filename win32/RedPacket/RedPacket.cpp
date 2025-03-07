// RedPacket.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "RedPacket.h"


// 全局变量:
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明:
// 此代码模块中包含的函数的前向声明:
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadGrabRed1(LPVOID lpParameter);
DWORD WINAPI ThreadGrabRed2(LPVOID lpParameter);
DWORD WINAPI ThreadGrabRed3(LPVOID lpParameter);
DWORD WINAPI ThreadInit(LPVOID lpParameter);

HWND hEditInput;
HWND hEdit1;
HWND hEdit2;
HWND hEdit3;
CRITICAL_SECTION cs;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProcMain);
	return 0;
}
BOOL CALLBACK    DialogProcMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hEditInput = GetDlgItem(hDlg, IDC_EDIT_INPUT);
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
			::CreateThread(NULL, 0,
				ThreadInit, NULL, 0, NULL);
			return TRUE;
		}
		case IDC_BUTTON_RESET:
		{
			SetWindowText(hEdit1, TEXT("0"));
			SetWindowText(hEdit2, TEXT("0"));
			SetWindowText(hEdit3, TEXT("0"));

		}
		}
	}
	}
	return FALSE;
}

DWORD __stdcall ThreadGrabRed1(LPVOID lpParameter)
{
	int value{ 0 };
	int myval{ 0 };
	TCHAR szBuffer[0x20] = { 0 };
	while (1) {
		//EnterCriticalSection(&cs);
		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("mutex"));
		WaitForSingleObject(hMutex, INFINITE);
		// 获取值
		GetWindowText(hEditInput, szBuffer, 0x20);
		value = _tstoi(szBuffer);
		if (value < 50)
		{
			//LeaveCriticalSection(&cs);
			return 0;
		}
		value -= 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), value);
		SetWindowText(hEditInput, szBuffer);
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		// 设置自己的值
		GetWindowText(hEdit1, szBuffer, 0x20);
		myval = _tstoi(szBuffer);
		myval += 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), myval);
		SetWindowText(hEdit1, szBuffer);
		Sleep(50);
	};
	return 0;
}

DWORD __stdcall ThreadGrabRed2(LPVOID lpParameter)
{
	int value{ 0 };
	int myval{ 0 };
	TCHAR szBuffer[0x20] = { 0 };
	while (1) {
		//// use CriticalSection
		//EnterCriticalSection(&cs);
		// use Mutex
		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("mutex"));
		WaitForSingleObject(hMutex, INFINITE);
		// 获取值
		GetWindowText(hEditInput, szBuffer, 0x20);
		value = _tstoi(szBuffer);
		if (value < 50)
		{
			LeaveCriticalSection(&cs);
			return 0;
		}
		value -= 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), value);
		SetWindowText(hEditInput, szBuffer);
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		// 设置自己的值
		GetWindowText(hEdit2, szBuffer, 0x20);
		myval = _tstoi(szBuffer);
		myval += 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), myval);
		SetWindowText(hEdit2, szBuffer);
		Sleep(50);
	};
	return 0;
}
DWORD __stdcall ThreadGrabRed3(LPVOID lpParameter)
{
	int value{ 0 };
	int myval{ 0 };
	TCHAR szBuffer[0x20] = { 0 };
	while (1) {
		//EnterCriticalSection(&cs);
		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("mutex"));
		WaitForSingleObject(hMutex, INFINITE);
		// 获取值
		GetWindowText(hEditInput, szBuffer, 0x20);
		value = _tstoi(szBuffer);
		if (value < 50)
		{
			LeaveCriticalSection(&cs);
			return 0;
		}
		value -= 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), value);
		SetWindowText(hEditInput, szBuffer);
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		// 设置自己的值
		GetWindowText(hEdit3, szBuffer, 0x20);
		myval = _tstoi(szBuffer);
		myval += 50;
		_stprintf_s(szBuffer, _countof(szBuffer), _T("%d"), myval);
		SetWindowText(hEdit3, szBuffer);
		Sleep(50);
	};
	return 0;
}
DWORD __stdcall ThreadInit(LPVOID lpParameter)
{
	//InitializeCriticalSection(&cs);
	HANDLE mutex = CreateMutex(NULL, FALSE, TEXT("mutex"));
	// 创建三个抢红包线程
	HANDLE hHandleArr[3];
	hHandleArr[0] = CreateThread(NULL, 0,
		ThreadGrabRed1, NULL, 0, NULL);
	hHandleArr[1] = CreateThread(NULL, 0,
		ThreadGrabRed2, NULL, 0, NULL);
	hHandleArr[2] = CreateThread(NULL, 0,
		ThreadGrabRed3, NULL, 0, NULL);
	// 等待三个线程都结束后close
	WaitForMultipleObjects(3, hHandleArr, TRUE, INFINITE);
	for (int i = 0; i < 3; ++i)
	{
		CloseHandle(hHandleArr[i]);
	}
	CloseHandle(mutex);
	return 0;
}

