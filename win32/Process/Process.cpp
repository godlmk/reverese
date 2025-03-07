// Process.cpp: 定义应用程序的入口点。
//

#include "Process.h"
#include <Windows.h>
#include <print>
#include <string>

using namespace std;

VOID TestCreateProcess()
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);

	TCHAR szCmdline[] = TEXT(" http://www.ifeng.com");

	BOOL res = CreateProcess(
		TEXT("c://program files//internet explorer//iexplore.exe"),
		szCmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
}
void TextInheritHandles()
{
	char szBuffer[256] = { 0 };
	char szHandle[8] = { 0 };
	//若要创建能继承的句柄，父进程必须指定一个SECURITY_ATTRIBUTES结构并对它进行初始化					
	//三个成员的意义：大小、默认安全属性、是否可以继承					
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	//创建一个可以被继承的内核对象					
	HANDLE g_hEvent = CreateEvent(&sa, TRUE, FALSE, NULL);

	//组织命令行参数					
	sprintf(szHandle, "%x", g_hEvent);
	sprintf(szBuffer, "C:/reverse/win32/Process/out/build/x86-debug/Processson.exe %s", szHandle);

	//定义创建进程需要用的结构体					
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);

	//创建子进程					
	BOOL res = CreateProcess(
		NULL,
		szBuffer,
		NULL,
		NULL,
		TRUE, // TRUE的时候，说明子进程可以继承父进程的句柄表
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
	DWORD errCode = GetLastError();
	println("{}, {}, {}, {}", pi.hProcess, pi.hThread, pi.dwProcessId, pi.dwThreadId);
	//设置事件为已通知					
	SetEvent(g_hEvent);

	//关闭句柄 内核对象是否会被销毁？					
	CloseHandle(g_hEvent);
}

int main(int argc, char* argv[])
{
	// 子进程程序
	//char szBuffer[256] = { 0 };
	//if (argc > 1) {
	//	strcpy_s(szBuffer, sizeof(szBuffer), argv[1]);
	//}
	//DWORD handle{ 0 };
	//sscanf(szBuffer, "%x", &handle);
	//println("argv[0] is {}, handle is {}", argv[0], handle);
	//HANDLE event = (HANDLE)handle;
	//println("begin waiting ...");
	//WaitForSingleObject(event, INFINITE);
	//DWORD errCode = GetLastError();
	//println("wait over .... {:X}", errCode);
	//getchar();
	TextInheritHandles();
	//TestCreateProcess();
	return 0;
}
