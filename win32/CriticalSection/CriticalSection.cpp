// CriticalSection.cpp: 定义应用程序的入口点。
//

#include "CriticalSection.h"

#include <windows.h>		
#include <print>
CRITICAL_SECTION g_csX;
CRITICAL_SECTION g_csY;
int x{ 0 }, y{ 0 };
using namespace std;

DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{
	for (int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&g_csX);
		EnterCriticalSection(&g_csY);
		++x;
		++y;
		Sleep(1000);


		LeaveCriticalSection(&g_csY);
		LeaveCriticalSection(&g_csX);

	}
	return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	for (int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&g_csY);
		EnterCriticalSection(&g_csX);

		Sleep(1000);


		LeaveCriticalSection(&g_csX);
		LeaveCriticalSection(&g_csY);

	}

	return 0;
}



int main(int argc, char* argv[])
{
	InitializeCriticalSection(&g_csX);
	InitializeCriticalSection(&g_csY);

	printf("主线程:%x %x %x\n", g_csX.LockCount, g_csX.RecursionCount, g_csX.OwningThread);

	//创建一个新的线程	
	HANDLE hThread1 = ::CreateThread(NULL, 0, ThreadProc1,
		NULL, 0, NULL);

	//创建一个新的线程
	HANDLE hThread2 = ::CreateThread(NULL, 0, ThreadProc2,
		NULL, 0, NULL);

	////创建一个新的线程
	//HANDLE hThread3 = ::CreateThread(NULL, 0, ThreadProc3,
	//	NULL, 0, NULL);

	////创建一个新的线程
	//HANDLE hThread4 = ::CreateThread(NULL, 0, ThreadProc4,
	//	NULL, 0, NULL);
	//如果不在其他的地方引用它 关闭句柄
	::CloseHandle(hThread1);
	::CloseHandle(hThread2);

	Sleep(1000 * 60 * 60);
	DeleteCriticalSection(&g_csX);
	DeleteCriticalSection(&g_csY);

	return 0;
}

