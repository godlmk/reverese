// importTable.cpp: 定义应用程序的入口点。
//

#include "importTable.h"

using namespace std;

bool PrintImportInfo(IN const char* filename)
{
	/*
	找到导入表
	找到各个dll
	打印各个成员
	*/
	// 导入表的rva

	void* file_buffer;
	DWORD size = ReadFileBuffer(filename, &file_buffer);
	if (size == 0)
	{
		return false;
	}
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_headers = &nt_headers->OptionalHeader;

	DWORD import_table_rva = op_headers->DataDirectory[1].VirtualAddress;
	DWORD import_table_foa = RVA2FOA(file_buffer, import_table_rva);
	PIMAGE_IMPORT_DESCRIPTOR first_import_table = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)file_buffer + import_table_foa);
	PIMAGE_IMPORT_DESCRIPTOR cur_import_table = first_import_table;

	while (cur_import_table->Name != 0)
	{
		// 输出dll名字
		DWORD dll_name_rva = cur_import_table->Name;
		DWORD dll_name_foa = RVA2FOA(file_buffer, dll_name_rva);
		const char* dll_name = (PCSTR)((PBYTE)file_buffer + dll_name_foa);
		std::println("------ now dll name is {} -------", dll_name);
		// 没有绑定时的情况
		//遍历IAT
		PDWORD cur_INT = (PDWORD)GetBufferAddr(file_buffer, cur_import_table->OriginalFirstThunk);
		constexpr DWORD first_1 = 1 << 31, last_31 = first_1 - 1;
		std::println("----INT 表如下----");
		while (*cur_INT != 0)
		{
			DWORD value = *cur_INT;
			if ((value & first_1) == first_1)
			{
				// 导出序号
				println("INT表中该函数序号为{:X}", value & last_31);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME import_name = (PIMAGE_IMPORT_BY_NAME)GetBufferAddr(file_buffer, value);
				PCSTR name = (PCSTR)import_name->Name;
				println("INT表中该函数名为hit/name:0x{:X}---{}", import_name->Hint, name);
			}
			cur_INT++;
		}
		// 遍历FirstThunkkk
		if (cur_import_table->TimeDateStamp == -1)
		{
			println("这是一个已经绑定的IAT表,跳过");
			continue;
		}
		println("----IAT 表如下----");
		PDWORD cur_IAT = (PDWORD)GetBufferAddr(file_buffer, cur_import_table->FirstThunk);
		while (*cur_IAT != 0)
		{
			DWORD value = *cur_IAT;
			if ((value & first_1) == first_1)
			{
				println("IAT表该函数序号为{:X}", value & last_31);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME import_name = (PIMAGE_IMPORT_BY_NAME)GetBufferAddr(file_buffer, value);
				PCSTR name = (PCSTR)import_name->Name;
				println("IAT表中该函数的名字为hit/name:0x{:X}---{}", import_name->Hint, name);
			}
			cur_IAT++;
		}
		cur_import_table++;
	}
	free(file_buffer);
	return true;
}
bool PrintBoundImportTable(const char* filename)
{
	void* file_buffer;
	DWORD size = ReadFileBuffer(filename, &file_buffer);
	if (size == 0)
	{
		return false;
	}
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	if (op_header->DataDirectory[11].VirtualAddress == 0)
	{
		println("该PE文件没有绑定导入表");
		return false;
	}
	PIMAGE_BOUND_IMPORT_DESCRIPTOR first_bound_import_desc = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)GetBufferAddr(file_buffer,
		op_header->DataDirectory[11].VirtualAddress);
	PIMAGE_BOUND_IMPORT_DESCRIPTOR cur_bound_import_desc = first_bound_import_desc;
	if (cur_bound_import_desc == NULL) return false;

	int index = 0;
	while (cur_bound_import_desc->OffsetModuleName != 0 && cur_bound_import_desc->TimeDateStamp != 0)
	{
		// 打印名字和时间戳
		index = 0;
		PCSTR dll_name = (PCSTR)((PBYTE)first_bound_import_desc + cur_bound_import_desc->OffsetModuleName);
		println("---该dll的时间戳为{:X}, 函数名为{}", cur_bound_import_desc->TimeDateStamp, dll_name);

		// 打印依赖的名字和时间戳
		if (cur_bound_import_desc->NumberOfModuleForwarderRefs > 0)
		{
			println("该dll的依赖dll有{}个，如下：", cur_bound_import_desc->NumberOfModuleForwarderRefs);
			for (int i = 0; i < cur_bound_import_desc->NumberOfModuleForwarderRefs; ++i)
			{
				++index;
				PIMAGE_BOUND_FORWARDER_REF bound_forwarder = (PIMAGE_BOUND_FORWARDER_REF)(cur_bound_import_desc + 1 + i);
				dll_name = (PCSTR)((PBYTE)first_bound_import_desc + bound_forwarder->OffsetModuleName);
				println("依赖{}---该dll的时间戳为{:X}, 函数名为{}", i + 1, bound_forwarder->TimeDateStamp, dll_name);
			}
		}
		else
		{
			println("该dll没有依赖的dll");
		}

		++index;
		// 移动到下一个绑定导入描述符
		cur_bound_import_desc += index;
	}
	return true;
}
bool ImportTableInject(IN PCSTR filename, IN PCSTR dllname, IN PCSTR func_name)
{
	/*
		找到导入表的大小
		判断最后一个节的空闲空间，不够的话增加节大小
		新建函数名表
		新建INT和IAT表
		移动导入表
		修改导入表使其指向新的INT和IAT
		修改DataEntry的值使其指向新的导入表
	*/
	void* file_buffer;
	DWORD buffer_size = ReadFileBuffer(filename, &file_buffer);
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	PIMAGE_IMPORT_DESCRIPTOR first_import_desc = (PIMAGE_IMPORT_DESCRIPTOR)GetBufferAddr(file_buffer,
		op_header->DataDirectory[1].VirtualAddress);
	PIMAGE_IMPORT_DESCRIPTOR cur_import_desc = first_import_desc;
	while (cur_import_desc->FirstThunk != 0 && cur_import_desc->OriginalFirstThunk != 0)
	{
		++cur_import_desc;
	}
	DWORD need_size{ 0 };
	// 原来表占用的空间，不包含最后的0
	DWORD origin_size = (cur_import_desc - first_import_desc) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
	// 包含0，和新增一个表的空间
	need_size += origin_size + 2 * sizeof(IMAGE_IMPORT_DESCRIPTOR);
	// dll长度 + 1
	need_size += strlen(dllname) + 1;
	// INT 0  IAT 0
	need_size += (8 + 8);
	// 函数名长度 + 1 + 2
	need_size += strlen(func_name) + 1 + 2;
	// 遍历节表找到空闲区域
	PIMAGE_SECTION_HEADER first_section_header = IMAGE_FIRST_SECTION(nt_headers);
	int section_count = nt_headers->FileHeader.NumberOfSections;
	int need_index = -1;
	for (int i = 0; i < section_count; ++i)
	{
		DWORD free = 0;
		if (i != section_count - 1)
		{
			free = (first_section_header + i + 1)->VirtualAddress - (first_section_header + i)->Misc.VirtualSize;
		}
		else
		{
			free = Align((first_section_header + i)->SizeOfRawData, op_header->SectionAlignment);
		}
		if (free >= need_size)
		{
			need_index = i;
			break;
		}
	}
	if (need_index == -1)
	{
		println("no more free space");
		return false;
	}
	PIMAGE_SECTION_HEADER cur_section = first_section_header + need_index;
	PVOID begin_addr = (PVOID)((DWORD)file_buffer + cur_section->PointerToRawData +
		RVA2FOA(file_buffer, cur_section->Misc.VirtualSize));
	// BY_NAME
	size_t func_name_len = strlen(func_name) + 1;
	PIMAGE_IMPORT_BY_NAME import_by_name_addr = (PIMAGE_IMPORT_BY_NAME)((PBYTE)begin_addr);
	import_by_name_addr->Hint = 0;
	strncpy(import_by_name_addr->Name, func_name, func_name_len);
	println("func name is {}", import_by_name_addr->Name);
	// INT
	PDWORD p_INT = (PDWORD)((PBYTE)import_by_name_addr +
		sizeof(IMAGE_IMPORT_BY_NAME) - 1 + func_name_len);
	*p_INT = RVA2FOA(file_buffer,
		(DWORD)import_by_name_addr - (DWORD)file_buffer);
	*(p_INT + 1) = 0;
	PDWORD p_IAT = p_INT + 2;
	*p_IAT = RVA2FOA(file_buffer,
		(DWORD)import_by_name_addr - (DWORD)file_buffer);
	*(p_IAT + 1) = 0;
	PSTR new_dll_name = (PSTR)(p_IAT + 2);
	size_t dll_len = strlen(dllname) + 1;
	strncpy(new_dll_name, dllname, dll_len);
	// 新的导入表
	PIMAGE_IMPORT_DESCRIPTOR new_import_desc = (PIMAGE_IMPORT_DESCRIPTOR)(new_dll_name + dll_len);
	memcpy(new_import_desc, first_import_desc, origin_size);
	PIMAGE_IMPORT_DESCRIPTOR change_import_desc = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)new_import_desc + origin_size);
	change_import_desc->OriginalFirstThunk = FOA2RVA(file_buffer, (DWORD)p_INT - (DWORD)file_buffer);
	change_import_desc->FirstThunk = FOA2RVA(file_buffer, (DWORD)p_IAT - (DWORD)file_buffer);
	change_import_desc->Name = FOA2RVA(file_buffer, (DWORD)new_dll_name - (DWORD)file_buffer);
	memset(change_import_desc + 1, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));
	// 修复DataEntry

	op_header->DataDirectory[1].VirtualAddress = FOA2RVA(file_buffer,
		(DWORD)new_import_desc - (DWORD)file_buffer);
	op_header->DataDirectory[1].Size = origin_size + sizeof(IMAGE_IMPORT_DESCRIPTOR);
	write_file(file_buffer, filename, buffer_size);
}

int main()
{
	const char* fgfilename = "C:/compress/drop/fg.exe";
	//PrintBoundImportTable(fgfilename);
	//PrintImportInfo(fgfilename);
	PCSTR dllname = "MyDll.dll", funcname = "Add";
	MessageBox(0, "init", "init", 0);
	//ImportTableInject(fgfilename, dllname, funcname);
	return 0;
}
