// pestruct.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include <Windows.h>

#include <iostream>
#include <format>

#include <stdio.h>
#include <winnt.h>

unsigned char* ReadPEFile(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		std::cout << "fopen failed" << '\n';
		fclose(fp);
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
void PrintNTHeaders(const char* filename) {
	unsigned char* buffer = ReadPEFile(filename);
	PIMAGE_DOS_HEADER pDOS = (PIMAGE_DOS_HEADER)buffer;
	if (pDOS->e_magic != IMAGE_DOS_SIGNATURE) {
		std::cout << "doesn't a ELF file" << '\n';
		return;
	}
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((DWORD)pDOS + pDOS->e_lfanew);
	if (pNT->Signature != IMAGE_NT_SIGNATURE) {
		std::cout << "doesn't a NT struct" << '\n';
		return;
	}
	std::cout << std::format({ "the nt Signature is {:X}\n" }, pNT->Signature);
	PIMAGE_FILE_HEADER pPE = (PIMAGE_FILE_HEADER)((DWORD)pNT + sizeof(pNT->Signature));
	std::cout << "------the FILE_HEADER field are these-----\n";
	auto str = std::format("Machine:{0:X}\nNumberOfSections:{1:X}\nTimeDateStamp:{2:X}\nPointerToSymbolsTable:{3:X}\nNumberOfSymbols : {4:X}\nSizeofOptionalHeader : {5:X}\nCharacterstics : {6:X}\n",
		pPE->Machine, pPE->NumberOfSections, pPE->TimeDateStamp, pPE->PointerToSymbolTable, pPE->NumberOfSymbols,
		pPE->SizeOfOptionalHeader, pPE->Characteristics);
	std::cout << str;
	PIMAGE_OPTIONAL_HEADER pOptional = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPE + sizeof(IMAGE_FILE_HEADER));
	std::cout << "-------the OPTIONAL_HEADER field are these---------\n";
	str = std::format(
		"Magic:{:X}\n"
		"MajorLinkerVersion:{:X}\n"
		"MinorLinkerVersion:{:X}\n"
		"SizeOfCode:{:X}\n"
		"SizeOfInitializedData:{:X}\n"
		"SizeOfUninitializedData : {:X}\n"
		"AddressOfEntryPoint : {:X}\n"
		"BaseOfCode:{:X}\n"
		"BaseOfData:{:X}\n"
		"ImageBase:{:X}\n"
		"SectionAlignment:{:X}\n"
		"FileAlignment:{:X}\n"
		"MajorOperatingSystemVersion:{:X}\n"
		"MinorOperatingSystemVersion:{:X}\n"
		"MajorImageVersion:{:X}\n"
		"MinorImageVersion:{:X}\n"
		"MajorSubsystemVersion:{:X}\n"
		"MinorSubsystemVersion:{:X}\n"
		"Win32VersionValue:{:X}\n"
		"SizeOfImage:{:X}\n"
		"SizeOfHeaders:{:X}\n"
		"CheckSum:{:X}\n"
		"DllCharacteristics:{:X}\n"
		"SizeOfStackReserve:{:X}\n"
		"SizeOfStackCommit:{:X}\n"
		"SizeOfHeapReserve:{:X}\n"
		"SizeOfHeapCommit:{:X}\n"
		"LoaderFlags:{:X}\n"
		"NumberOfRvaAndSizes:{:X}\n",
		pOptional->Magic,
		pOptional->MajorLinkerVersion,
		pOptional->MinorLinkerVersion,
		pOptional->SizeOfCode,
		pOptional->SizeOfInitializedData,
		pOptional->SizeOfUninitializedData,
		pOptional->AddressOfEntryPoint,
		pOptional->BaseOfCode,
		pOptional->BaseOfData,
		pOptional->ImageBase,
		pOptional->SectionAlignment,
		pOptional->FileAlignment,
		pOptional->MajorOperatingSystemVersion,
		pOptional->MinorOperatingSystemVersion,
		pOptional->MajorImageVersion,
		pOptional->MinorImageVersion,
		pOptional->MajorSubsystemVersion,
		pOptional->MinorSubsystemVersion,
		pOptional->Win32VersionValue,
		pOptional->SizeOfImage,
		pOptional->SizeOfHeaders,
		pOptional->CheckSum,
		pOptional->DllCharacteristics,
		pOptional->SizeOfStackReserve,
		pOptional->SizeOfStackCommit,
		pOptional->SizeOfHeapReserve,
		pOptional->SizeOfHeapCommit,
		pOptional->LoaderFlags,
		pOptional->NumberOfRvaAndSizes);
	free(buffer);
	std::cout << str;
}
