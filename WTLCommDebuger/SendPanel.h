
#pragma once
#include <atldlgs.h> 
#include <vector>
#define AUTO_SEND_TIMER 3724
class CSendPanel 
	: public CDialogImpl<CSendPanel>,
	public CDialogResize<CSendPanel>,
	public CMessageFilter
{
public:
	enum {IDD = IDD_SEND};

	BEGIN_MSG_MAP_EX(CSendPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_ID_HANDLER(IDC_CBX_AUTOSEND,OnAutoSend)

		COMMAND_ID_HANDLER(IDC_BTN_CLEARSEND, OnClear)
		COMMAND_ID_HANDLER(IDC_BTN_STOPSEND, OnStop)
		COMMAND_ID_HANDLER(IDC_BTN_SEND,OnSend)
		COMMAND_ID_HANDLER(IDC_BTN_READFILE, OnReadFile)

		MSG_WM_TIMER(OnTimer)
		
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CSendPanel>)
	END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CSendPanel)          // RC is ResizeAble
        DLGRESIZE_CONTROL(IDC_EDT_SEND, DLSZ_SIZE_X|DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_CBX_AUTOSEND, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_CBX_HEXSEND, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_EDT_INTERVAL, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_SEND, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_CLEARSEND, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_STOPSEND, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_READFILE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC_IV1, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC_IV2, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

	CColorEdit m_edtText;
	CButton m_chkAuto;
	CButton m_chkHex;
	CEdit m_edtInterval;
	CButton m_btnSend;
	CButton m_btnStop;
	CButton m_btnClear;
	CButton m_btnRead;

private:

	CFont m_fntText;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false);

		m_edtText.SubclassWindow(GetDlgItem(IDC_EDT_SEND));
		m_edtText.SetTheme(0x00800000,GetSysColor(COLOR_WINDOW),OPAQUE,GetSysColorBrush(COLOR_WINDOW));

		m_chkAuto = GetDlgItem(IDC_CBX_AUTOSEND);
		m_chkHex = GetDlgItem(IDC_CBX_HEXSEND);
		m_edtInterval = GetDlgItem(IDC_EDT_INTERVAL);
		m_btnSend = GetDlgItem(IDC_BTN_SEND);
		m_btnStop = GetDlgItem(IDC_BTN_STOPSEND);
		m_btnClear = GetDlgItem(IDC_BTN_CLEARSEND);
		m_btnRead = GetDlgItem(IDC_BTN_READFILE);
		
		m_edtText.SetLimitText(0);

		m_fntText.CreatePointFont(90,GetStringFromID(IDS_SWFONT));
		m_edtText.SetFont(m_fntText);

		m_edtText.SetWindowText(GetStringFromID(IDS_SEND_INIT));

		SetDlgItemInt(IDC_EDT_INTERVAL,1000,FALSE);

		CMessageLoop *pml=_Module.GetMessageLoop();
        pml->AddMessageFilter(this);

		return TRUE;
	}

	BOOL  PreTranslateMessage (MSG *pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	LRESULT OnAutoSend(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		bool bCurAutoSend = m_chkAuto.GetCheck()==BST_CHECKED;
	
		if(!bCurAutoSend)
		{
			UINT uInterval = GetDlgItemInt(IDC_EDT_INTERVAL,NULL,FALSE);
			
			if(uInterval > 0)
			{
				SetTimer(AUTO_SEND_TIMER,uInterval);

				m_chkAuto.SetCheck(BST_CHECKED);
				bCurAutoSend = true;

				// SetDlgItemInt(IDC_EDT_INTERVAL,uInterval,FALSE);
			}
		}
		else
		{
			KillTimer(AUTO_SEND_TIMER);

			m_chkAuto.SetCheck(BST_UNCHECKED);
			bCurAutoSend = false;
		}

		m_edtInterval.SetReadOnly(bCurAutoSend);
		return 0;
	}

	LRESULT OnClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edtText.SetWindowText(_T(""));
		return 0;
	}
	
	LRESULT OnSend(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(!CMyComm::Instance()->Opened()) return 0;
		
		int len = ::GetWindowTextLengthA(m_edtText);
		char *txt = new char[len+1];
		::GetWindowTextA(m_edtText,txt,len+1);   // must change to ASCII

		if(m_chkHex.GetCheck() == BST_CHECKED)
		{
			std::vector<char> SendData;
			char *p = strtok(txt," ,\r\n\t\b");
			while(p)
			{
				long data = strtol(p,NULL,16);

				BYTE b = BYTE(data & 0xff); 
				SendData.push_back(b);

				p = strtok(NULL," ,\r\n\t\b");
			}
			CMyComm::Instance()->Write(&SendData[0],SendData.size());
		}
		else
		{
			CMyComm::Instance()->Write(txt,len);
		}

		delete []txt;
		return 0;
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent == AUTO_SEND_TIMER)
		{
			BOOL b;
			OnSend(0,0,0,b);
		}
	}

	LRESULT OnStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_chkAuto.GetCheck()==BST_CHECKED) m_chkAuto.Click();
		if(CMyComm::Instance()->Opened()) CMyComm::Instance()->ClearWrtBuf(); 
		return 0;
	}

	LRESULT OnReadFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog OpenDlg(true);
		if(IDOK == OpenDlg.DoModal(m_hWnd))
		{
			HANDLE fh= ::CreateFile(OpenDlg.m_szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
			if(fh == INVALID_HANDLE_VALUE)
			{
				::MessageBox(m_hWnd,GetStringFromID(IDS_ERR_OPENFILE),NULL,MB_OK|MB_ICONERROR);
				return 0;
			}

			HANDLE hMap = ::CreateFileMapping(fh, NULL, PAGE_READONLY,0,0,NULL);     // Use filemapping to load entire file
			if(hMap != NULL)
			{
				LPVOID pData = ::MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
				if(pData != NULL)
				{
					if(m_chkHex.GetCheck() == BST_CHECKED)
					{
						DWORD dwSize = GetFileSize(fh, NULL);
						PBYTE pBytes = (PBYTE)pData;

						TCHAR *HexData = new TCHAR[dwSize*3+dwSize/8+1];

						TCHAR *p=HexData;
						for(DWORD i=0; i<dwSize; i++)
						{
							::wsprintf(p,_T("%02X "),pBytes[i]);
							p+=3;
							if(i % 16 ==15 )
							{
								*(p++)=_T('\r');
								*(p++)=_T('\n');
								*p = 0;
							}
						}
						
						m_edtText.SetWindowText(HexData);
						delete []HexData;
					}
					else
					{
						if((*(WORD*)pData) == 0xfeff)    // Is Unicode flag
						{
							::SetWindowTextW(m_edtText, (LPCWSTR)pData);
						}
						else
						{
							::SetWindowTextA(m_edtText, (LPCSTR)pData);
						}
					}
					::UnmapViewOfFile(pData);
				}
				::CloseHandle(hMap);
			}
			::CloseHandle(fh);

		}
		return 0;
	}
};