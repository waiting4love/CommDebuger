#include "stdafx.h"

#include "MainDlg.h"
#include "Comm.h"


CMainDlg::CMainDlg()
{
	m_CommOpts.bLastPos=true; 
	m_CommOpts.bCtrlChar=false; 
	m_CommOpts.bSuperT=false; 
	m_CommOpts.bAutoSave=false; 
	m_CommOpts.dCharCount=10000; 
	lstrcpy(m_CommOpts.szFileName,_T("CommLog.txt"));
	m_vecSerialPort = GetAvailablePorts();
}

BOOL  CMainDlg::PreTranslateMessage (MSG *pMsg)
{
	if(IsDialogMessage(pMsg))
	{
		if(pMsg->hwnd) m_tip.RelayEvent(pMsg);
		return TRUE;
	}
	return FALSE;
}

void SetComboBoxItem(HWND hCtrl,UINT idStr,int idxInit=-1)
{
	CComboBox cb(hCtrl);

	TCHAR Buf[1024];
	::LoadString(_Module.GetResourceInstance(),idStr,Buf,1024);

	TCHAR *p = _tcstok(Buf,_T(","));
	while(p)
	{
		cb.AddString(p);
		p = _tcstok(NULL,_T(","));
	}

	if(idxInit>=0) cb.SetCurSel(idxInit);
}

