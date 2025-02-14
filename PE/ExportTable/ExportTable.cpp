// ExportTable.cpp: 定义应用程序的入口点。
//

#include "ExportTable.h"
#include <winnt.h>
void PrintAllExportFuncName(IN const char* filename)
{
	/*
	找到导出表
	找到名字表
	打印所有名字
	*/
	PVOID fileImage = ReadFileBuffer(filename);
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(fileImage);
	if (dosHeader == NULL)
	{
		std::println("dosHeader is invalid");
		return;
	}
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(fileImage, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 导出表是第一张表
	IMAGE_DATA_DIRECTORY exportDataDict = opHeader->DataDirectory[0];
	// 导出表的RVA
	DWORD exportTable_rva = exportDataDict.VirtualAddress;
	// 导出表的FOA
	DWORD exportTable_foa = RVA2FOA(fileImage, exportTable_rva);
	// 导出表的指针
	PIMAGE_EXPORT_DIRECTORY pExporttable = (PIMAGE_EXPORT_DIRECTORY)((DWORD)fileImage + exportTable_foa);
	// 打印一些信息
	DWORD name_foa = RVA2FOA(fileImage, pExporttable->Name);
	const char* name = (const char*)((DWORD)fileImage + name_foa);
	std::println("Name:{}\nBase:{}\nNumberOfFunctions:{}\nNumberOfNames:{}\n------------",
		name, pExporttable->Base, pExporttable->NumberOfFunctions, pExporttable->NumberOfNames);

	//导出表的函数名字表的RVA
	DWORD nameTable_rva = pExporttable->AddressOfNames;
	// 导出表的函数名字表的FOA
	DWORD nameTable_foa = RVA2FOA(fileImage, nameTable_rva);
	// 函数名字表的指针
	PDWORD pNameTable = (PDWORD)((DWORD)fileImage + nameTable_foa);

	// ordinal表的rva
	DWORD ordinal_table_rva = pExporttable->AddressOfNameOrdinals;
	// ordinal表的foa
	DWORD ordinal_table_foa = RVA2FOA(fileImage, ordinal_table_rva);
	// ordinal表
	PWORD ordinal_table = (PWORD)((DWORD)fileImage + ordinal_table_foa);
	// 打印ordinals表和names表
	std::println("functions' name and ordinals");
	for (DWORD i = 0; i < pExporttable->NumberOfNames; ++i)
	{
		// 函数名的rva
		DWORD funcName_rva = pNameTable[i];
		// 函数名字的foa
		DWORD funcName_foa = RVA2FOA(fileImage, funcName_rva);
		//打印函数名
		const char* funcName = (const char*)((DWORD)fileImage + funcName_foa);
		std::println("ordinal为{}的函数名为{}", (ordinal_table[i]), funcName);
	}
	DWORD func_table_rva = pExporttable->AddressOfFunctions;
	DWORD func_table_foa = RVA2FOA(fileImage, func_table_rva);
	PDWORD func_table = (PDWORD)((DWORD)fileImage + func_table_foa);
	std::println("functions' address ");
	for (DWORD i = 0; i < pExporttable->NumberOfFunctions; ++i)
	{
		std::println("下表为{}的函数的rva是{:X}", i, func_table[i]);
	}
}

int main()
{
	const char* filename = "C:/reverse/PE/defExport/Debug/defExport.dll";
	PrintAllExportFuncName(filename);
	const char* funcname = "Mul";
	void* pFileBuffer = ReadFileBuffer(filename);
	void* mulAddress = GetFunctionAddrByName(pFileBuffer, (PSTR)funcname);
	//std::println("muladdress is {:X}", (DWORD)mulAddress);
	//std::println("ordianl 2 func is {:X}", (DWORD)GetFunctionAddrByOrdinals(pFileBuffer, 13));
	return 0;
}

PVOID GetFunctionAddrByName(PVOID pFileBuffer, IN PSTR funcname)
{
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pFileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(pFileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 导出表的RVA
	DWORD export_rva = opHeader->DataDirectory[0].VirtualAddress;
	//导出表的FOA
	DWORD export_foa = RVA2FOA(pFileBuffer, export_rva);
	// 导出表的指针
	PIMAGE_EXPORT_DIRECTORY export_table = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + export_foa);

	// 导出表的函数名表的rva
	DWORD func_name_table_rva = export_table->AddressOfNames;
	//导出表的函数名表的foa
	DWORD func_name_table_foa = RVA2FOA(pFileBuffer, func_name_table_rva);
	// 导出表的函数名表的指针
	PDWORD func_name_table = (PDWORD)((DWORD)pFileBuffer + func_name_table_foa);
	DWORD index = 0;
	for (; index < export_table->NumberOfNames; ++index)
	{
		// 函数名的rva
		DWORD func_name_rva = func_name_table[index];
		// 函数名的foa
		DWORD func_name_foa = RVA2FOA(pFileBuffer, func_name_rva);
		// 函数名
		PSTR func_name = (PSTR)((DWORD)pFileBuffer + func_name_foa);
		if (0 == strcmp(func_name, funcname))
		{
			break;
		}
	}
	if (index == export_table->NumberOfNames)
	{
		std::println("函数名表中没有该文件名");
		return PVOID();
	}
	// ordinalTablerva
	DWORD ordinal_table_rva = export_table->AddressOfNameOrdinals;
	// ordinalTable foa
	DWORD ordinal_table_foa = RVA2FOA(pFileBuffer, ordinal_table_rva);
	// ordinalTable
	PWORD oridinal_table = (PWORD)((DWORD)pFileBuffer + ordinal_table_foa);

	WORD index_func_table = *(oridinal_table + index);
	// 函数表rva
	DWORD func_table_rva = export_table->AddressOfFunctions;
	// 函数表foa
	DWORD func_table_foa = RVA2FOA(pFileBuffer, func_table_rva);
	// 函数表
	PDWORD func_table = (PDWORD)((DWORD)pFileBuffer + func_table_foa);
	return (PVOID)(func_table[index_func_table]);
}

PVOID GetFunctionAddrByOrdinals(PVOID pFileBuffer, IN int ordinal)
{
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pFileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(pFileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// 导出表的RVA
	DWORD export_rva = opHeader->DataDirectory[0].VirtualAddress;
	//导出表的FOA
	DWORD export_foa = RVA2FOA(pFileBuffer, export_rva);
	// 导出表的指针
	PIMAGE_EXPORT_DIRECTORY export_table = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + export_foa);

	// 导出表函数表rva
	DWORD func_table_rva = export_table->AddressOfFunctions;
	// 导出表函数表foa
	DWORD func_table_foa = RVA2FOA(pFileBuffer, func_table_rva);
	// 导出表指针
	PDWORD func_table = (PDWORD)((DWORD)pFileBuffer + func_table_foa);
	return (PVOID)func_table[ordinal - export_table->Base];
}
