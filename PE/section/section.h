// section.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <Windows.h>
#include <iostream>
#include <print>
#include <string>

#include <stdio.h>
#include <winnt.h>

unsigned char* ReadFile(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		std::cout << "fopen failed" << '\n';
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	if (!buffer) {
		fclose(fp);
		return NULL;
	}
	int ret = fread(buffer, bytes, 1, fp);
	if (ret != 1) {
		free(buffer);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return buffer;
}

void PrintSection(const char* filename) {
	unsigned char* buffer = ReadFile(filename);
	if (!buffer) {
		std::cout << "ReadFile failed" << '\n';
		return;
	}
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::cout << "dosHeader->e_magic failed" << '\n';
		free(buffer);
		return;
	}
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::cout << "ntHeader->Signature failed" << '\n';
		free(buffer);
		return;
	}
	PIMAGE_FILE_HEADER fileHeader = &ntHeader->FileHeader;
	const int sectionCount = fileHeader->NumberOfSections;
	PIMAGE_SECTION_HEADER firstSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(IMAGE_FILE_HEADER)
		+ fileHeader->SizeOfOptionalHeader);
	char charname[9] = { 0 };
	for (int i = 0; i < sectionCount; i++) {
		PIMAGE_SECTION_HEADER curSection = firstSectionHeader + i;
		memcpy(charname, (const char*)curSection->Name, sizeof(curSection->Name));
		std::print("the {} section,name is {:s},Misc is {:X}, VirtualAddress is {:X},SizeOfRawData is {:X}, "
			"PointerToRawData:{:X},Characteristics:{:X}\n", i, charname,
			curSection->Misc.VirtualSize,
			curSection->VirtualAddress,
			curSection->SizeOfRawData,
			curSection->PointerToRawData,
			curSection->Characteristics);
	}

	free(buffer);
}