void SetComboBoxDropedWidth(HWND hCtrl)
{
	// Find the longest string in the combo box.
	CSize      sz;
	int        dx = 0;
	TEXTMETRIC tm;
	CComboBox cb(hCtrl);

	CDCHandle dc = cb.GetDC();
	CFontHandle Font = cb.GetFont();

	// Select the listbox font, save the old font
	CFontHandle OldFont = dc.SelectFont(Font);
	// Get the text metrics for avg char width
	dc.GetTextMetrics(&tm);

	TCHAR str[MAX_PATH];
	for (int i = 0, c = cb.GetCount(); i< c; i++)
	{
		cb.GetLBText(i, str);
		dc.GetTextExtent(str, lstrlen(str), &sz);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	dc.SelectFont(OldFont);
	cb.ReleaseDC(dc);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2 * ::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	cb.SetDroppedWidth(dx);
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	CIcon Icon;
	Icon.LoadIcon(IDR_MAINFRAME);
	SetIcon(Icon);

	// set ctrl data
	m_cbPort = GetDlgItem(IDC_CBB_PORT);
	// SetComboBoxItem(m_cbPort,IDS_PORT,0);
	m_cbPort.SetMinVisible(8);
	for (auto& x : m_vecSerialPort)
	{
		if (StrCmpN(x.portName, _T("COM"), 3) == 0)
		{
			CString s = x.portName + _T(" - ") + x.description;
			int idx = m_cbPort.AddString(s);
			m_cbPort.SetItemDataPtr(idx, &x);
		}
	}
	SetComboBoxDropedWidth(m_cbPort);

	m_cbBaudRate = GetDlgItem(IDC_CBB_BAUDRATE);
	SetComboBoxItem(m_cbBaudRate,IDS_BAUDRATE,8);
	m_cbBaudRate.SetMinVisible(8);

	m_cbByteSize = GetDlgItem(IDC_CBB_BYTESIZE);
	SetComboBoxItem(m_cbByteSize,IDS_BYTESIZE,3);

	m_cbStopBits = GetDlgItem(IDC_CBB_STOPBITS);
	SetComboBoxItem(m_cbStopBits,IDS_STOPBITS,0);

	m_cbParity = GetDlgItem(IDC_CBB_PARITY);
	SetComboBoxItem(m_cbParity,IDS_PARITY,0);

	m_cbFlowCtrl = GetDlgItem(IDC_CBB_FLOWCTRL);
	SetComboBoxItem(m_cbFlowCtrl,IDS_FLOWCTRL,0);

	m_btnOpen = GetDlgItem(IDC_BTN_COMMOPEN);

	m_chkCount = GetDlgItem(IDC_CBX_COUNT);
	m_chkCount.SetCheck(BST_CHECKED);			// Set Init Check

	m_btnCntClear = GetDlgItem(IDC_BTN_COUNTCLEAR);

	m_edtSend = GetDlgItem(IDC_EDT_SEND);
	m_edtSend.SetWindowText(_T("0"));

	m_edtRecv = GetDlgItem(IDC_EDT_RECV);
	m_edtRecv.SetWindowText(_T("0"));

	m_chkDTR = GetDlgItem(IDC_CBX_DTR);
	m_chkRTS = GetDlgItem(IDC_CBX_RTS);
	m_chkBREAK = GetDlgItem(IDC_CBX_BREAK);

	m_stOpened.SubclassWindow(GetDlgItem(IDC_STATIC_OPENED));
	m_stOpened.SetTextColor(0x0000ff);
	m_stOpened.SetBrush(GetSysColorBrush(COLOR_BTNFACE));
	m_stOpened.ShowWindow(SW_HIDE);
	
	m_stCTS.SubclassWindow(GetDlgItem(IDC_STATIC_CTS));
	m_stDSR.SubclassWindow(GetDlgItem(IDC_STATIC_DSR));
	m_stDCD.SubclassWindow(GetDlgItem(IDC_STATIC_DCD));
	m_stRING.SubclassWindow(GetDlgItem(IDC_STATIC_RING));
	m_stCTS.SetBrush(GetSysColorBrush(COLOR_BTNFACE));
	m_stDSR.SetBrush(GetSysColorBrush(COLOR_BTNFACE));
	m_stDCD.SetBrush(GetSysColorBrush(COLOR_BTNFACE));
	m_stRING.SetBrush(GetSysColorBrush(COLOR_BTNFACE));

	// fix groupbox
	m_gpInit.SubclassWindow(GetDlgItem(IDC_GPBINIT));
	m_gpCount.SubclassWindow(GetDlgItem(IDC_GPBCOUNT));
	m_gpState.SubclassWindow(GetDlgItem(IDC_GPBSTATE));

	// Splitter
	RECT rcClient;
	::GetWindowRect(GetDlgItem(IDC_GROUP_DATA),&rcClient);
	::InflateRect(&rcClient,-2,-2);
	::MapWindowPoints(NULL,m_hWnd,(LPPOINT)&rcClient,2);
	m_Splitter.Create(
		m_hWnd,
		rcClient,//CRect(2,2,100,100),
		NULL,
		0,
		WS_EX_CONTROLPARENT,
		ID_SPLITTER
		);
	::BringWindowToTop(m_Splitter);

	m_pnlSend.Create(m_Splitter);
	m_pnlRecv.Create(m_Splitter);
	m_Splitter.SetSplitterPanes(m_pnlSend,m_pnlRecv);

	m_Splitter.SetSplitterPos();

	// ToolTips
	m_tip.Create(m_hWnd);//,NULL,NULL,WS_POPUP | TTS_NOPREFIX | TTS_BALLOON);
	m_tip.Activate(TRUE);
	m_tip.SetTipBkColor(0x00F0CAA6);
	m_tip.SetMaxTipWidth(500);   // Support Multi-Line

	m_tip.AddTool(GetDlgItem(IDC_CBX_DTR),IDC_CBX_DTR);
	m_tip.AddTool(GetDlgItem(IDC_CBX_RTS),IDC_CBX_RTS);
	m_tip.AddTool(GetDlgItem(IDC_CBX_BREAK),IDC_CBX_BREAK);
	m_tip.AddTool(GetDlgItem(IDC_STATIC_CTS),IDC_STATIC_CTS);
	m_tip.AddTool(GetDlgItem(IDC_STATIC_DSR),IDC_STATIC_DSR);
	m_tip.AddTool(GetDlgItem(IDC_STATIC_DCD),IDC_STATIC_DCD);
	m_tip.AddTool(GetDlgItem(IDC_STATIC_RING),IDC_STATIC_RING);
	
	
	// Assign Comm Event
	CMyComm::m_hwndListenWrite = m_hWnd;
	CMyComm::m_hwndListenRead = m_hWnd;
	CMyComm::m_hwndListenSignal = m_hWnd;

    // Add MessageFilter
	CMessageLoop *pml=_Module.GetMessageLoop();
    pml->AddMessageFilter(this);

	DlgResize_Init(true);
	//ModifyStyle(WS_CLIPCHILDREN,0);

	return TRUE;
}

LRESULT CMainDlg::OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnClose();
	return 0;
}

