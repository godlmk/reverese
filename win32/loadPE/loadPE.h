#pragma once

#include "resource.h"

void __cdecl OutputDebugStringF(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcPE(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
// ��ʼ��������
void InitProcessListView(HWND hDlg);
// ��������
void EnumProcess(HWND hListProcess);
// ��ʼ��ģ����
void InitModulesListView(HWND hDlg);
// �������̵�ģ��
void EnumModules(HWND hListProcess, HWND hListModules);
// ��ʾ���̵�����ģ��

void ListProcessModules(DWORD dwPid, HWND hListModules);
// ��ʾ�ļ�ѡ��Ի��򲢻�ȡ�ļ�·��
BOOL OpenFileDialog(HWND hwnd, LPTSTR filePath, DWORD filePathSize);
