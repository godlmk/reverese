// PEchecker.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <print>
#include <Windows.h>
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
#include <tchar.h>

// 修改 ReadFileBuffer 函数，使其接受 LPCTSTR 类型的 filename 参数
DWORD ReadFileBuffer(LPCTSTR filename, void** pBuffer) {
	FILE* fp;
	errno_t err;

	// 根据 UNICODE 宏定义选择合适的 _tfopen_s 函数
#ifdef UNICODE
	err = _wfopen_s(&fp, filename, L"rb");
#else
	err = fopen_s(&fp, filename, "rb");
#endif

	if (fp == NULL)
	{
		char errStr[0x20];
		strerror_s(errStr, 0x20, errno);
		std::println("fread failed, because:{}", errStr);
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	if (!buffer) {
		char errStr[0x20];
		strerror_s(errStr, 0x20, errno);
		std::println("malloc failed, because:{}", errStr);
		fclose(fp);
		exit(-1);
	}
	int ret = fread(buffer, bytes, 1, fp);
	if (ret != 1) {
		char errStr[0x20];
		strerror_s(errStr, 0x20, errno);
		std::println("fread failed, because:{}", errStr);
		free(buffer);
		fclose(fp);
		exit(-1);
	}
	fclose(fp);
	*pBuffer = buffer;
	return bytes;
}
// TODO: 在此处引用程序需要的其他标头。