void CMainDlg::OnClose()
{
	if(m_CommOpts.bAutoSave && m_CommOpts.szFileName[0]>='0')
	{
		DWORD len = ::GetWindowTextLengthA(m_pnlRecv.m_edtText);
		if(len > 0)
		{
			HANDLE fh = ::CreateFile( m_CommOpts.szFileName,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if(fh != INVALID_HANDLE_VALUE)
			{	
				char * data = new char[len+1];
				::GetWindowTextA(m_pnlRecv.m_edtText,data,len+1);
				::SetFilePointer(fh,0,NULL,FILE_END);			// Append
				::WriteFile(fh,data,len ,&len,NULL);
				delete []data;
				::CloseHandle(fh);
			}
		}
	}

	DestroyWindow();
	::PostQuitMessage(0);

}

LRESULT CMainDlg::OnCommOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		CMyComm::Instance()->Close();
		m_stOpened.ShowWindow(SW_HIDE);
		m_btnOpen.SetWindowText(GetStringFromID(IDS_OPENBTN));
		m_cbPort.EnableWindow(TRUE);
		PostMessage(UWM_COMMSIGNAL,0,0);
	}
	else
	{
		int iBaudRate = GetDlgItemInt(IDC_CBB_BAUDRATE,NULL,FALSE);
		int iByteSize = GetDlgItemInt(IDC_CBB_BYTESIZE,NULL,FALSE);
		int iStopBits = m_cbStopBits.GetCurSel();
		int iParity = m_cbParity.GetCurSel();

		int iFlowCtrl = m_cbFlowCtrl.GetCurSel();

		if(
			(iByteSize ==5)&&(iStopBits==2) ||
				(iByteSize!=5)&&(iStopBits==1)
		)
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_BYTESIZE_STOPBITS),NULL,MB_OK|MB_ICONERROR);
			return 0;
		}
		
		CSerialPortInfo* info = (CSerialPortInfo*)m_cbPort.GetItemDataPtr(m_cbPort.GetCurSel());
		if (info == nullptr)
		{
			::MessageBox(m_hWnd, GetStringFromID(IDS_ERR_COMM_OPEN), NULL, MB_OK | MB_ICONERROR);
			return 0;
		}

		if(CMyComm::Instance()->Open(info->device))
		{
			TMyDCB dcb=CMyComm::Instance()->GetDCB(); 
			dcb.fBinary=1; 
			dcb.BaudRate=iBaudRate; //
			dcb.Parity=iParity; //
			dcb.fParity=dcb.Parity?1:0; 
			dcb.ByteSize=iByteSize; //
			dcb.StopBits=iStopBits; //
			dcb.SetFlowControl(TMyDCB::TFlowControl(iFlowCtrl) ); 
			
			if(
				CMyComm::Instance()->SetDCB(dcb) &&
				CMyComm::Instance()->SetTimeouts(20) &&
				CMyComm::Instance()->SetBuffers(4096, 4096))
			{
				m_stOpened.ShowWindow(SW_SHOW);
				m_cbPort.EnableWindow(FALSE);
				m_btnOpen.SetWindowText(GetStringFromID(IDS_CLOSEBTN));
				
				PostMessage(UWM_COMMSIGNAL,0,0);   // Update Line State
				m_chkRTS.SetCheck(BST_UNCHECKED);
				m_chkDTR.SetCheck(BST_UNCHECKED);
				m_chkBREAK.SetCheck(BST_UNCHECKED);
			}
			else
			{
				::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_COMM_PARAMS),NULL,MB_OK|MB_ICONERROR);
				CMyComm::Instance()->Close();
			}
		}
		else
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_COMM_OPEN),NULL,MB_OK|MB_ICONERROR);
		}
	}
	return 0;
}

