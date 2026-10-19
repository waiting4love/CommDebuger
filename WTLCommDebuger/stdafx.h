// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#include <atlbase.h>

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlctrls.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlctrlx.h>
#include <atlsplit.h>
#include <atlframe.h> 

#include "Comm.h"

/***************************************************
 * Name: GetStringFromID
 * Desc: Load string by resource ID
 * Return: string pointer; LPCTSTR GetStringFromID(ID)
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
 * Name: CFixedGroup
 * Desc: Subclass GroupBox to fix flicker with WS_CLIPCHILDREN
 * Return: Use CFixedGroup to subclass GroupBox
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

		HBRUSH br = (HBRUSH)::GetClassLongPtr(::GetParent(m_hWnd), GCLP_HBRBACKGROUND);
		if(br == NULL) br = (HBRUSH)::GetSysColorBrush(COLOR_3DFACE);
		CClientDC DC(m_hWnd);
		DC.FillRect(&rect,br);
		// Instruct Windows to paint the group box text and frame.
		Invalidate(FALSE);

		return TRUE; // Background has been erased.
	}

};