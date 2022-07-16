#pragma once
#include <vector>
class CSerialPortInfo
{
public:
	CSerialPortInfo()
		: vendorIdentifier(0)
		, productIdentifier(0)
		, hasVendorIdentifier(false)
		, hasProductIdentifier(false)
	{
	}

	~CSerialPortInfo()
	{
	}

	static CString portNameToSystemLocation(LPCTSTR source);
	static CString portNameFromSystemLocation(LPCTSTR source);

	CString portName;
	CString device;
	CString description;
	CString manufacturer;
	CString serialNumber;

	UINT16 vendorIdentifier;
	UINT16 productIdentifier;

	bool    hasVendorIdentifier;
	bool    hasProductIdentifier;
};

typedef std::vector<CSerialPortInfo> CSerialPortInfoList;
CSerialPortInfoList GetAvailablePorts();

