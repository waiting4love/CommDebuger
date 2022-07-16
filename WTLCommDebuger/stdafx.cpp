// stdafx.cpp : source file that includes just the standard includes
//	WTLCommDebuger.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

HWND CMyComm::m_hwndListenWrite=NULL;
HWND CMyComm::m_hwndListenRead=NULL;
HWND CMyComm::m_hwndListenError=NULL;
HWND CMyComm::m_hwndListenSignal=NULL;
