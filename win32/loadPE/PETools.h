#pragma once
#define _CRT_SECURE_NO_WARNINGS
// importTable.h: ��׼ϵͳ�����ļ��İ����ļ�
// ����Ŀ�ض��İ����ļ���

#include <windows.h>
#include <string>

int Align(int origin, int alignment);
PIMAGE_DOS_HEADER GetDosHeader(LPVOID pImageBuffer);
PIMAGE_NT_HEADERS GetNTHeader(LPVOID pImageBuffer, PIMAGE_DOS_HEADER dosHeader);
DWORD ReadFileBuffer(LPCTSTR filename, void** pBuffer);
PBYTE ReadMemoryImage(LPCTSTR filename);
bool ImageMemory2File(PBYTE pMemBuffer, const wchar_t* destPath);
bool ImageMemory2File(PBYTE pMemBuffer, const char* destPath);
DWORD RVA2FOA(IN LPVOID pMemoryBuffer, IN DWORD Rva);
DWORD FOA2RVA(IN LPVOID pMemoryBuffer, IN DWORD Foa);
bool write_file(IN void* buffer, IN const char* filename, IN const DWORD size);
DWORD Offset(PVOID buffer, PVOID addr);
void* GetBufferAddr(PVOID buffer, DWORD rva);
bool AddNewSection(IN  LPCTSTR infilename, int& newFOA);
bool BigerSection(IN LPCTSTR infilename, IN const char* outfilename);

std::string RelocatedTable(PVOID pFileBuffer);

std::string ExportTable(PVOID fileImage);

std::string ImportTable(PVOID file_buffer);

std::string BoundImportTable(PVOID file_buffer);

std::string ResourceTable(PVOID file_buffer);

