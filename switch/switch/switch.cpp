// switch.cpp: 定义应用程序的入口点。
//

#include <iostream>

using namespace std;
void testSwitch(char a)
{
	switch (a)
	{
	case 101:
		cout << "a = 1" << endl;
		break;
		//case 102:
		//	cout << "a = 2" << endl;
		//	break;
	case 103:
		cout << "a = 3" << endl;
		break;
	case 104:
		cout << "a = 4" << endl;
		break;
	case 5:
		cout << "a = 5" << endl;
		break;
	default:
		cout << "a = " << a << endl;
		break;
	}
}

int main()
{
	testSwitch((char)103);
	return 0;
}
