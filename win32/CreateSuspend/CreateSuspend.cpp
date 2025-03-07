// CreateSuspend.cpp: 定义应用程序的入口点。
//

#include "CreateSuspend.h"
#include <Windows.h>
#include <print>

using namespace std;

void CreateSuspendProcess()
{
	STARTUPINFO ie_si = { 0 };
	PROCESS_INFORMATION ie_pi;
	ie_si.cb = sizeof(ie_si);

	TCHAR szBuffer[256] = "C:/compress/drop/fg.exe";
	CreateProcess(
		NULL,                    // name of executable module						
		szBuffer,                // command line string						
		NULL, 					 // SD	
		NULL,  		             // SD				
		FALSE,                   // handle inheritance option						
		CREATE_SUSPENDED,        // creation flags  					
		NULL,                    // new environment block						
		NULL,                    // current directory name						
		&ie_si,                  // startup information						
		&ie_pi                   // process information						
	);


	CONTEXT contx;
	contx.ContextFlags = CONTEXT_FULL;


	GetThreadContext(ie_pi.hThread, &contx);

	//获取入口点							
	DWORD dwEntryPoint = contx.Eax;

	//获取ImageBase							
	char* baseAddress = (CHAR*)contx.Ebx + 8;

	memset(szBuffer, 0, 256);

	ReadProcessMemory(ie_pi.hProcess, baseAddress, szBuffer, 4, NULL);
	println("{:p}, {:s}", (void*)baseAddress, szBuffer);

	ResumeThread(ie_pi.hThread);


}
int main()
{
	CreateSuspendProcess();
	return 0;
}
