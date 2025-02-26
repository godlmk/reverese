// Widechar.cpp: 定义应用程序的入口点。
//

#include "Widechar.h"

#include <stdio.h>
#include <print>
#include <locale.h>
#include <Windows.h>

using namespace std;
void print_characters()
{
	char c = '中';
	wchar_t wc = '中';
	wc = L'中';
}
void print_str()
{
	setlocale(LC_ALL, "");
	char x[] = "中国";
	wchar_t x1[] = L"中国";
	TCHAR cht[] = TEXT("中国");
	printf("%s\n", x);
	wprintf(L"%s\n", x1);
	int len = strlen(x);
	int len1 = wcslen(x1);
}

int main()
{
	//print_characters();
	print_str();
	return 0;
}
