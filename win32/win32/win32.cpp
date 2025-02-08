// win32.cpp: 定义应用程序的入口点。
//

#include <iostream>

char getIndex(const char* str, int index) {
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] >= 0xa1) {
			i += 4;
		}
		else {
			i += 1;
		}
		--index;
		if (index == 0) break;
	}
	return str[i];
}
int main() {
	std::cout << getIndex("fewifw你好，世界", 6) << std::endl;
	return 0;
}