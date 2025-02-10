// ShellCode.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。
#include <windows.h>
#include <winnt.h>
#include <print>
#include <stdio.h>
#include <cassert>

PIMAGE_DOS_HEADER GetDosHeader(LPVOID pImageBuffer)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::print("Invalid DOS signature\n");
		return NULL;
	}
	return dosHeader;
}
PIMAGE_NT_HEADERS GetNTHeader(LPVOID pImageBuffer, PIMAGE_DOS_HEADER dosHeader)
{
	const PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::print("Invalid NT signature\n");
		return NULL;
	}
	return ntHeader;
}
unsigned char* ReadPEFile(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		std::cout << "fopen failed" << '\n';
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	if (!buffer) {
		fclose(fp);
		exit(-1);
	}
	int ret = fread(buffer, bytes, 1, fp);
	if (ret != 1) {
		free(buffer);
		fclose(fp);
		exit(-1);
	}
	fclose(fp);
	return buffer;
}

PBYTE ImageFile2Memory(const char* filename) {
	unsigned char* buffer = ReadPEFile(filename);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		free(buffer);
		std::print("Invalid DOS signature\n");
		return NULL;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)(buffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		free(buffer);
		std::print("Invalid NT signature\n");
		return NULL;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	const DWORD ImageSize = optionalHeader->SizeOfImage;
	PBYTE const ImageBuffer = (PBYTE)malloc(ImageSize);
	if (!ImageBuffer) {
		free(buffer);
		std::print("malloc failed\n");
		return NULL;
	}
	memset(ImageBuffer, 0, ImageSize);
	// 拷贝所有的头和节表
	const DWORD sizeOfHeaderAndSection = optionalHeader->SizeOfHeaders;
	memcpy(ImageBuffer, buffer, sizeOfHeaderAndSection);
	// 拷贝每一节的数据到应该在的位置
	const PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	const int sectionCount = ntHeader->FileHeader.NumberOfSections;
	for (int i = 0; i < sectionCount; ++i) {
		PIMAGE_SECTION_HEADER curSection = firstSection + i;
		const DWORD offsetInMemoty = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD sectionSizeInFile = curSection->SizeOfRawData;
		memcpy(ImageBuffer + offsetInMemoty, buffer + offsetInFile, sectionSizeInFile);
	}
	free(buffer);
	return ImageBuffer;
}
bool ImageMemory2File(PBYTE pMemBuffer, const char* destPath) {
	const PIMAGE_DOS_HEADER const dosHeader = (PIMAGE_DOS_HEADER)pMemBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("has not dos format");
		return false;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pMemBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::print("Invalid NT signature\n");
		return false;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	DWORD fileSize = 0;
	// 加上所有header和节表的大小
	fileSize += optionalHeader->SizeOfHeaders;
	//	加上每一节的大小
	const size_t sectionCount = ntHeader->FileHeader.NumberOfSections;
	const PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		fileSize += curSection->SizeOfRawData;
	}
	PBYTE pFileBuffer = (PBYTE)malloc(fileSize);
	if (!pFileBuffer) {
		std::println("malloc failed");
		return false;
	}
	memset(pFileBuffer, 0, fileSize);
	// 拷贝所有的头和节表
	memcpy(pFileBuffer, pMemBuffer, optionalHeader->SizeOfHeaders);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		const DWORD offsetInMemry = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD curSectionSizeinFile = curSection->SizeOfRawData;
		memcpy(pFileBuffer + offsetInFile, pMemBuffer + offsetInMemry, curSectionSizeinFile);
	}
	FILE* fp = fopen(destPath, "wb");
	if (!fp) {
		std::println("open file fail");
		return false;
	}
	size_t ret = fwrite(pFileBuffer, fileSize, 1, fp);
	if (ret != 1) {
		std::println("fwrite fail");
		fclose(fp);
		return false;
	}
	fclose(fp);
	free(pFileBuffer);
	return true;
}
DWORD Rva2FileOffset(IN LPVOID pFileBuffer, IN DWORD Rva)
{
	auto const dosHeader = GetDosHeader(pFileBuffer);
	if (dosHeader == NULL) return -1;
	auto const ntHeader = GetNTHeader(pFileBuffer, dosHeader);
	if (ntHeader == NULL) return -1;
	auto const optionalHeader = &ntHeader->OptionalHeader;
	// 如果偏移量还在头部，那么直接返回即可
	if (Rva < optionalHeader->SizeOfHeaders)
	{
		return Rva;
	}
	auto const sectionCount = ntHeader->FileHeader.NumberOfSections;
	auto const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < sectionCount; ++i)
	{
		auto const curSection = firstSection + i;
		auto const virtualAddress = curSection->VirtualAddress;
		auto const ptoRawData = curSection->PointerToRawData;
		auto const sectionSize = curSection->SizeOfRawData;
		if (Rva >= virtualAddress && Rva < virtualAddress + sectionSize)
		{
			auto const ans = ptoRawData + (Rva - virtualAddress);
			std::println("the fva is {:X}", ans);
			return ans;
		}
	}
	return 0;
}
