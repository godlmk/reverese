// practice.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "practice.h"
#include "Tools.h"

struct Person {
	int a, b, c, d;
};
void pPerson(Person p) {
	Person son = p;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	TCHAR szContent[] = TEXT("内容");
	TCHAR szTitle[] = TEXT("标题");
	MessageBox((HWND)1, szContent, szTitle, MB_OK);
	DWORD errorCode = GetLastError();
	if (errorCode != 0) {
		OutputDebugStringF("error code is %d", errorCode);
	}
	Person p;
	p.a = 1, p.b = 2;
	pPerson(p);
	Person b = { 0 };
	pPerson(b);
	return 0;
}