LRESULT CMainDlg::OnBaudRateChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		TCHAR buf[16];
		
		int iBaudRate = GetDlgItemInt(IDC_CBB_BAUDRATE,NULL,FALSE);

		if(!CMyComm::Instance()->SetBaudRate(iBaudRate))
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_BAUDRATE),NULL,MB_OK|MB_ICONERROR);
			
			::wsprintf(buf,_T("%d"),CMyComm::Instance()->GetBaudRate());

			m_cbBaudRate.SelectString(0,buf);
		}
	}	
	return 0;
}

LRESULT CMainDlg::OnByteSizeChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		TCHAR buf[16];
		
		int iByteSize = GetDlgItemInt(IDC_CBB_BYTESIZE,NULL,FALSE);

		if(!CMyComm::Instance()->SetByteSize(iByteSize))
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_BYTESIZE),NULL,MB_OK|MB_ICONERROR);
			
			::wsprintf(buf,_T("%d"),CMyComm::Instance()->GetByteSize());

			m_cbByteSize.SelectString(0,buf);
		}
	}	
	return 0;
}

LRESULT CMainDlg::OnStopBitsChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		int iStopBits = m_cbStopBits.GetCurSel();
		if(!CMyComm::Instance()->SetStopBits(iStopBits))
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_STOPBITS),NULL,MB_OK|MB_ICONERROR);
			
			m_cbStopBits.SetCurSel(CMyComm::Instance()->GetStopBits());
		}
	}
	return 0;
}

LRESULT CMainDlg::OnFlowCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		TMyDCB dcb=CMyComm::Instance()->GetDCB(); 
		TMyDCB::TFlowControl fcFlowCtrl = (TMyDCB::TFlowControl)m_cbFlowCtrl.GetCurSel();
		dcb.SetFlowControl(fcFlowCtrl); 
		if(!CMyComm::Instance()->SetDCB(dcb))
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_FLOWCTRL),NULL,MB_OK|MB_ICONERROR);
			dcb = CMyComm::Instance()->GetDCB();
			m_cbFlowCtrl.SetCurSel(dcb.GetFlowControl());
		}
	}
	return 0;
}

LRESULT CMainDlg::OnParityChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CMyComm::Instance()->Opened())
	{
		int iParity = m_cbParity.GetCurSel();
		if(!CMyComm::Instance()->SetParity(iParity))
		{
			::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_PARITY),NULL,MB_OK|MB_ICONERROR);
			
			m_cbParity.SetCurSel(CMyComm::Instance()->GetParity());
		}
	}
	return 0;
}

