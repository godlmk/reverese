#pragma once

#include "resource.h"

void __cdecl OutputDebugStringF(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcPE(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcSection(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcDirectory(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
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
// �����ļ�����ʼ��PE�鿴������
void InitPEView(HWND hDlg);

// ��ʼ������Ϣ
void InitSectionView(HWND hDlg);
// ���ڵ������Ϣ
void PopulateSectionView(HWND hDlg);
// ���Ŀ¼�������Ϣ
void InitDirectoryView(HWND hDlg);

// ��ʼ��Ŀ¼����ϸ��Ϣ
BOOL CALLBACK DialogProcExport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcImport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcResource(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcRelocation(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProcBoundImport(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
