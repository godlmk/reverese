// ExportTableMove.cpp: 定义应用程序的入口点。
//

#include "ExportTableMove.h"

bool ExportTableMove(IN const char* filename)
{
	/*
		新增节
	*/
	int foa = 0;
	auto addSuccess = AddNewSection(filename, foa);
	if (addSuccess == false)
	{
		std::println("新增节失败");
		return addSuccess;
	}
	// 找到导出表的
	void* fileBuffer;
	DWORD size = ReadFileBuffer(filename, &fileBuffer);
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(fileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(fileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	//exportTable的rva
	DWORD export_table_rva = opHeader->DataDirectory[0].VirtualAddress;
	// export table的foa
	DWORD export_table_foa = RVA2FOA(fileBuffer, export_table_rva);
	// export table的指针
	PIMAGE_EXPORT_DIRECTORY export_table = (PIMAGE_EXPORT_DIRECTORY)((DWORD)fileBuffer + export_table_foa);

	// function 的rva
	DWORD functionaddr_table_rva = export_table->AddressOfFunctions;
	// function 的foa
	DWORD functionaddr_table_foa = RVA2FOA(fileBuffer, functionaddr_table_rva);
	// function table指针
	PDWORD functionaddr_table = (PDWORD)((DWORD)fileBuffer + functionaddr_table_foa);

	// function  name rva
	DWORD functionname_table_rva = export_table->AddressOfNames;
	// foa
	DWORD functionname_table_foa = RVA2FOA(fileBuffer, functionname_table_rva);
	//function name指针
	PDWORD functionname_table = (PDWORD)((DWORD)fileBuffer + functionname_table_foa);

	// function ordinals rva
	DWORD functionordinals_table_rva = export_table->AddressOfNameOrdinals;
	//foa
	DWORD functionordinals_table_foa = RVA2FOA(fileBuffer, functionordinals_table_rva);
	// function ordinals 
	PWORD functionordinals_table = (PWORD)((DWORD)fileBuffer + functionordinals_table_foa);

	// 拷贝AddresOfFunctions
	PDWORD functionaddr_dest = (DWORD*)((DWORD)fileBuffer + foa);
	memcpy(functionaddr_dest, functionaddr_table, 4 * export_table->NumberOfFunctions);
	// 拷贝AddressOfOrdinals
	PWORD functionordinals_dest = (PWORD)((DWORD)functionaddr_dest + 4 * export_table->NumberOfFunctions);
	memcpy(functionordinals_dest, functionordinals_table, 2 * export_table->NumberOfNames);
	// 拷贝AddressOfNames
	PDWORD functionname_dest = (PDWORD)((DWORD)functionordinals_dest + 2 * export_table->NumberOfNames);
	memcpy(functionname_dest, functionname_table, 4 * export_table->NumberOfNames);
	// 复制所有的函数名
	char* firstNameAddr = (char*)((DWORD)functionname_dest + 4 * export_table->NumberOfNames), * curNameAddr = firstNameAddr;
	for (DWORD i = 0; i < export_table->NumberOfNames; ++i)
	{
		DWORD functionname_rva = functionname_table[i];
		DWORD functionname_foa = RVA2FOA(fileBuffer, functionname_rva);
		const char* pFunctionName = (const char*)((DWORD)fileBuffer + functionname_foa);
		// 长度,加上最后的0的长度
		size_t length = strlen(pFunctionName) + 1;
		// strncpy该函数会在后面加上0
		strncpy(curNameAddr, pFunctionName, length);
		std::println("curname is {}", curNameAddr);
		// 修改新的function name的指向，使其指向新地址的foa
		functionname_dest[i] = FOA2RVA(fileBuffer, Offset(fileBuffer, curNameAddr));
		curNameAddr = (char*)((DWORD)curNameAddr + length);
	}
	// 复制export_table_directory
	PIMAGE_EXPORT_DIRECTORY new_export_table = (PIMAGE_EXPORT_DIRECTORY)(curNameAddr);
	memcpy(new_export_table, export_table, sizeof(IMAGE_EXPORT_DIRECTORY));
	// 修复新表里面的三个值
	new_export_table->AddressOfFunctions = FOA2RVA(fileBuffer, Offset(fileBuffer, functionaddr_dest));
	new_export_table->AddressOfNameOrdinals = FOA2RVA(fileBuffer, Offset(fileBuffer, functionordinals_dest));
	new_export_table->AddressOfNames = FOA2RVA(fileBuffer, Offset(fileBuffer, functionname_dest));
	// 修改新表的地址
	opHeader->DataDirectory[0].VirtualAddress = FOA2RVA(fileBuffer, Offset(fileBuffer, new_export_table));
	// 写回到文件
	write_file(fileBuffer, filename, size);
	return true;
}

int main()
{
	const char* filename = "C:/reverse/PE/defExport/Debug/defExport.dll";
	ExportTableMove(filename);
	return 0;
}
