// ExportTable.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <cassert>
#include <print>
#include <stdio.h>
#include <windows.h>
int Align(int origin, int alignment)
{
	return (origin / alignment - 1) * alignment;
}

PIMAGE_DOS_HEADER GetDosHeader(LPVOID pImageBuffer)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("Invalid DOS signature");
		return NULL;
	}
	return dosHeader;
}
PIMAGE_NT_HEADERS GetNTHeader(LPVOID pImageBuffer, PIMAGE_DOS_HEADER dosHeader)
{
	const PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::println("Invalid NT signature");
		return NULL;
	}
	return ntHeader;
}
unsigned char* ReadFileBuffer(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		std::println("fread failed, because:{}", strerror(errno));
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	if (!buffer) {
		std::println("malloc failed, because:{}", strerror(errno));
		fclose(fp);
		exit(-1);
	}
	int ret = fread(buffer, bytes, 1, fp);
	if (ret != 1) {
		std::println("fread failed, because:{}", strerror(errno));
		free(buffer);
		fclose(fp);
		exit(-1);
	}
	fclose(fp);
	return buffer;
}

PBYTE ReadMemoryImage(const char* filename) {
	unsigned char* buffer = ReadFileBuffer(filename);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		free(buffer);
		std::println("Invalid DOS signature");
		return NULL;
	}
	PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)(buffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		free(buffer);
		std::println("Invalid NT signature");
		return NULL;
	}
	PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	const DWORD ImageSize = optionalHeader->SizeOfImage;
	PBYTE const ImageBuffer = (PBYTE)malloc(ImageSize);
	if (!ImageBuffer) {
		free(buffer);
		std::println("malloc failed, because:{}", strerror(errno));
		return NULL;
	}
	memset(ImageBuffer, 0, ImageSize);
	// 拷贝所有的头和节表
	const DWORD sizeOfHeaderAndSection = optionalHeader->SizeOfHeaders;
	memcpy(ImageBuffer, buffer, sizeOfHeaderAndSection);
	// 拷贝每一节的数据到应该在的位置
	PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
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
	PIMAGE_DOS_HEADER const dosHeader = (PIMAGE_DOS_HEADER)pMemBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("has not dos format");
		return false;
	}
	PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pMemBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::println("Invalid NT signature");
		return false;
	}
	PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	DWORD fileSize = 0;
	// 加上所有header和节表的大小
	fileSize += optionalHeader->SizeOfHeaders;
	//	加上每一节的大小
	const size_t sectionCount = ntHeader->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (size_t i = 0; i < sectionCount; ++i) {
		PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		fileSize += curSection->SizeOfRawData;
	}
	PBYTE pFileBuffer = (PBYTE)malloc(fileSize);
	if (!pFileBuffer) {
		std::println("malloc failed, because:{}", strerror(errno));
		return false;
	}
	memset(pFileBuffer, 0, fileSize);
	// 拷贝所有的头和节表
	memcpy(pFileBuffer, pMemBuffer, optionalHeader->SizeOfHeaders);
	for (size_t i = 0; i < sectionCount; ++i) {
		PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		const DWORD offsetInMemry = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD curSectionSizeinFile = curSection->SizeOfRawData;
		PBYTE dest = pFileBuffer + offsetInFile;
		PBYTE src = pMemBuffer + offsetInMemry;
		memcpy(dest, src, curSectionSizeinFile);
	}
	FILE* fp = fopen(destPath, "wb");
	if (!fp) {
		std::println("fopen failed, because:{}", strerror(errno));
		return false;
	}
	size_t ret = fwrite(pFileBuffer, fileSize, 1, fp);
	if (ret != 1) {
		std::println("fwrite failed, because:{}", strerror(errno));
		fclose(fp);
		return false;
	}
	fclose(fp);
	free(pFileBuffer);
	return true;
}
DWORD RVA2FOA(IN LPVOID pMemoryBuffer, IN DWORD Rva)
{
	auto const dosHeader = GetDosHeader(pMemoryBuffer);
	if (dosHeader == NULL) return -1;
	auto const ntHeader = GetNTHeader(pMemoryBuffer, dosHeader);
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
			return ans;
		}
	}
	return 0;
}
// 返回对应函数名的函数地址 （RVA)
PVOID GetFunctionAddrByName(PVOID pFileBuffer, IN PSTR funcname);
// 返回对应ordinal的函数地址额 （RVA)
PVOID GetFunctionAddrByOrdinals(PVOID pFileBuffer, IN int ordinal);


// TODO: 在此处引用程序需要的其他标头。
