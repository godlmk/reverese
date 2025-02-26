// ResourceTable.cpp: 定义应用程序的入口点。
//

#include "ResourceTable.h"

using namespace std;
void PrintThirdInfo(PIMAGE_RESOURCE_DIRECTORY third_res_direcotry, void* res_addr)
{
	PIMAGE_RESOURCE_DIRECTORY_ENTRY res_entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((PBYTE)third_res_direcotry + sizeof(IMAGE_RESOURCE_DIRECTORY)), cur_entry = NULL;
	int count = third_res_direcotry->NumberOfIdEntries + third_res_direcotry->NumberOfNamedEntries;
	for (int i = 0; i < count; ++i)
	{
		cur_entry = res_entry + i;
		println("代码页是0x{:X}", cur_entry->Name);
		PIMAGE_DATA_DIRECTORY data = (PIMAGE_DATA_DIRECTORY)((PBYTE)res_addr + cur_entry->OffsetToDirectory);
		println("rva is 0x{:X}, size is 0x{:X}", data->VirtualAddress,
			data->Size);
	}
}
void PrintResourceDirectory(PIMAGE_RESOURCE_DIRECTORY res_directory, void* file_buffer, int level, void* res_addr)
{
	PIMAGE_RESOURCE_DIRECTORY_ENTRY res_entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((PBYTE)res_directory + sizeof(IMAGE_RESOURCE_DIRECTORY));
	int count = res_directory->NumberOfIdEntries + res_directory->NumberOfNamedEntries;

	for (int i = 0; i < count; ++i)
	{
		PIMAGE_RESOURCE_DIRECTORY_ENTRY cur_entry = res_entry + i;
		for (int j = 0; j < level; ++j) print("   ");  // Indent based on level
		if (cur_entry->NameIsString)
		{
			// Handle named entries if necessary
			println("Name: {}", (PCHAR)(GetBufferAddr(file_buffer, cur_entry->Name)));
		}
		else
		{
			//cout << "ID: " << cur_entry->Name << endl;
			println("ID: {}", cur_entry->Name);
		}

		if (cur_entry->DataIsDirectory)
		{
			PIMAGE_RESOURCE_DIRECTORY sub_directory = (PIMAGE_RESOURCE_DIRECTORY)
				((PBYTE)res_addr + (cur_entry->OffsetToDirectory & 0x7FFFFFFF));
			PrintResourceDirectory(sub_directory, file_buffer, level + 1, res_addr);
		}
		else
		{
			PIMAGE_RESOURCE_DATA_ENTRY data_entry = (PIMAGE_RESOURCE_DATA_ENTRY)
				((PBYTE)res_addr + cur_entry->OffsetToData);
			for (int j = 0; j < level; ++j) print("   "); // Indent based on level
			println("RVA is 0x{:X}, size is 0x{:X}", data_entry->OffsetToData,
				data_entry->Size);
		}
	}
}
void PrintResource(LPCTSTR filename, const int type)
{
	/* 找到资源表首地址
	根据type找到第一层需要解析的项
	找到所有第二层的项并打印资源编号
	找到第三层所有的项并打印代码页
	根据第三层找到rva和size
	*/
	void* file_buffer;
	DWORD size = ReadFileBuffer(filename, &file_buffer);
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	// 第一层
	PIMAGE_RESOURCE_DIRECTORY res_directory = (PIMAGE_RESOURCE_DIRECTORY)GetBufferAddr(file_buffer,
		op_header->DataDirectory[2].VirtualAddress);
	// 如果type是-1，打印所有的类型
	if (type == -1)
	{
		PrintResourceDirectory(res_directory, file_buffer, 0, res_directory);
		return;
	}
	DWORD count = res_directory->NumberOfIdEntries + res_directory->NumberOfNamedEntries;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY res_entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((PBYTE)res_directory + sizeof(IMAGE_RESOURCE_DIRECTORY));
	PIMAGE_RESOURCE_DIRECTORY_ENTRY need_entry = NULL;
	for (DWORD i = 0; i < count; ++i)
	{
		PIMAGE_RESOURCE_DIRECTORY_ENTRY cur_entry = res_entry + i;
		if (cur_entry->NameIsString == 0)
		{
			if (cur_entry->Name == type)
			{
				need_entry = cur_entry;
				print("类型是：{}--", cur_entry->Name);
				break;
			}
		}
	}
	if (need_entry == NULL)
	{
		println("can't find need type {}", type);
		return;
	}
	// 取到第二层
	PIMAGE_RESOURCE_DIRECTORY second_res = (PIMAGE_RESOURCE_DIRECTORY)
		((PBYTE)res_directory + need_entry->OffsetToDirectory);
	// 打印所有信息
	count = second_res->NumberOfIdEntries + second_res->NumberOfNamedEntries;

	res_entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((PBYTE)second_res + sizeof(IMAGE_RESOURCE_DIRECTORY));
	PIMAGE_RESOURCE_DIRECTORY_ENTRY cur_entry;
	println("数量有{}个", count);
	for (int i = 0; i < count; ++i)
	{
		cur_entry = res_entry + i;
		// 第三层
		println("name is {}", cur_entry->Name);
		PIMAGE_RESOURCE_DIRECTORY  third_res = (PIMAGE_RESOURCE_DIRECTORY)
			((PBYTE)res_directory + cur_entry->OffsetToDirectory);
		PrintThirdInfo(third_res, res_directory);
	}

}

int main()
{
	LPCTSTR filename = "C:/compress/drop/fg.exe";
	PrintResource(filename, -1);
	return 0;
}
