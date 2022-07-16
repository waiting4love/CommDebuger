#include "stdafx.h"
#include "SerialPortInfo.h"

#include <initguid.h>
#include <devguid.h> // for GUID_DEVCLASS_PORTS and GUID_DEVCLASS_MODEM
#include <winioctl.h> // for GUID_DEVINTERFACE_COMPORT
#include <setupapi.h>
#include <cfgmgr32.h>

#pragma  comment(lib, "setupapi.lib")

//#include <ntddmodm.h> // for GUID_DEVINTERFACE_MODEM
DEFINE_GUID(GUID_DEVINTERFACE_MODEM, 0x2c7089aa, 0x2e0e, 0x11d1, 0xb1, 0x14, 0x00, 0xc0, 0x4f, 0xc2, 0xaa, 0xe4);

typedef std::vector<CString> CStringList;

CStringList portNamesFromHardwareDeviceMap()
{
	HKEY hKey = nullptr;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		return CStringList();

	CStringList result;
	DWORD index = 0;

	// This is a maximum length of value name, see:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724872%28v=vs.85%29.aspx
	enum { MaximumValueNameInChars = 16383 };

	std::vector<wchar_t> outputValueName(MaximumValueNameInChars, 0);
	std::vector<wchar_t> outputBuffer(MAX_PATH + 1, 0);
	DWORD bytesRequired = MAX_PATH;
	for (;;) {
		DWORD requiredValueNameChars = MaximumValueNameInChars;
		const LONG ret = ::RegEnumValue(hKey, index, &outputValueName[0], &requiredValueNameChars,
			nullptr, nullptr, reinterpret_cast<PBYTE>(&outputBuffer[0]), &bytesRequired);
		if (ret == ERROR_MORE_DATA) {
			outputBuffer.resize(bytesRequired / sizeof(wchar_t) + 2, 0);
		}
		else if (ret == ERROR_SUCCESS) {
			result.push_back(&outputBuffer[0]);
			++index;
		}
		else {
			break;
		}
	}
	::RegCloseKey(hKey);
	return result;
}

CString deviceRegistryProperty(HDEVINFO deviceInfoSet,
	PSP_DEVINFO_DATA deviceInfoData,
	DWORD property)
{
	DWORD dataType = 0;
	std::vector<wchar_t> outputBuffer(MAX_PATH + 1, 0);
	DWORD bytesRequired = MAX_PATH;
	for (;;) {
		if (::SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, &dataType,
			reinterpret_cast<PBYTE>(&outputBuffer[0]),
			bytesRequired, &bytesRequired)) {
			break;
		}

		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER
			|| (dataType != REG_SZ && dataType != REG_EXPAND_SZ)) {
			return CString();
		}
		outputBuffer.resize(bytesRequired / sizeof(wchar_t) + 2, 0);
	}
	return &outputBuffer[0];
}

CString deviceInstanceIdentifier(DEVINST deviceInstanceNumber)
{
	std::vector<wchar_t> outputBuffer(MAX_DEVICE_ID_LEN + 1, 0);
	if (::CM_Get_Device_ID(
		deviceInstanceNumber,
		&outputBuffer[0],
		MAX_DEVICE_ID_LEN,
		0) != CR_SUCCESS) {
		return CString();
	}

	CString result(&outputBuffer[0]);
	result.MakeUpper();
	return result;
}

DEVINST parentDeviceInstanceNumber(DEVINST childDeviceInstanceNumber)
{
	ULONG nodeStatus = 0;
	ULONG problemNumber = 0;
	if (::CM_Get_DevNode_Status(&nodeStatus, &problemNumber,
		childDeviceInstanceNumber, 0) != CR_SUCCESS) {
		return 0;
	}
	DEVINST parentInstanceNumber = 0;
	if (::CM_Get_Parent(&parentInstanceNumber, childDeviceInstanceNumber, 0) != CR_SUCCESS)
		return 0;
	return parentInstanceNumber;
}

CString deviceDescription(HDEVINFO deviceInfoSet,
	PSP_DEVINFO_DATA deviceInfoData)
{
	return deviceRegistryProperty(deviceInfoSet, deviceInfoData, SPDRP_DEVICEDESC);
}

CString deviceManufacturer(HDEVINFO deviceInfoSet,
	PSP_DEVINFO_DATA deviceInfoData)
{
	return deviceRegistryProperty(deviceInfoSet, deviceInfoData, SPDRP_MFG);
}

