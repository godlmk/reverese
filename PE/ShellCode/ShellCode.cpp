// ShellCode.cpp: 定义应用程序的入口点。
//

#include "ShellCode.h"
#include <Windows.h>
#include <array>
#include <algorithm>

using namespace std;

constexpr BYTE shellcode[] = {
	0x6a, 0x00,0x6a, 0x00,0x6a, 0x00,0x6a, 0x00, // 参数入栈
	0xe8, 0x00,0x00,0x00,0x00,  // call MessageBoxA
	0xe9, 0x00,0x00,0x00,0x00  // jmp   EOA
};
constexpr int shellcodelen = sizeof(shellcode) / sizeof(shellcode[0]);
constexpr DWORD MessageAddr = 0x7592AF50;

bool AddShellCode2DataSection(IN const char* infilename, IN const char* outfilename, int sectionNumber)
{
	/*
	读取磁盘文件
	在内存中拉伸成为ImageBuffer的样子
	判断代码块是否有足够空间放下shellcode
	修改shellcode中的call和jup指令的地址
	修改oep的值
	将该内存的数据写入到新文件中
	*/
	void* memoryImage = ImageFile2Memory(infilename);

	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, dosHeader);

	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	const int sectionCount = ntHeaders->FileHeader.NumberOfSections;
	if (sectionNumber > sectionCount)
	{
		std::println("sectionNumber is too big");
		return false;
	}
	PIMAGE_SECTION_HEADER firstSection = ((PIMAGE_SECTION_HEADER)((ULONG_PTR)(ntHeaders)+((LONG)__builtin_offsetof(IMAGE_NT_HEADERS, OptionalHeader)) + ((ntHeaders))->FileHeader.SizeOfOptionalHeader));
	// 判断是否有足够空间放下shellcode
	PIMAGE_SECTION_HEADER curSection = firstSection + (sectionNumber - 1);
	if (curSection->SizeOfRawData - curSection->Misc.VirtualSize < shellcodelen)
	{
		std::println("the {x}th section can't hold shellcode", sectionNumber - 1);
		free(memoryImage);
		return false;
	}
	PBYTE beginAddr = (PBYTE)((DWORD)memoryImage + curSection->VirtualAddress + curSection->Misc.VirtualSize);
	memcpy(beginAddr, shellcode, 18);
	// 对应的call地址
	DWORD callAddr = MessageAddr - (opHeader->ImageBase + ((DWORD)(beginAddr + 0xd) - (DWORD)memoryImage));
	*(PDWORD)(beginAddr + 0x9) = callAddr;
	// 对应的eoa地址
	DWORD jmpAddr = (opHeader->ImageBase + opHeader->AddressOfEntryPoint) -
		(opHeader->ImageBase + ((DWORD)(beginAddr + 0x12) - (DWORD)memoryImage));
	*(PDWORD)(beginAddr + 0xe) = jmpAddr;
	// 修改该节的权限
	curSection->Characteristics |= firstSection->Characteristics;
	// 修改oep的值
	opHeader->AddressOfEntryPoint = (DWORD)beginAddr - (DWORD)memoryImage;
	bool success = ImageMemory2File((PBYTE)memoryImage, outfilename);
	free(memoryImage);
	return success;
}

bool AddShellCode2CodeSection(IN const char* infilename, IN const char* outfilename)
{
	/*
	读取磁盘文件
	在内存中拉伸成为ImageBuffer的样子
	判断代码块是否有足够空间放下shellcode
	修改shellcode中的call和jup指令的地址
	修改oep的值
	将该内存的数据写入到新文件中
	*/
	void* memoryImage = ImageFile2Memory(infilename);

	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	PIMAGE_SECTION_HEADER firstSection = ((PIMAGE_SECTION_HEADER)((ULONG_PTR)(ntHeaders)+((LONG)__builtin_offsetof(IMAGE_NT_HEADERS, OptionalHeader)) + ((ntHeaders))->FileHeader.SizeOfOptionalHeader));
	// 判断是否有足够空间放下shellcode
	if (firstSection->SizeOfRawData - firstSection->Misc.VirtualSize < shellcodelen)
	{
		std::println(".text can't hold shellcode");
		free(memoryImage);
		return false;
	}
	PBYTE beginAddr = (PBYTE)((DWORD)memoryImage + firstSection->VirtualAddress + firstSection->Misc.VirtualSize);
	memcpy(beginAddr, shellcode, 18);
	// 对应的call地址
	DWORD callAddr = MessageAddr - (opHeader->ImageBase + ((DWORD)(beginAddr + 0xd) - (DWORD)memoryImage));
	*(PDWORD)(beginAddr + 0x9) = callAddr;
	// 对应的eoa地址
	DWORD jmpAddr = (opHeader->ImageBase + opHeader->AddressOfEntryPoint) -
		(opHeader->ImageBase + ((DWORD)(beginAddr + 0x12) - (DWORD)memoryImage));
	*(PDWORD)(beginAddr + 0xe) = jmpAddr;
	// 修改oep的值
	opHeader->AddressOfEntryPoint = (DWORD)beginAddr - (DWORD)memoryImage;
	bool success = ImageMemory2File((PBYTE)memoryImage, outfilename);
	free(memoryImage);
	return success;
}

int main()
{
	const char* originfilename = "C:/softpackage/WeMeeting.exe";
	const char* outfilename = "C:/softpackage/we.exe";
	AddShellCode2CodeSection(originfilename, outfilename);
	return 0;
}
