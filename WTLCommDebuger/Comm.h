#pragma once

#include "tmycommAPI.h"
#define UWM_COMMWRITE WM_USER+10
#define UWM_COMMREAD  WM_USER+11
#define UWM_COMMERROR WM_USER+12
#define UWM_COMMSIGNAL WM_USER+13

typedef TMyComm<ThreadRd,ThreadWrt> CThreadComm;

class CMyComm
{
public:
	CThreadComm* operator->()
	{
		return &m_Comm;
	}

	static CMyComm &Instance(){		// Singleton Mode
		static CMyComm MyComm;
		return MyComm;
	}

	static HWND m_hwndListenWrite;
	static HWND m_hwndListenRead;
	static HWND m_hwndListenError;
	static HWND m_hwndListenSignal;

private:
	
	CMyComm(){
		m_Comm.OnError = _Error;
		m_Comm.OnRead = _Read;
		m_Comm.OnWritten = _Write;
		m_Comm.OnSignal = _Signal;
	}
	CMyComm &operator=(const CMyComm&);
	CMyComm(const CMyComm &);

	static void _Write(HANDLE Handle, void* Buf, DWORD Len)
	{
		if(::IsWindow(m_hwndListenWrite)) SendMessage(m_hwndListenWrite,UWM_COMMWRITE,(WPARAM)Buf,(LPARAM)Len);
	}
	static void _Read(HANDLE Handle, void* Buf, DWORD Len)
	{
		if(::IsWindow(m_hwndListenRead)) SendMessage(m_hwndListenRead,UWM_COMMREAD,(WPARAM)Buf,(LPARAM)Len);
	}
	static void _Error(HANDLE Handle, DWORD ErrorCode)
	{
		if(::IsWindow(m_hwndListenError)) SendMessage(m_hwndListenError,UWM_COMMERROR,0,(LPARAM)ErrorCode);
	}
	static void _Signal(HANDLE Handle, DWORD EvtMask)
	{
		if(::IsWindow(m_hwndListenSignal)) SendMessage(m_hwndListenSignal,UWM_COMMSIGNAL,0,(LPARAM)EvtMask);
	}

	CThreadComm m_Comm;
};