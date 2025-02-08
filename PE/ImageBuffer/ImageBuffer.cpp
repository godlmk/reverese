// ImageBuffer.cpp: 定义应用程序的入口点。
//

#include "ImageBuffer.h"

using namespace std;

int main()
{
	PBYTE ans = (PBYTE)ImageFile2Buffer("C:/compress/fg.exe");
	// std::print("ImageBuffer is 0X{:X}\n", ans);
	cout << "Hello CMake." << endl;
	ImageMemory2File(ans, "C:/compress/fgback.exe");
	return 0;
}
