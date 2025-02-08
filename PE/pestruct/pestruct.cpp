// pestruct.cpp: 定义应用程序的入口点。
//

#include "pestruct.h"

using namespace std;

int main()
{
	const char* filename = "C:/compress/fg.exe";
	PrintNTHeaders(filename);
	cout << "Hello CMake." << endl;
	return 0;
}
