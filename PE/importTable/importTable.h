// importTable.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <windows.h>
#include <print>
#include <cassert>

int Align(int origin, int alignment)
{
	return ((origin + alignment - 1) / alignment) * alignment;
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
DWORD ReadFileBuffer(const char* filename, void** pBuffer) {
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
	*pBuffer = buffer;
	return bytes;
}

PBYTE ReadMemoryImage(const char* filename) {
	unsigned char* buffer;
	DWORD size = ReadFileBuffer(filename, (void**)&buffer);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		free(buffer);
		std::println("Invalid DOS signature\n");
		return NULL;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)(buffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		free(buffer);
		std::println("Invalid NT signature\n");
		return NULL;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	std::println("offset is {:X}, use & is {:X}", (DWORD)optionalHeader, (DWORD)&ntHeader->OptionalHeader);
	assert(optionalHeader == &ntHeader->OptionalHeader);
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
		std::println("Invalid NT signature");
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
		std::println("malloc failed, because:{}", strerror(errno));
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
			//std::println("the foa is {:X}", ans);
			return ans;
		}
	}
	return 0;
}
DWORD FOA2RVA(IN LPVOID pMemoryBuffer, IN DWORD Foa)
{
	auto const dosHeader = GetDosHeader(pMemoryBuffer);
	if (dosHeader == NULL) return -1;
	auto const ntHeader = GetNTHeader(pMemoryBuffer, dosHeader);
	if (ntHeader == NULL) return -1;
	auto const optionalHeader = &ntHeader->OptionalHeader;
	// 如果偏移量还在头部，那么直接返回即可
	if (Foa < optionalHeader->SizeOfHeaders)
	{
		return Foa;
	}
	auto const sectionCount = ntHeader->FileHeader.NumberOfSections;
	auto const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < sectionCount; ++i)
	{
		auto const curSection = firstSection + i;
		auto const virtualAddress = curSection->VirtualAddress;
		auto const ptoRawData = curSection->PointerToRawData;
		auto const sectionSize = curSection->SizeOfRawData;
		if (Foa >= ptoRawData && Foa < ptoRawData + sectionSize)
		{
			auto const ans = virtualAddress + (Foa - ptoRawData);
			//std::println("the rva is {:X}", ans);
			return ans;
		}
	}
	return 0;
}
// 写入内存中的数据到对应文件名
bool write_file(IN void* buffer, IN const char* filename, IN const DWORD size)
{
	FILE* fp = fopen(filename, "wb");
	auto ans = fwrite(buffer, size, 1, fp);
	if (ans == -1)
	{
		std::println("fwrite fail, becase:", strerror(errno));
		return false;
	}
	fclose(fp);
	return true;
}

DWORD Offset(PVOID buffer, PVOID addr)
{
	return (DWORD)(addr)-(DWORD)buffer;
}

void* GetBufferAddr(PVOID buffer, DWORD rva)
{
	DWORD foa = RVA2FOA(buffer, rva);
	return (PVOID)((PBYTE)buffer + foa);
}


/*
	新增一个节，大小为0x1000 字节的节，之后返回新增节的FOA
*/
bool AddNewSection(IN const char* infilename, int& newFOA)
{
	/*
		将文件读取到内存中来
		判断是否能够在不增加SizeOfHeaders的情况下添加一个节表
		申请一块新的足够大的内存来保存新的memoryImage
		复制原来的memoryImage到新的中
		添加一个节
		添加一个节表（可以复制一个节表）
		修改节表中的节大小（对齐后），va，misc.VirtualSize（对其前），foa
		修改NumberOfSection
		修改SizeOfImage
		写回到文件中
	*/
	void* memoryImage = ReadMemoryImage(infilename);
	if (memoryImage == NULL)
	{
		std::println("打开内存镜像buffer文件失败");
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
	// 计算出添加一节后的ImageSize,这里增加0x1000即可
	const int newSizeOfImage = opHeader->SizeOfImage + 0x1000;
	void* newMemoryImage = calloc(1, newSizeOfImage);
	assert(newMemoryImage);
	memcpy(newMemoryImage, memoryImage, opHeader->SizeOfImage);
	free(memoryImage);
	dosHeader = GetDosHeader(newMemoryImage);
	ntHeaders = GetNTHeader(newMemoryImage, dosHeader);
	fileHader = &ntHeaders->FileHeader;
	opHeader = &ntHeaders->OptionalHeader;
	// 复制第一个节表到最后
	PIMAGE_SECTION_HEADER firSection = IMAGE_FIRST_SECTION(ntHeaders);
	const int sectionCount = fileHader->NumberOfSections;
	memcpy(firSection + sectionCount, firSection, sizeof(IMAGE_SECTION_HEADER));
	memset(firSection + sectionCount + 1, 0, sizeof(IMAGE_SECTION_HEADER));
	PIMAGE_SECTION_HEADER curSection = firSection + sectionCount;
	const char* name = "export";
	memset(curSection->Name, 0, sizeof(curSection->Name));
	memcpy(curSection->Name, name, strlen(name));
	curSection->Misc.VirtualSize = 0x1000;
	curSection->SizeOfRawData = 0x1000;
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
	// 修改输入值
	newFOA = curSection->PointerToRawData;
	// 修改节数量
	fileHader->NumberOfSections = fileHader->NumberOfSections + 1;
	// 修改内存镜像大小
	opHeader->SizeOfImage = newSizeOfImage;
	auto ans = ImageMemory2File((PBYTE)newMemoryImage, infilename);
	free(newMemoryImage);
	return ans;

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

	int const sectionCount = fileHeader->NumberOfSections;
	PIMAGE_SECTION_HEADER lastSection = IMAGE_FIRST_SECTION(ntHeaders) + (sectionCount - 1);
	lastSection->Misc.VirtualSize = imageSize - lastSection->VirtualAddress;
	lastSection->SizeOfRawData = lastSection->Misc.VirtualSize;
	opHeader->SizeOfImage = imageSize;
	ImageMemory2File((PBYTE)newMemoryImage, outfilename);
	return true;
}
