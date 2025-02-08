// scale.cpp: 定义应用程序的入口点。
//
#include <vector>
#include <unordered_map>
#include <ranges>

#include "scale.h"

using namespace std;

static vector<char> charset = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
// static unordered_map<char, int> pos = {
// 	{'0', 0},
// 	{'1', 1},
// 	{'2', 2},
// 	{'3', 3},
// 	{'4', 4},
// 	{'5', 5},
// 	{'6', 6},
// 	{'7', 7},
// 	{'8', 8},
// 	{'9', 9},
// 	{'a', 10},
// 	{'b', 11},
// 	{'c', 12},
// 	{'d', 13},
// 	{'e', 14},
// 	{'f', 15}
// };
string scale(int base, int num) {
	if (base < 2 || base > 16) return "-1";
	string ans;
	while (num > 0) {
		char cur = charset[num % base];
		ans += cur;
		num /= base;
	}
	reverse(ans.begin(), ans.end());
	return ans;
}

int main()
{
	cout << scale(2, 100) << '\n';
	return 0;
}
