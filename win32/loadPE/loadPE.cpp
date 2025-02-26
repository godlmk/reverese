// loadPE.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "loadPE.h"

#include <stdio.h>
#include <commctrl.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <commdlg.h>
// 全局变量:
HINSTANCE hInst;                                // 当前实例


//BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG  
#define DbgPrintfA   OutputDebugStringF  
#define DbgPrintfW   OutputDebugStringFW
#else  
#define DbgPrintfA
#define DbgPrintfW
#endif 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInst = hInstance;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProcMain);
	return 0;
}

BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HICON  hSmallIcon;
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		//MessageBox(NULL, TEXT("WM_INITDIALOG"), TEXT("INIT"), MB_OK);
		// 获取图标
		hSmallIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_SMALL));
		// 设置图标
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (DWORD)hSmallIcon);
		InitProcessListView(hwndDlg);
		InitModulesListView(hwndDlg);
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hwndDlg, 0);
		return TRUE;
	}
	case WM_NOTIFY:
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (wParam == IDC_LIST_PROCESS && pNMHDR->code == NM_CLICK)
		{
			EnumModules(GetDlgItem(hwndDlg, IDC_LIST_PROCESS),
				GetDlgItem(hwndDlg, IDC_LIST_MOUDLE));
		}
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case   IDC_BUTTON_ABOUT:
		{
			return TRUE;
		}
		case   IDC_BUTTON_OPEN:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PE), hwndDlg, DialogProcPE);
			return TRUE;
		}
		case   IDC_BUTTON_QUIT:
		{
			EndDialog(hwndDlg, 0);

			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}

BOOL CALLBACK DialogProcPE(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		TCHAR filePath[MAX_PATH];
		if (OpenFileDialog(hwndDlg, filePath, MAX_PATH))
		{
			MessageBox(hwndDlg, filePath, TEXT("Select File"), MB_OK);
		}
		return false;
	}
	case WM_CLOSE:
	{
		EndDialog(hwndDlg, 0);
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case   IDC_BUTTON_QUIT:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		}
		break;
	}
	}
	return FALSE;
}

void InitProcessListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	// 初始化
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取process句柄
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	// 设置整行选中
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	// 第一列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("进程"); // 列标题
	lv.cx = 200;
	lv.iSubItem = 0;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	// 第二列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("PID"); // 列标题
	lv.cx = 200;
	lv.iSubItem = 1;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
	// 第三列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("镜像基址"); // 列标题
	lv.cx = 200;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 2, (DWORD)&lv);
	// 第四列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("镜像大小"); // 列标题
	lv.cx = 200;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 3, (DWORD)&lv);
	EnumProcess(hListProcess);
}

void EnumProcess(HWND hListProcess)
{
	LV_ITEM lvitem;
	HANDLE hProcessSnap;
	HANDLE hModuleSnap;
	PROCESSENTRY32 pe32;
	MODULEENTRY32 me32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(hListProcess, TEXT("CreateToolhelp32Snapshot failed."), TEXT("ERROR"), MB_OK);
		return;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return;
	}
	do {
		// 创建模块快照
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		me32.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hModuleSnap, &me32)) {
			CloseHandle(hModuleSnap);
			continue;
		}
		// 进程名
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = ListView_GetItemCount(hListProcess);
		lvitem.iSubItem = 0;
		lvitem.pszText = pe32.szExeFile;
		ListView_InsertItem(hListProcess, &lvitem);
		// pid
		lvitem.iSubItem = 1;
		TCHAR szPID[0x32];
		_stprintf_s(szPID, 0x32, TEXT("%u"), pe32.th32ProcessID);
		lvitem.pszText = szPID;
		ListView_SetItem(hListProcess, &lvitem);
		// imagebase
		lvitem.iSubItem = 2;
		TCHAR szBaseAddr[0x32];
		_stprintf_s(szBaseAddr, 0x32, TEXT("0x%08X"), (DWORD)me32.modBaseAddr);
		lvitem.pszText = szBaseAddr;
		ListView_SetItem(hListProcess, &lvitem);

		// imagesize
		lvitem.iSubItem = 3;
		TCHAR szBaseSize[0x32];
		_stprintf_s(szBaseSize, 0x32, TEXT("%u"), me32.modBaseSize);
		lvitem.pszText = szBaseSize;
		ListView_SetItem(hListProcess, &lvitem);
		// 关闭模块快照
		CloseHandle(hModuleSnap);
	} while (Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
}

