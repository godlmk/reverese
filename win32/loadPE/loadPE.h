#pragma once

#include "resource.h"

void __cdecl OutputDebugStringF(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcPE(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcSection(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcDirectory(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
// 初始化进程列
void InitProcessListView(HWND hDlg);
// 遍历进程
void EnumProcess(HWND hListProcess);
// 初始化模块列
void InitModulesListView(HWND hDlg);
// 遍历进程的模块
void EnumModules(HWND hListProcess, HWND hListModules);
// 显示进程的所有模块

void ListProcessModules(DWORD dwPid, HWND hListModules);
// 显示文件选择对话框并获取文件路径
BOOL OpenFileDialog(HWND hwnd, LPTSTR filePath, DWORD filePathSize);
// 根据文件名初始化PE查看器数据
void InitPEView(HWND hDlg);

// 初始化节信息
void InitSectionView(HWND hDlg);
// 填充节的相关信息
void PopulateSectionView(HWND hDlg);
// 填充目录项相关信息
void InitDirectoryView(HWND hDlg);

// 初始化目录的详细信息
BOOL CALLBACK DialogProcExport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcImport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcResource(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcRelocation(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcBoundImport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
