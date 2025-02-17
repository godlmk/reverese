// RelocationMove.cpp: 定义应用程序的入口点。
//

#include "RelocationMove.h"


bool RelocaTableMove(IN const char* filename)
{
	// 新增一节
	// 找到重定向表
	// 复制全部项
	int new_section_foa;
	bool add_success = AddNewSection(filename, new_section_foa);
	if (!add_success)
	{
		std::println("新增节失败");
		return false;
	}
	void* file_buffer;
	int size = ReadFileBuffer(filename, &file_buffer);

	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	// 重定向表的rva
	DWORD relocation_table_rva = op_header->DataDirectory[5].VirtualAddress;
	// foa
	DWORD relocation_talbe_foa = RVA2FOA(file_buffer, relocation_table_rva);
	// 指针
	PIMAGE_BASE_RELOCATION prelocation_table = (PIMAGE_BASE_RELOCATION)((PBYTE)file_buffer + relocation_talbe_foa),
		cur_table = prelocation_table;
	// 新增节的地址
	PBYTE dest = (PBYTE)((PBYTE)file_buffer + new_section_foa);
	int i{ 0 };
	while (cur_table->SizeOfBlock != 0)
	{
		++i;
		DWORD base = cur_table->VirtualAddress, size = cur_table->SizeOfBlock;
		memcpy(dest, cur_table, size);
		dest += size;
		cur_table = (PIMAGE_BASE_RELOCATION)((DWORD)cur_table + size);
	}
	// 最后的中止结构
	memset(dest, 0, 8);
	std::println("has {} item", i);
	// 写回去
	write_file(file_buffer, filename, size);
	return true;
}


int main()
{
	const char* filename = "C:/reverse/PE/defExport/Debug/defExport.dll";
	RelocaTableMove(filename);
	return 0;
}