UINT16 parseDeviceIdentifier(LPCTSTR instanceIdentifier,
	LPCTSTR identifierPrefix,
	int identifierSize, bool &ok)
{
	LPCTSTR p = StrStr(instanceIdentifier, identifierPrefix);
	if (p == NULL) return 0;

	p += lstrlen(identifierPrefix);
	CString s(p, std::min<>(identifierSize, lstrlen(p)));
	
	LPTSTR endp=NULL;
	UINT16 res = UINT16(std::_tcstoul(s, &endp, 16));

	ok = (*endp==0);

	return res;
}

UINT16 deviceVendorIdentifier(LPCTSTR instanceIdentifier, bool &ok)
{
	static const int vendorIdentifierSize = 4;
	UINT16 result = parseDeviceIdentifier(
		instanceIdentifier, _T("VID_"), vendorIdentifierSize, ok);
	if (!ok)
		result = parseDeviceIdentifier(
			instanceIdentifier, _T("VEN_"), vendorIdentifierSize, ok);
	return result;
}

UINT16 deviceProductIdentifier(LPCTSTR instanceIdentifier, bool &ok)
{
	static const int productIdentifierSize = 4;
	UINT16 result = parseDeviceIdentifier(
		instanceIdentifier, _T("PID_"), productIdentifierSize, ok);
	if (!ok)
		result = parseDeviceIdentifier(
			instanceIdentifier, _T("DEV_"), productIdentifierSize, ok);
	return result;
}

CString parseDeviceSerialNumber(LPCTSTR instanceIdentifier)
{	
	int lenOfParam = lstrlen(instanceIdentifier);
	LPTSTR pFB = StrRChr(instanceIdentifier, NULL, _T('\\'));
	LPTSTR pLB = NULL;

	if (pFB)
		pLB = StrChr(pFB, _T('\\'));

	if (StrCmpN(instanceIdentifier, _T("USB\\"), 4) == 0)
	{
		if (pLB != instanceIdentifier + lenOfParam - 3)
		{
			pLB = const_cast<LPTSTR>(instanceIdentifier + lenOfParam);
		}

		if (pFB)
		{
			LPTSTR pAS = StrChr(pFB, _T('&'));
			if (pAS && pAS < pLB) return CString();
		}
	}
	else if (StrCmpN(instanceIdentifier, _T("FTDIBUS\\"), 8) == 0)
	{
		pFB = StrRChr(instanceIdentifier, NULL, _T('+'));
		pLB = NULL;
		if (pFB)
		{
			pLB = StrChr(pFB, _T('\\'));
		}

		if (pLB == NULL)
			return CString();
	}
	else
	{
		return CString();
	}

	return (pLB!=NULL&&pLB!=NULL)? CString(pFB + 1, pLB - pFB - 1):CString();
}

CString deviceSerialNumber(CString instanceIdentifier,
	DEVINST deviceInstanceNumber)
{
	for (;;) {
		const CString result = parseDeviceSerialNumber(instanceIdentifier);
		if (!result.IsEmpty())
			return result;
		deviceInstanceNumber = parentDeviceInstanceNumber(deviceInstanceNumber);
		if (deviceInstanceNumber == 0)
			break;
		instanceIdentifier = deviceInstanceIdentifier(deviceInstanceNumber);
		if (instanceIdentifier.IsEmpty())
			break;
	}

	return CString();
}

bool anyOfPorts(const CSerialPortInfoList& ports, LPCTSTR portName)
{
	const auto end = ports.end();
	auto isPortNamesEquals = [&portName](const CSerialPortInfo &portInfo) {
		return portInfo.portName == portName;
	};
	return std::find_if(ports.begin(), end, isPortNamesEquals) != end;
}

