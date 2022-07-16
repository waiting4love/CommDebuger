#include "stdafx.h"
#include "resource.h"
#include "ConnCfgDlg.h"
#include "SerialPortInfo.h"
#include "MotorMan.h"

CConnCfgDlg::CConnCfgDlg()
{
}


CConnCfgDlg::~CConnCfgDlg()
{
}


LRESULT CConnCfgDlg::OnBnClickedRadioSerial(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	BOOL is_serial = (m_radioUSB.GetCheck() == BST_CHECKED?FALSE:TRUE);
	m_cbxBaudrate.EnableWindow(is_serial);
	m_cbxPort.EnableWindow(is_serial);
	return 0;
}


LRESULT CConnCfgDlg::OnOkCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (wID == IDOK)
	{
		GetConfig(&m_cfg);
	}
	EndDialog(wID);
	return 0;
}

LRESULT CConnCfgDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_radioUSB = GetDlgItem(IDC_RADIO_USB);
	CButton radioSerial = GetDlgItem(IDC_RADIO_SERIAL);
	m_cbxBaudrate = GetDlgItem(IDC_COMBO_BAUDRATE);
	m_cbxPort = GetDlgItem(IDC_COMBO_PORT);

	const int BaudRates[] = {
		CBR_9600   ,
		CBR_19200  ,
		CBR_38400  ,
		CBR_57600  ,
		CBR_115200 ,
	};

	//BaudRate
	TCHAR buff[128];
	for (int i = 0; i < ARRAYSIZE(BaudRates); i++)
	{
		wsprintf(buff, _T("%d"), BaudRates[i]);
		int idx = m_cbxBaudrate.AddString(buff);
		m_cbxBaudrate.SetItemData(idx, BaudRates[i]);
	}

	const int* BaudRatePtr = std::find(BaudRates, BaudRates + ARRAYSIZE(BaudRates), m_cfg.BandRate);
	int BaudRateIdx = std::distance(BaudRates, BaudRatePtr);
	if (BaudRateIdx >= ARRAYSIZE(BaudRates)) BaudRateIdx = 0;
	m_cbxBaudrate.SetCurSel(BaudRateIdx);

	//Ports
	TCHAR szPortName[128];
	int nSelIdx = -1;
	
	struct TListItem
	{
		int PortId;
		CString PortName;
	};

	//端口列表
	std::vector<TListItem> PortList;
	for (int i = 1; i < 16; i++)
	{
		wsprintf(szPortName, _T("COM%d"), i);
		PortList.push_back({i, szPortName});
	}

	//把目前在系统里的COM口改个醒目点的名字
	for (auto x : GetAvailablePorts())
	{
		if (StrCmpN(x.portName, _T("COM"), 3) == 0)
		{
			CString s = x.portName + _T(" - ") + x.description;
			int idPort = StrToInt((LPCTSTR)s + 3);
			auto itr = std::find_if(PortList.begin(), PortList.end(), [idPort](TListItem& li) {return li.PortId == idPort; });
			if (itr == PortList.end())
			{
				PortList.push_back({ idPort, s });
			}
			else
			{
				itr->PortName = s;
			}
		}
	}

	for (auto x : PortList)
	{
		int idx = m_cbxPort.AddString(x.PortName);
		m_cbxPort.SetItemData(idx, x.PortId);
		if (x.PortId == m_cfg.Port)
			nSelIdx = idx;
	}

	m_cbxPort.SetCurSel(nSelIdx);

	if (!m_bIsMX)
	{
		radioSerial.Click();
		m_radioUSB.ShowWindow(SW_HIDE);
		radioSerial.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PROTOCOL).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOMX).ShowWindow(SW_SHOW);
	}
	else
	{
		//Protocol
		if (m_cfg.Protocol == TProtocolType::USB)
			m_radioUSB.Click();
		else
			radioSerial.Click();
		GetDlgItem(IDC_STATIC_NOMX).ShowWindow(SW_HIDE);
	}
	return TRUE;
}


LRESULT CConnCfgDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::GetDlgCtrlID((HWND)lParam) == IDC_STATIC_NOMX)
	{
		HDC dc = (HDC)wParam;
		SetTextColor(dc, 0xff);
		SetBkMode(dc, TRANSPARENT);
		return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}


LRESULT CConnCfgDlg::OnBnClickedButtonTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TConnectionConfigData cfg;
	GetConfig(&cfg);
	IMotorTable* mt = GetGlobalMotorTable();

	UINT ids_prompt = IDS_FAIL;
	UINT ids_title = IDS_TEST;
	UINT btn = MB_ICONERROR;

	if (mt->Open(cfg))
	{
		mt->Close();
		ids_prompt = IDS_SUCC;
		btn = MB_ICONINFORMATION;
	}
	mt->Destroy();

	TCHAR prompt[128], title[128];
	AtlLoadString(ids_prompt, prompt, ARRAYSIZE(prompt));
	AtlLoadString(ids_title, title, ARRAYSIZE(title));
	MessageBox(prompt, title, btn);

	return 0;
}

void CConnCfgDlg::GetConfig(TConnectionConfigData* cfg)
{
	ZeroMemory(cfg, sizeof(TConnectionConfigData));
	cfg->Port = 1;
	cfg->Protocol = TProtocolType::Serial;
	cfg->BandRate = CBR_9600;

	int idxPort = m_cbxPort.GetCurSel();
	if (idxPort >= 0)
		cfg->Port = m_cbxPort.GetItemData(idxPort);

	int idxBaudRate = m_cbxBaudrate.GetCurSel();
	if (idxBaudRate >= 0)
		cfg->BandRate = m_cbxBaudrate.GetItemData(idxBaudRate);

	if (m_radioUSB.GetCheck() == BST_CHECKED)
		cfg->Protocol = TProtocolType::USB;
	else
		cfg->Protocol = TProtocolType::Serial;
}