LRESULT CMainDlg::OnWritten(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_chkCount.GetCheck() == BST_CHECKED)
	{
		SetDlgItemInt(IDC_EDT_SEND, GetDlgItemInt(IDC_EDT_SEND,NULL,FALSE) + lParam,FALSE);
	}
	if(m_CommOpts.bSuperT)
	{
		char *Buf=(char *)wParam; 
		CString S(Buf,lParam);
		m_pnlRecv.m_edtText.AppendText(S);
	}
	return 0;
}
LRESULT CMainDlg::OnRead(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_chkCount.GetCheck() == BST_CHECKED)    
	{
		SetDlgItemInt(IDC_EDT_RECV, GetDlgItemInt(IDC_EDT_RECV,NULL,FALSE) + lParam,FALSE);
	}

	if(m_pnlRecv.m_btnPause.GetCheck() ==  BST_UNCHECKED)
	{
		char *Buf = (char *)wParam;
		DWORD Len = (DWORD)lParam;

		CString S;
		if(m_pnlRecv.m_chkHex.GetCheck() == BST_CHECKED)  // 16进制显示
		{
			for(DWORD i=0; i<Len; i++)
			{
				CString AddStr;
				AddStr.Format(_T("%02X "),(BYTE)Buf[i]);
				S = S + AddStr;
			}
		}
		else if(m_CommOpts.bCtrlChar)  // 控制符改成[xx]
		{
			S = CString(Buf,Len);

			for(int i=S.GetLength()-1; i>=0; i--)
			{
				TCHAR c= (TCHAR)S[i]; 
				if(c < 0x20 && c>=0)
				{
					CString AddStr;
					AddStr.Format(_T("[%02X]"),c);
					S.Delete(i);
					S.Insert(i,AddStr);
				}
			}			
		}
		else  // 普通模式
		{
			S = CString(Buf,Len);
		}
		
		// 是否定位当前位置
		DWORD dwCurSel = m_pnlRecv.m_edtText.GetSel();
		m_pnlRecv.m_edtText.AppendText(S,TRUE);
		if( !m_CommOpts.bLastPos )
		{
			m_pnlRecv.m_edtText.SetSel(dwCurSel,TRUE);	
		}

		// 自动保存
		if(m_CommOpts.bAutoSave && m_CommOpts.dCharCount>0 && m_CommOpts.szFileName[0]>='0')
		{
			DWORD len = ::GetWindowTextLengthA(m_pnlRecv.m_edtText);
			if(len >= m_CommOpts.dCharCount)
			{
				HANDLE fh = ::CreateFile( m_CommOpts.szFileName,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
				if(fh != INVALID_HANDLE_VALUE)
				{
					
					char * data = new char[len+1];
					::GetWindowTextA(m_pnlRecv.m_edtText,data,len+1);
					::SetFilePointer(fh,0,NULL,FILE_END);			// Append
					::WriteFile(fh,data,len ,&len,NULL);
					delete []data;
					::CloseHandle(fh);

					m_pnlRecv.m_edtText.SetWindowText(_T(""));   // 保存完后清空文本框
				}
			}
		}
	}
	return 0;
}
LRESULT CMainDlg::OnSignal(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD MState=CMyComm::Instance()->GetCommModemStatus(); 

	m_stCTS.SetTextColor(MState & MS_CTS_ON? 0xff : 0x00);
	m_stDSR.SetTextColor(MState & MS_DSR_ON? 0xff : 0x00);
	m_stRING.SetTextColor(MState & MS_RING_ON? 0xff : 0x00);
	m_stDCD.SetTextColor(MState & MS_RLSD_ON? 0xff : 0x00);

	return 0;
}

LRESULT CMainDlg::OnCountClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_edtRecv.SetWindowText(_T("0"));
	m_edtSend.SetWindowText(_T("0"));
	return 0;
}

LRESULT CMainDlg::OnStateChecked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// IDC_CBX_DTR,IDC_CBX_RTS,IDC_CBX_BREAK
	if(CMyComm::Instance()->Opened())
	{
		CButton Sender = GetDlgItem(wID);
		
		DWORD dwFunc = 0;
		switch(wID){
		case IDC_CBX_DTR:
			dwFunc = SETDTR;
			break;
		case IDC_CBX_RTS:
			dwFunc = SETRTS;
			break;
		case IDC_CBX_BREAK:
			dwFunc = SETBREAK;
			break;
		}
		if(Sender.GetCheck() == BST_UNCHECKED) dwFunc++;  // SET* -> CLR*
		CMyComm::Instance()->EscapeCommFunction(dwFunc);
	}
	return 0;
}
LRESULT CMainDlg::OnOptClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COptionDlg dlg;
	dlg.m_CommOpts = m_CommOpts;

	if(dlg.DoModal() == IDOK)
	{
		m_CommOpts = dlg.m_CommOpts;
	}
	return 0;
}

void CMainDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	if(pRect->right - pRect->left < 530) pRect->right = pRect->left + 530;
	if(pRect->bottom - pRect->top < 450)  pRect->bottom = pRect->top + 450;	
}
