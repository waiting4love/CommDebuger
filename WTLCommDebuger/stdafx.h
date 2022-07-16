// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#include <atlbase.h>

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <atlctrls.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlctrlx.h>
#include <atlsplit.h>
#include <atlframe.h> 

#include "Comm.h"

/***************************************************
* 类: GetStringFromID
* 作用: 从资源ID中获取字符串
* 用法: 把它当作函数 LPCTSTR GetStringFromID(ID)
****************************************************/
struct GetStringFromID{    
	TCHAR Buf[256];
	
	GetStringFromID(UINT uID){
		::ZeroMemory(Buf,sizeof(Buf));
		::LoadString(_Module.GetResourceInstance(),uID,Buf,256);
	}

	operator LPCTSTR(){
		return Buf;
	}

};

/***************************************************
* 类: CFixedGroup
* 作用: 解决GroupBox的父窗口指定为WS_CLIPCHILDREN时的自画问题
* 用法: 将目标GroupBox子类化成CFixedGroup
****************************************************/
class CFixedGroup
	: public  CWindowImpl<CFixedGroup,CButton,CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX | BS_TEXT> >
{
	typedef CWindowImpl<CFixedGroup,CButton,CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX | BS_TEXT> > BaseClass;
public:
	DECLARE_WND_CLASS(_T("Button.GroupBox"));

	BEGIN_MSG_MAP_EX(CFixedGroup)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		RECT rect;
		GetClientRect(&rect);

		HBRUSH br = (HBRUSH)::GetClassLong(::GetParent(m_hWnd),GCL_HBRBACKGROUND);
		if(br == NULL) br = (HBRUSH)::GetSysColorBrush(COLOR_3DFACE);
		CClientDC DC(m_hWnd);
		DC.FillRect(&rect,br);
		// Instruct Windows to paint the group box text and frame.
		Invalidate(FALSE);

		return TRUE; // Background has been erased.
	}

};