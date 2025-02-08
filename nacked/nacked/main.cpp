#include <iostream>
// 内平栈，参数从右到左，返回值在eax
int __declspec(naked) plus(int a, int b, int c) {
	__asm {
		// 拓展堆栈
		push ebp
		mov ebp, esp
		sub esp, 0x40
		// 保存现场
		push ebx
		push esi
		push edi
		// 初始化堆栈
		lea edi, dword ptr ds : [ebp - 0x40]
		mov eax, 0xcccccccc
		mov ecx, 0x10
		rep stosd
		// 局部变量赋值
		mov dword ptr ds : [ebp - 4] , 2
		mov dword ptr ds : [ebp - 8] , 3
		mov dword ptr ds : [ebp - 0xc] , 4
		// 函数体
		mov eax, dword ptr ds : [ebp + 8]
		add eax, dword ptr ds : [ebp + 0xc]
		add eax, dword ptr ds : [ebp + 0x10]
		add eax, dword ptr ds : [ebp - 4]
		add eax, dword ptr ds : [ebp - 8]
		add eax, dword ptr ds : [ebp - 0xc]
		// 恢复现场
		pop edi
		pop esi
		pop ebx
		// 恢复堆栈
		mov esp, ebp
		pop ebp
		// 返回
		ret
	}
}
int main() {
	std::cout << plus(1, 2, 3) << std::endl;
	return 0;
}