#include <iostream>
// ��ƽջ���������ҵ��󣬷���ֵ��eax
int __declspec(naked) plus(int a, int b, int c) {
	__asm {
		// ��չ��ջ
		push ebp
		mov ebp, esp
		sub esp, 0x40
		// �����ֳ�
		push ebx
		push esi
		push edi
		// ��ʼ����ջ
		lea edi, dword ptr ds : [ebp - 0x40]
		mov eax, 0xcccccccc
		mov ecx, 0x10
		rep stosd
		// �ֲ�������ֵ
		mov dword ptr ds : [ebp - 4] , 2
		mov dword ptr ds : [ebp - 8] , 3
		mov dword ptr ds : [ebp - 0xc] , 4
		// ������
		mov eax, dword ptr ds : [ebp + 8]
		add eax, dword ptr ds : [ebp + 0xc]
		add eax, dword ptr ds : [ebp + 0x10]
		add eax, dword ptr ds : [ebp - 4]
		add eax, dword ptr ds : [ebp - 8]
		add eax, dword ptr ds : [ebp - 0xc]
		// �ָ��ֳ�
		pop edi
		pop esi
		pop ebx
		// �ָ���ջ
		mov esp, ebp
		pop ebp
		// ����
		ret
	}
}
int main() {
	std::cout << plus(1, 2, 3) << std::endl;
	return 0;
}