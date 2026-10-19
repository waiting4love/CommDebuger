// WTLCommDebuger.cpp : main source file for WTLCommDebuger.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainDlg.h"

CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	ATLASSERT(SUCCEEDED(hRes));

	// High DPI awareness: declare Per-Monitor V2 awareness at runtime.
	// This is a fallback in case the manifest is not applied (e.g. older OS).
	// The manifest entry takes precedence on Windows 10 1703+.
	{
		HMODULE hUser32 = ::GetModuleHandle(_T("user32.dll"));
		if (hUser32 != NULL)
		{
			typedef BOOL (WINAPI *PFN_SetProcessDpiAwarenessContext)(HANDLE);
			PFN_SetProcessDpiAwarenessContext pfn =
				(PFN_SetProcessDpiAwarenessContext)::GetProcAddress(hUser32, "SetProcessDpiAwarenessContext");
			if (pfn != NULL)
			{
				// DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 = ((DPI_CONTEXT_HANDLE)-4)
				pfn((HANDLE)-4);
			}
			else
			{
				typedef BOOL (WINAPI *PFN_SetProcessDpiAwareness)(int);
				PFN_SetProcessDpiAwareness pfn2 =
					(PFN_SetProcessDpiAwareness)::GetProcAddress(hUser32, "SetProcessDpiAwareness");
				if (pfn2 != NULL)
				{
					// PROCESS_PER_MONITOR_DPI_AWARENESS = 2
					pfn2(2);
				}
			}
		}
	}

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES |
		ICC_TAB_CLASSES | ICC_UPDOWN_CLASS | ICC_PROGRESS_CLASS | ICC_HOTKEY_CLASS |
		ICC_ANIMATE_CLASS | ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES |
		ICC_COOL_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS |
		ICC_NATIVEFNTCTL_CLASS | ICC_LINK_CLASS);	// full Common Controls v6 support

	HRESULT hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = 0;
	// BLOCK: Run application
	{
		CMessageLoop ml;

		_Module.AddMessageLoop(&ml);

		CMainDlg dlgMain;
		dlgMain.Create(NULL);

		dlgMain.ShowWindow(nCmdShow);
		//nRet = dlgMain.DoModal();
		nRet = ml.Run();

		_Module.RemoveMessageLoop();

	}

	_Module.Term();

	return nRet;
}
