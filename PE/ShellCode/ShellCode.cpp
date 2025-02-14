// ShellCode.cpp: 定义应用程序的入口点。
//

#include "ShellCode.h"
#include <Windows.h>
#include <array>
#include <algorithm>
#include <ranges>
#include <string>


constexpr BYTE shellcode[] = {
	0x6a, 0x00,0x6a, 0x00,0x6a, 0x00,0x6a, 0x00, // 参数入栈
	0xe8, 0x00,0x00,0x00,0x00,  // call MessageBoxA
	0xe9, 0x00,0x00,0x00,0x00  // jmp   EOA
};
constexpr int shellcodelen = sizeof(shellcode) / sizeof(shellcode[0]);
constexpr DWORD MessageAddr = 0x7592AF50;
// 数组下标和对应表的名字

std::string NAMES[] = {
	 "Export Directory",
"Import Directory",
"Resource Directory",
"Exception Directory"
"Security Directory",
"Base Relocation Table",
"Debug Directory",
"(X86 usage)",
"Architecture Specific Data",
"RVA of GP",
"TLS Directory",
"Load Configuration Directory",
"Bound Import Directory in headers",
"Import Address Table",
"Delay Load Import Descriptors",
"COM Runtime descriptor"
};

bool AddShellCode2DataSection(IN const char* infilename, IN const char* outfilename, int sectionNumber)
{
	/*
	读取磁盘文件
	在内存中拉伸成为ImageBuffer的样子
	判断数据块是否有足够空间放下shellcode
	修改shellcode中的call和jup指令的地址
	修改节的可执行权限
	修改oep的值
	将该内存的数据写入到新文件中
	*/
	void* memoryImage = ReadMemoryImage(infilename);

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
	void* memoryImage = ReadMemoryImage(infilename);

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
bool AddNewSection(IN const char* infilename, IN const char* outfilename)
{
	/*
		将文件读取到内存中来
		判断是否能够在不增加SizeOfHeaders的情况下添加一个节表
		申请一块新的足够大的内存来保存新的memoryImage
		复制原来的memoryImage到新的中
		添加一个节，并且将节的数据修改为要执行的代码
		添加一个节表（可以复制一个节表）
		修改节表中的节大小（对齐后），va，misc.VirtualSize（对其前），foa
		修改NumberOfSection
		修改SizeOfImage
		写回到文件中
	*/
	void* memoryImage = ReadMemoryImage(infilename);
	if (memoryImage == NULL)
	{
		std::println("打开文件失败");
		return false;
	}
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, dosHeader);
	PIMAGE_FILE_HEADER fileHader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 判断headers的空闲大小是否还符合要求
	const int freebytes = opHeader->SizeOfHeaders - (dosHeader->e_lfanew +
		sizeof(IMAGE_NT_HEADERS) + fileHader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
	if (freebytes < 2 * sizeof(IMAGE_SECTION_HEADER))
	{
		std::println("该文件的headers空闲区域无法放下一张节表");
		return false;
	}
	// 计算出添加一节后的ImageSize
	const int newSizeOfImage = opHeader->SizeOfImage + opHeader->SectionAlignment;
	void* newMemoryImage = calloc(1, newSizeOfImage);
	assert(newMemoryImage);
	memcpy(newMemoryImage, memoryImage, opHeader->SizeOfImage);
	free(memoryImage);
	dosHeader = GetDosHeader(newMemoryImage);
	ntHeaders = GetNTHeader(newMemoryImage, dosHeader);
	fileHader = &ntHeaders->FileHeader;
	opHeader = &ntHeaders->OptionalHeader;
	// 修改新增的一节的数据
	memcpy((void*)((DWORD)newMemoryImage + opHeader->SizeOfImage), shellcode, sizeof(shellcode));
	DWORD beginAddr = (DWORD)newMemoryImage + opHeader->SizeOfImage;
	memcpy((void*)(beginAddr), shellcode, sizeof(shellcode));
	// e8后的地址
	DWORD callAddr = MessageAddr - (DWORD)(opHeader->ImageBase + beginAddr + 0xd - (DWORD)newMemoryImage);
	*(DWORD*)(beginAddr + 0x9) = callAddr;
	// e9后的地址
	DWORD jmpAddr = (opHeader->ImageBase + opHeader->AddressOfEntryPoint) -
		(opHeader->ImageBase + ((DWORD)(beginAddr + 0x12) - (DWORD)newMemoryImage));
	*(PDWORD)(beginAddr + 0xe) = jmpAddr;
	// 修改eop的值
	opHeader->AddressOfEntryPoint = beginAddr - (DWORD)newMemoryImage;

	// 复制第一个节表到最后
	PIMAGE_SECTION_HEADER firSection = IMAGE_FIRST_SECTION(ntHeaders);
	const int sectionCount = fileHader->NumberOfSections;
	memcpy(firSection + sectionCount, firSection, sizeof(IMAGE_SECTION_HEADER));
	memset(firSection + sectionCount + 1, 0, sizeof(IMAGE_SECTION_HEADER));
	PIMAGE_SECTION_HEADER curSection = firSection + sectionCount;
	const char* name = ".lmk";
	memset(curSection->Name, 0, sizeof(curSection->Name));
	memcpy(curSection->Name, name, 4);
	curSection->Misc.VirtualSize = opHeader->SectionAlignment;
	curSection->SizeOfRawData = opHeader->SectionAlignment;
	// 计算出上一页的大小，每一页的大小必须是SectionALignment的整数倍并且能够全覆盖SizeOfRawData和VirtualSize
	PIMAGE_SECTION_HEADER preSection = curSection - 1;
	DWORD sectionSize = (preSection->SizeOfRawData) > (preSection->Misc.VirtualSize) ? (preSection->SizeOfRawData) : (preSection->Misc.VirtualSize);
	if (sectionSize % opHeader->SectionAlignment != 0)
	{
		sectionSize = (sectionSize / opHeader->SectionAlignment + 1) * opHeader->SectionAlignment;
	}
	curSection->VirtualAddress = preSection->VirtualAddress + sectionSize;
	//计算在文件中的偏移，这个比较固定，因为SizeOfRawData本来就是对其后的大小
	curSection->PointerToRawData = preSection->PointerToRawData + preSection->SizeOfRawData;
	// 修改节数量
	fileHader->NumberOfSections = fileHader->NumberOfSections + 1;
	// 修改内存镜像大小
	opHeader->SizeOfImage = newSizeOfImage;
	ImageMemory2File((PBYTE)newMemoryImage, outfilename);
	free(newMemoryImage);
	return true;
}
bool BigerSection(IN const char* infilename, IN const char* outfilename)
{
	/*
		读取到内存中
		计算出新建节后memoryImage的大小
		重新申请内存并且拷贝原来的数据到新的内存中
		填充最后一个节新的空闲区域
		修改节表
		修改节表属性
		修改SizeOfImage
	*/
	void* memoryImage = ReadMemoryImage(infilename);
	assert(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, GetDosHeader(memoryImage));
	int const imageSize = ntHeaders->OptionalHeader.SizeOfImage + ntHeaders->OptionalHeader.SectionAlignment;
	void* newMemoryImage = calloc(1, imageSize);
	assert(newMemoryImage);
	memcpy(newMemoryImage, memoryImage, ntHeaders->OptionalHeader.SizeOfImage);
	free(memoryImage);

	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(newMemoryImage);
	ntHeaders = GetNTHeader(newMemoryImage, dosHeader);
	PIMAGE_FILE_HEADER fileHeader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 填充要执行的代码
	int const sectionCount = fileHeader->NumberOfSections;
	PIMAGE_SECTION_HEADER lastSection = IMAGE_FIRST_SECTION(ntHeaders) + (sectionCount - 1);
	lastSection->Misc.VirtualSize = imageSize - lastSection->VirtualAddress;
	lastSection->SizeOfRawData = lastSection->Misc.VirtualSize;
	opHeader->SizeOfImage = imageSize;
	ImageMemory2File((PBYTE)newMemoryImage, outfilename);
	return true;
}
bool MergeSection(IN const char* infilename, IN const char* outfilename)
{
	void* memoryImage = ReadMemoryImage(infilename);
	if (memoryImage == NULL) return false;
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, GetDosHeader(memoryImage));
	PIMAGE_FILE_HEADER fileHeader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(ntHeaders);
	firstSection->SizeOfRawData = opHeader->SizeOfImage - firstSection->VirtualAddress;
	firstSection->Misc.VirtualSize = opHeader->SizeOfImage - firstSection->VirtualAddress;
	// 第一节具有所有的属性
	for (int i = 1; i < fileHeader->NumberOfSections; ++i)
	{
		firstSection->Characteristics |= (firstSection + i)->Characteristics;
	}
	fileHeader->NumberOfSections = 1;
	bool success = ImageMemory2File((PBYTE)memoryImage, outfilename);
	free(memoryImage);
	return success;
}
void PirntDataDirectory(IN const char* filename)
{
	void* memoryImage = ReadFileBuffer(filename);
	if (memoryImage == NULL) return;
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, GetDosHeader(memoryImage));
	PIMAGE_FILE_HEADER fileHeader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES - 1; ++i)
	{
		PIMAGE_DATA_DIRECTORY curData = opHeader->DataDirectory + i;
		std::println("The name of DataDirectory[{}]: {}", i, NAMES[i]);
		std::println("VirtualAddress:{:x}", curData->VirtualAddress);
		std::println("Size:{:x}", curData->Size);
	}
}
int main()
{
	const char* tenctentoriginfilename = "C:/softpackage/WeMeeting.exe";
	const char* tencentoutfilename = "C:/softpackage/we.exe";
	const char* fgoldname = "C:/compress/drop/fg.exe";
	const char* fgnewname = "C:/compress/drop/fg2.exe";
	//AddShellCode2DataSection(fgoldname, fgnewname, 4);
	AddNewSection(fgoldname, fgnewname);
	//BigerSection(fgoldname, fgnewname);
	//MergeSection(fgoldname, fgnewname);
	//PirntDataDirectory(fgoldname);
	return 0;
}
