// BaseRelocate.cpp: 定义应用程序的入口点。
//

#include "BaseRelocate.h"

using namespace std;
void PrintRelocatedTable(PVOID pFileBuffer)
{
	/*
	找到重定向表
	打印对应的结构
	*/
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pFileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(pFileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 重定向表的rva
	DWORD relocate_table_rva = opHeader->DataDirectory[5].VirtualAddress;
	// 重定向表的foa
	DWORD relocate_table_foa = RVA2FOA(pFileBuffer, relocate_table_rva);
	// 重定向表的第一个指针
	PIMAGE_BASE_RELOCATION first_relocation_table = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + relocate_table_foa),
		cur_table = first_relocation_table;

	//循环所有的重定向块
	int i = 0;
	while (cur_table->SizeOfBlock != 0)
	{
		++i;
		DWORD base = cur_table->VirtualAddress, size = cur_table->SizeOfBlock;
		int item_count = (size - 8) / 2;
		println("第{}块，基址为0x{:X},块大小为{} bytes，子项数量为{}", i, base, size, item_count);
		PWORD pItem = (PWORD)((PBYTE)cur_table + 8);
		WORD first4 = 0xf000, last12 = 0x0fff;
		for (int j = 0; j < item_count; ++j)
		{
			WORD value = pItem[j];
			// 前4bit
			int type = (value & first4) >> 12;
			// 后12bit
			WORD rva_offset = value & last12;
			if (type != 0)
			{
				println("第{}项，类型是{}，rva是0x{:X}", j + 1, type, base + rva_offset);
			}
			else {
				println("第{}项，类型是{}, rva: N/A", j + 1, type);
			}
		}
		cur_table = (PIMAGE_BASE_RELOCATION)((DWORD)cur_table + size);
	}
}
void Change_base(PVOID pFileBuffer, DWORD value)
{
	/*
	找到重定向表
	打印对应的结构
	*/
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pFileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(pFileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 重定向表的rva
	DWORD relocate_table_rva = opHeader->DataDirectory[5].VirtualAddress;
	// 重定向表的foa
	DWORD relocate_table_foa = RVA2FOA(pFileBuffer, relocate_table_rva);
	// 重定向表的第一个指针
	PIMAGE_BASE_RELOCATION first_relocation_table = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + relocate_table_foa),
		cur_table = first_relocation_table;

	//循环所有的重定向块
	int i = 0;
	while (cur_table->SizeOfBlock != 0)
	{
		++i;
		cur_table->VirtualAddress += value;
		DWORD base = cur_table->VirtualAddress, size = cur_table->SizeOfBlock;
		int item_count = (size - 8) / 2;
		println("第{}块，基址为0x{:X},块大小为{} bytes，子项数量为{}", i, base, size, item_count);
		PWORD pItem = (PWORD)((PBYTE)cur_table + 8);
		WORD first4 = 0xf000, last12 = 0x0fff;
		for (int j = 0; j < item_count; ++j)
		{
			WORD value = pItem[j];
			// 前4bit
			int type = (value & first4) >> 12;
			// 后12bit
			WORD rva_offset = value & last12;
			if (type != 0)
			{
				println("第{}项，类型是{}，rva是0x{:X}", j + 1, type, base + rva_offset);
			}
			else {
				println("第{}项，类型是{}, rva: N/A", j + 1, type);
			}
		}
		cur_table = (PIMAGE_BASE_RELOCATION)((DWORD)cur_table + size);
	}
}

int main()
{
	const char* filename = "C:/reverse/PE/defExport/Debug/defExport.dll";
	const char* uidllname = "C:/Windows/System32/cryptui.dll";
	//PrintRelocatedTable(ReadFileBuffer(uidllname));
	void* buffer;
	size_t size = ReadFileBuffer(filename, &buffer);
	Change_base(buffer, 0x10000000);
	write_file(buffer, filename, size);
	return 0;
}

