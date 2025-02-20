#include <windows.h>

extern "C" __declspec(dllimport) void MyFunction();

int main() {
    MyFunction();
    return 0;
}