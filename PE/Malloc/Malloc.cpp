// Malloc.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <stdio.h>
#include <winnt.h>
using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	FILE* fp = fopen("C:/compress/DTDebug/DTDebug/DTDebug.exe", "rb");
	if (fp == NULL)
	{
		cout << "fopen failed" << endl;
		exit(-1);
	}
	else
	{
		cout << "fopen success" << endl;
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	cout << "total byte: " << bytes << endl;
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	int ret = fread(buffer, 1, bytes, fp);
	fclose(fp);
	fp = fopen("C:/compress/DTDebug/DTDebug/dt.exe", "wb");
	fwrite(buffer, 1, bytes, fp);
	fclose(fp);
	free(buffer);
	return 0;
}