void devicePortName(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData, LPTSTR portName)
{
	portName[0] = 0;
	const HKEY key = ::SetupDiOpenDevRegKey(deviceInfoSet, deviceInfoData, DICS_FLAG_GLOBAL,
		0, DIREG_DEV, KEY_READ);
	if (key == INVALID_HANDLE_VALUE) return;

	static const wchar_t * const keyTokens[] = {
		L"PortName\0",
		L"PortNumber\0"
	};

	enum { KeyTokensCount = sizeof(keyTokens) / sizeof(keyTokens[0]) };

	for (int i = 0; i < KeyTokensCount; ++i) {
		DWORD dataType = 0;
		std::vector<wchar_t> outputBuffer(MAX_PATH + 1, 0);
		DWORD bytesRequired = MAX_PATH;
		for (;;) {
			const LONG ret = ::RegQueryValueEx(key, keyTokens[i], nullptr, &dataType,
				reinterpret_cast<PBYTE>(&outputBuffer[0]), &bytesRequired);
			if (ret == ERROR_MORE_DATA) {
				outputBuffer.resize(bytesRequired / sizeof(wchar_t) + 2, 0);
				continue;
			}
			else if (ret == ERROR_SUCCESS) {
				if (dataType == REG_SZ)
#ifdef _UNICODE
					lstrcpy(portName, &outputBuffer[0]);
#else
					WideCharToMultiByte(CP_ACP, 0, &outputBuffer[0], -1, portName, MAX_PATH, NULL);
#endif
				else if (dataType == REG_DWORD)
					wsprintf(portName, _T("COM%d"), *(PDWORD(&outputBuffer[0])));
			}
			break;
		}

		if (portName[0])
			break;
	}
	::RegCloseKey(key);
}

CSerialPortInfoList GetAvailablePorts()
{
	static const struct {
		GUID guid; DWORD flags;
	} setupTokens[] = {
		{ GUID_DEVCLASS_PORTS, DIGCF_PRESENT },
		{ GUID_DEVCLASS_MODEM, DIGCF_PRESENT },
		{ GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE },
		{ GUID_DEVINTERFACE_MODEM, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE }
	};

	enum { SetupTokensCount = sizeof(setupTokens) / sizeof(setupTokens[0]) };

	CSerialPortInfoList serialPortInfoList;

	for (int i = 0; i < SetupTokensCount; ++i) {
		const HDEVINFO deviceInfoSet = ::SetupDiGetClassDevs(&setupTokens[i].guid, nullptr, nullptr, setupTokens[i].flags);
		if (deviceInfoSet == INVALID_HANDLE_VALUE)
			return serialPortInfoList;

		SP_DEVINFO_DATA deviceInfoData;
		::memset(&deviceInfoData, 0, sizeof(deviceInfoData));
		deviceInfoData.cbSize = sizeof(deviceInfoData);

		DWORD index = 0;
		while (::SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData)) {
			TCHAR portName[MAX_PATH] = { 0 };
			devicePortName(deviceInfoSet, &deviceInfoData, portName);
			//if (portName.isEmpty() || portName.contains(QLatin1String("LPT")))
			//	continue;
			if (portName[0] == 0 || StrStr(portName, _T("LPT")) != NULL)
				continue;

			if (anyOfPorts(serialPortInfoList, portName))
				continue;

			CSerialPortInfo priv;

			priv.portName = portName;
			priv.device = CSerialPortInfo::portNameToSystemLocation(portName);
			priv.description = deviceDescription(deviceInfoSet, &deviceInfoData);
			priv.manufacturer = deviceManufacturer(deviceInfoSet, &deviceInfoData);

			CString instanceIdentifier = deviceInstanceIdentifier(deviceInfoData.DevInst);

			priv.serialNumber =
				deviceSerialNumber(instanceIdentifier, deviceInfoData.DevInst);
			priv.vendorIdentifier =
				deviceVendorIdentifier(instanceIdentifier, priv.hasVendorIdentifier);
			priv.productIdentifier =
				deviceProductIdentifier(instanceIdentifier, priv.hasProductIdentifier);

			serialPortInfoList.push_back(priv);
		}
		::SetupDiDestroyDeviceInfoList(deviceInfoSet);
	}

	const auto portNames = portNamesFromHardwareDeviceMap();
	for (const CString &portName : portNames) {
		if (!anyOfPorts(serialPortInfoList, portName)) {
			CSerialPortInfo priv;
			priv.portName = portName;
			priv.device = CSerialPortInfo::portNameToSystemLocation(portName);
			serialPortInfoList.push_back(priv);
		}
	}

	return serialPortInfoList;
}

CString CSerialPortInfo::portNameToSystemLocation(LPCTSTR source)
{
	return StrCmpN(source, _T("COM"), 3) == 0 ?
		CString(_T("\\\\.\\")) + source : CString(source);
}

CString CSerialPortInfo::portNameFromSystemLocation(LPCTSTR source)
{
	return (StrCmpN(source, _T("\\\\.\\"), 4) == 0 || StrCmpN(source, _T("//./"), 4) == 0) ?
		CString(source + 4) : CString(source);
}
