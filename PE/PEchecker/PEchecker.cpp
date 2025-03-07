// PEchecker.cpp: 定义应用程序的入口点。
//

#include "PEchecker.h"

using namespace std;
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <print>

// 检查PE文件头和节表的完整性
bool CheckPEFileIntegrity(LPVOID pImageBuffer) {
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pImageBuffer);
	if (dosHeader == NULL) {
		std::cerr << "Invalid DOS header" << std::endl;
		return false;
	}

	PIMAGE_NT_HEADERS ntHeader = GetNTHeader(pImageBuffer, dosHeader);
	if (ntHeader == NULL) {
		std::cerr << "Invalid NT header" << std::endl;
		return false;
	}

	PIMAGE_FILE_HEADER fileHeader = &ntHeader->FileHeader;
	PIMAGE_OPTIONAL_HEADER optionalHeader = &ntHeader->OptionalHeader;

	// 检查SizeOfImage
	DWORD calculatedSizeOfImage = optionalHeader->SizeOfHeaders;
	PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < fileHeader->NumberOfSections; ++i) {
		calculatedSizeOfImage = max(calculatedSizeOfImage, sectionHeader[i].VirtualAddress +
			sectionHeader[i].Misc.VirtualSize);
		std::println("index {} , va is 0x{:X}, virtualsize is 0x{:X}, sum is 0x{:X}", i,
			sectionHeader[i].VirtualAddress, sectionHeader[i].Misc.VirtualSize,
			sectionHeader[i].VirtualAddress + sectionHeader[i].Misc.VirtualSize);
	}
	if (calculatedSizeOfImage != optionalHeader->SizeOfImage) {
		std::println("the calc size is 0x{:X}, op header's size is 0x{:X}",
			calculatedSizeOfImage, optionalHeader->SizeOfImage);
		return false;
	}

	// 检查节的对齐
	for (int i = 0; i < fileHeader->NumberOfSections; ++i) {
		if (sectionHeader[i].VirtualAddress % optionalHeader->SectionAlignment != 0) {
			std::cerr << i << " Section alignment error" << sectionHeader[i].VirtualAddress << std::endl;
			return false;
		}
		if (sectionHeader[i].PointerToRawData % optionalHeader->FileAlignment != 0) {
			std::cerr << "File alignment error" << std::endl;
			return false;
		}
	}

	return true;
}

int main() {
	// 读取PE文件到内存
	LPCTSTR filename = _T("C:/compress/drop/fg2.exe");
	void* pImageBuffer;
	DWORD size = ReadFileBuffer(filename, &pImageBuffer);

	// 检查PE文件完整性
	if (CheckPEFileIntegrity(pImageBuffer)) {
		std::cout << "PE file integrity check passed" << std::endl;
	}
	else {
		std::cerr << "PE file integrity check failed" << std::endl;
	}

	free(pImageBuffer);
	return 0;
}