void InitModulesListView(HWND hDlg)
{
	LV_COLUMN lv = { 0 };
	HWND hListModules = GetDlgItem(hDlg, IDC_LIST_MOUDLE);

	// 设置整行选中
	SendMessage(hListModules, LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	// 第一列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("模块名称");
	lv.cx = 400;
	lv.iSubItem = 0;
	ListView_InsertColumn(hListModules, 0, &lv);

	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPWSTR)TEXT("模块位置");
	lv.cx = 400;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListModules, 1, &lv);
}

void EnumModules(HWND hListProcess, HWND hListModules)
{
	// 获取选择的行下标
	DWORD dwRowId = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (dwRowId == -1)
	{
		MessageBox(NULL, TEXT("请选择进程"), TEXT("出错了"), MB_OK);
		return;
	}
	// 获取选择的行的信息
	LV_ITEM item = { 0 };
	TCHAR szPid[0x20] = { 0 };
	item.iSubItem = 1;
	item.pszText = szPid;
	item.cchTextMax = 0x20;
	SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowId, (DWORD)&item);
	// 拿到了PId
	DWORD dwPid = _ttoi(szPid);
	// 清空原来的信息
	ListView_DeleteAllItems(hListModules);
	// 列出所有的模块
	ListProcessModules(dwPid, hListModules);
}

void ListProcessModules(DWORD dwPid, HWND hListModules)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// 模块快照
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("创建模块快照失败"), TEXT("Error"), MB_OK);
		return;
	}
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		MessageBox(NULL, TEXT("获取快照模块信息失败"), TEXT("Error"), MB_OK);
		CloseHandle(hModuleSnap);
		return;
	}
	// 遍历模块
	do {
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = ListView_GetItemCount(hListModules);
		lvItem.iSubItem = 0;
		lvItem.pszText = me32.szModule;
		ListView_InsertItem(hListModules, &lvItem);

		lvItem.iSubItem = 1;
		TCHAR szBaseAddr[0x20];
		_stprintf_s(szBaseAddr, 0x20, TEXT("0x%016lX"), (DWORD_PTR)me32.modBaseAddr);
		lvItem.pszText = szBaseAddr;
		ListView_SetItem(hListModules, &lvItem);

	} while (Module32Next(hModuleSnap, &me32));
	CloseHandle(hModuleSnap);
}

BOOL OpenFileDialog(HWND hwnd, LPTSTR filePath, DWORD filePathSize)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH] = { 0 };
	// 初始化ofn
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		_tcsncpy_s(filePath, filePathSize, ofn.lpstrFile, _TRUNCATE);
		return TRUE;
	}
	return FALSE;
}

void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

	if (strBuffer == NULL) {
		return; // Handle allocation failure
	}

	va_start(vlArgs, format);
	_vsnprintf_s(strBuffer, 4096, _TRUNCATE, format, vlArgs);
	va_end(vlArgs);
	strcat_s(strBuffer, 4096, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}
void __cdecl OutputDebugStringFW(const wchar_t* format, ...)
{
	va_list vlArgs;
	wchar_t* strBuffer = (wchar_t*)GlobalAlloc(GPTR, 4096 * sizeof(wchar_t));
	if (strBuffer == NULL) { return; }

	va_start(vlArgs, format);
	_vsnwprintf_s(strBuffer, 4096, _TRUNCATE, format, vlArgs);
	va_end(vlArgs);
	wcscat_s(strBuffer, 4096, L"\n");
	OutputDebugStringW(strBuffer);
	return;
}
