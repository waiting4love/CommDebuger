#pragma once

#include <atldlgs.h>
struct TCommOptions{
	bool bLastPos;
    bool bCtrlChar;
    bool bSuperT;
    bool bAutoSave;
    unsigned dCharCount;
    TCHAR szFileName[MAX_PATH];
};

class COptionDlg
	: public CDialogImpl<COptionDlg>
{
public:	
	enum {IDD = IDD_OPTION};

	TCommOptions m_CommOpts;

	BEGIN_MSG_MAP_EX(COptionDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER(IDOK,OnModalClick)
		COMMAND_ID_HANDLER(IDCANCEL,OnModalClick)
		COMMAND_ID_HANDLER(IDC_BTN_FILENAME,OnFilename)
		COMMAND_ID_HANDLER(IDC_CBX_AUTOSAVE,OnAutoSaveClick)
	END_MSG_MAP()

	CButton m_chkLastPos;
	CButton m_chkShowCtrlChar;
	CButton m_chkHTLike;
	CButton m_chkAutoSave;
	CEdit m_edtSaveCount;
	CEdit m_edtFilename;
	CButton m_btnFilename;

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		m_edtFilename = GetDlgItem(IDC_EDT_FILENAME);
		m_btnFilename = GetDlgItem(IDC_BTN_FILENAME);
		m_chkLastPos = GetDlgItem(IDC_CBX_LASTPOS);
		m_chkShowCtrlChar = GetDlgItem(IDC_CBX_SHOWCTRLCHAR);
		m_chkHTLike = GetDlgItem(IDC_CBX_HTLIKE);
		m_chkAutoSave = GetDlgItem(IDC_CBX_AUTOSAVE);
		m_edtSaveCount = GetDlgItem(IDC_EDT_COUNT);

		if(m_CommOpts.bLastPos) m_chkLastPos.SetCheck(BST_CHECKED);
		if(m_CommOpts.bCtrlChar) m_chkShowCtrlChar.SetCheck(BST_CHECKED);
		if(m_CommOpts.bSuperT) m_chkHTLike.SetCheck(BST_CHECKED);
		if(m_CommOpts.bAutoSave) m_chkAutoSave.SetCheck(BST_CHECKED);

		SetDlgItemInt(IDC_EDT_COUNT,m_CommOpts.dCharCount,FALSE);

		m_edtFilename.SetWindowText(m_CommOpts.szFileName);

		return 0;
	}
	LRESULT OnModalClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(wID == IDOK)
		{
			m_CommOpts.bLastPos = (m_chkLastPos.GetCheck()==BST_CHECKED);
			m_CommOpts.bCtrlChar= (m_chkShowCtrlChar.GetCheck()==BST_CHECKED);
			m_CommOpts.bSuperT  = (m_chkHTLike.GetCheck()==BST_CHECKED);
			m_CommOpts.bAutoSave= (m_chkAutoSave.GetCheck()==BST_CHECKED);

			m_CommOpts.dCharCount = GetDlgItemInt(IDC_EDT_COUNT,NULL,FALSE);

			if(m_CommOpts.dCharCount == 0) m_CommOpts.dCharCount = 10000;

			m_edtFilename.GetWindowText(m_CommOpts.szFileName,MAX_PATH);
		}
		EndDialog(wID);
		return 0;
	}
	void OnClose()
	{
		EndDialog(IDCANCEL);
	}
	LRESULT OnFilename(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog dlg(TRUE);
		m_edtFilename.GetWindowText(dlg.m_szFileName,MAX_PATH);
		if(dlg.DoModal(m_hWnd)==IDOK)
		{
			m_edtFilename.SetWindowText(dlg.m_szFileName);
		}
		return 0;
	}
	LRESULT OnAutoSaveClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL Checked = (m_chkAutoSave.GetCheck()== BST_CHECKED);
		
		m_edtSaveCount.EnableWindow(Checked);
		m_edtFilename.EnableWindow(Checked);
		m_btnFilename.EnableWindow(Checked);
		
		return 0;
	}
};
