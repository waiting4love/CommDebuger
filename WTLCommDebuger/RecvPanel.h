
#pragma once

#include <atldlgs.h> 

class CRecvPanel 
	: public CDialogImpl<CRecvPanel>,
	public CDialogResize<CRecvPanel>,
	public CMessageFilter
{
public:
	enum {IDD = IDD_RECV};

	BEGIN_MSG_MAP(CRecvPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_ID_HANDLER(IDC_BTN_CLEARDATA, OnClear)
		COMMAND_ID_HANDLER(IDC_BTN_SAVEAS, OnSave)
		COMMAND_ID_HANDLER(IDC_BTN_PAUSE, OnPause);
		
		MESSAGE_HANDLER(UWM_COMMERROR, OnError)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CRecvPanel>)
	END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CRecvPanel)          // RC is ResizeAble
        DLGRESIZE_CONTROL(IDC_EDT_RECV, DLSZ_SIZE_X|DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_CBX_HEXRECV, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_PAUSE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_CLEARDATA, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_SAVEAS, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

	CColorEdit m_edtText;
	CButton m_chkHex;
	CButton m_btnPause;
	CButton m_btnClear;
	CButton m_btnSave;
	
private:
	CFont m_fntText;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false);

		m_edtText.SubclassWindow(GetDlgItem(IDC_EDT_RECV));
		m_edtText.SetTheme(0x00800080,GetSysColor(COLOR_WINDOW),OPAQUE,GetSysColorBrush(COLOR_WINDOW));

		m_chkHex = GetDlgItem(IDC_CBX_HEXRECV);
		m_btnPause = GetDlgItem(IDC_BTN_PAUSE);
		m_btnClear = GetDlgItem(IDC_BTN_CLEARDATA);
		m_btnSave = GetDlgItem(IDC_BTN_SAVEAS);
		
		m_edtText.SetLimitText(0);

		m_fntText.CreatePointFont(90,GetStringFromID(IDS_SWFONT));
		m_edtText.SetFont(m_fntText);

		m_edtText.SetWindowText(GetStringFromID(IDS_RECV_INIT));

		CMyComm::m_hwndListenError = m_hWnd;

        CMessageLoop *pml=_Module.GetMessageLoop();
        pml->AddMessageFilter(this);

		return TRUE;
	}

	BOOL  PreTranslateMessage (MSG *pMsg)
	{
		return IsDialogMessage(pMsg);
	}


	LRESULT OnClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edtText.SetWindowText(_T(""));
		return 0;
	}

	LRESULT OnPause(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_btnPause.SetCheck(
			m_btnPause.GetCheck()==BST_CHECKED?
				BST_UNCHECKED : BST_CHECKED
		);
		return 0;
	}

	LRESULT OnSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog OpenDlg(
				false,
				GetStringFromID(IDS_RECV_OPENDIALOG_DEFEXT),
				NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				GetStringFromID(IDS_RECV_OPENDIALOG_FILTER));
		if(IDOK == OpenDlg.DoModal(m_hWnd))
		{
			HANDLE fh = ::CreateFile(OpenDlg.m_szFileName ,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

			if(fh == INVALID_HANDLE_VALUE)
			{
				::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_OPENFILE),NULL,MB_OK|MB_ICONERROR);
				return 0;
			}

			DWORD len = GetWindowTextLengthA(m_edtText);
			char * data = new char[len+1];
			::GetWindowTextA(m_edtText,data,len+1);

			::WriteFile(fh,data,len ,&len,NULL);
			::CloseHandle(fh);
			
			delete data;
		}
		return 0;
	}
	
	LRESULT OnError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD State = (DWORD)lParam;

		if(State&CE_BREAK)  m_edtText.AppendText(GetStringFromID(IDS_CE_BREAK)); 
		if(State&CE_FRAME)  m_edtText.AppendText(GetStringFromID(IDS_CE_FRAME)); 
		if(State&CE_IOE)  m_edtText.AppendText(GetStringFromID(IDS_CE_IOE)); 
		if(State&CE_MODE)  m_edtText.AppendText(GetStringFromID(IDS_CE_MODE)); 
		if(State&CE_OVERRUN)  m_edtText.AppendText(GetStringFromID(IDS_CE_OVERRUN)); 
		if(State&CE_RXOVER)  m_edtText.AppendText(GetStringFromID(IDS_CE_RXOVER)); 
		if(State&CE_RXPARITY) m_edtText.AppendText(GetStringFromID(IDS_CE_RXPARITY)); 
		if(State&CE_TXFULL)  m_edtText.AppendText(GetStringFromID(IDS_CE_TXFULL)); 

		return 0;
	}
};