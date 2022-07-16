// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "ColorStatic.h"
#include "SendPanel.h"
#include "RecvPanel.h"
#include "Option.h"
#include "SerialPortInfo.h"


template <bool t_bVertical>
class CMySplitterWindowT : 
    public CSplitterWindowImpl<CMySplitterWindowT<t_bVertical>>
{
public:
	using base_t = CSplitterWindowImpl<CMySplitterWindowT<t_bVertical>>;
    DECLARE_WND_CLASS_EX(_T("My_SplitterWindow"), 
                         CS_DBLCLKS, COLOR_WINDOW)

    // Overrideables
    void DrawSplitterBar(CDCHandle dc)
    {
		RECT rect;
  
        if ( GetSplitterBarRect ( &rect ) )
        {
			dc.FillRect(&rect, COLOR_BTNFACE);
			dc.DrawEdge(&rect, EDGE_ETCHED, 
						t_bVertical ? (BF_LEFT | BF_RIGHT) 
									: (BF_TOP | BF_BOTTOM));
        }
    }

	CMySplitterWindowT()
		:base_t(t_bVertical)
	{}
 };
typedef CMySplitterWindowT<true>    CMySplitterWindow;
typedef CMySplitterWindowT<false>   CMyHorSplitterWindow;


class CMainDlg 
	: public CDialogImpl<CMainDlg>,
	public CMessageFilter,
	public CDialogResize<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG, ID_SPLITTER = 999 };

	BEGIN_MSG_MAP_EX(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_CLOSE(OnClose)

		COMMAND_ID_HANDLER(IDC_BTN_COMMOPEN, OnCommOpen)
		COMMAND_HANDLER(IDC_CBB_BAUDRATE,CBN_SELCHANGE,OnBaudRateChange)  
		COMMAND_HANDLER(IDC_CBB_BYTESIZE,CBN_SELCHANGE,OnByteSizeChange)  
		COMMAND_HANDLER(IDC_CBB_STOPBITS,CBN_SELCHANGE,OnStopBitsChange)  
		COMMAND_HANDLER(IDC_CBB_PARITY,CBN_SELCHANGE,OnParityChange)
		COMMAND_HANDLER(IDC_CBB_FLOWCTRL,CBN_SELCHANGE,OnFlowCtrlChange)

		COMMAND_ID_HANDLER(IDC_BTN_COUNTCLEAR, OnCountClear)

		COMMAND_ID_HANDLER(IDC_CBX_DTR, OnStateChecked)
		COMMAND_ID_HANDLER(IDC_CBX_RTS, OnStateChecked)
		COMMAND_ID_HANDLER(IDC_CBX_BREAK, OnStateChecked)

		MESSAGE_HANDLER(UWM_COMMWRITE, OnWritten)
		MESSAGE_HANDLER(UWM_COMMREAD, OnRead)
		MESSAGE_HANDLER(UWM_COMMSIGNAL, OnSignal)
		MSG_WM_SIZING(OnSizing)

		COMMAND_ID_HANDLER(IDC_BTN_OPTION, OnOptClick)
		COMMAND_ID_HANDLER(IDC_BTN_EXIT, OnExit)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CMainDlg>)

	END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CMainDlg)          // RC is ResizeAble
        DLGRESIZE_CONTROL(ID_SPLITTER, DLSZ_SIZE_X|DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_GROUP_DATA, DLSZ_SIZE_X|DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

public:
	TCommOptions m_CommOpts;

	// 1: Create, 2: AddTools, 3:RelayEvent on PreTranslateMessage or Self MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
	// SS_NOTIFY is needed for STATIC
	CToolTipCtrl m_tip;  

	CComboBox m_cbPort;
	CComboBox m_cbBaudRate;
	CComboBox m_cbByteSize;
	CComboBox m_cbStopBits;
	CComboBox m_cbParity;
	CComboBox m_cbFlowCtrl;
	CButton m_btnOpen;
	CButton m_chkCount;
	CButton m_btnCntClear;
	CEdit m_edtSend;
	CEdit m_edtRecv;
	CButton m_chkDTR;
	CButton m_chkRTS;
	CButton m_chkBREAK;
	CColorStatic m_stOpened;
	CColorStatic m_stCTS;
	CColorStatic m_stDSR;
	CColorStatic m_stRING;
	CColorStatic m_stDCD;
	CMyHorSplitterWindow m_Splitter;

	CSendPanel m_pnlSend;
	CRecvPanel m_pnlRecv;
	CFixedGroup m_gpInit,m_gpCount,m_gpState;

	CMainDlg();

private:
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOptClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCommOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBaudRateChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnByteSizeChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStopBitsChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);	
	LRESULT OnParityChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFlowCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCountClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnStateChecked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnWritten(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRead(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSignal(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	void OnSizing(UINT fwSide, LPRECT pRect);

	void OnClose();

	BOOL PreTranslateMessage (MSG *pMsg);

	CSerialPortInfoList m_vecSerialPort;
};
