//---------------------------------------------------------------------------
#ifndef TMyCommAPIH
#define TMyCommAPIH

#include <windows.h>
#include <process.h>

#pragma warning (disable: 4800)   // disable BOOL->bool performance warning
#pragma warning (disable: 4068)   // disable unknown pragma

const unsigned long INVALIDTHREAD=(unsigned long)-1;

//---------------------------------------------------------------------------

//! Simple encapsulation of DCB
struct TMyDCB :DCB
{
    //! enum of DCB's Flow Control
    /*!
        \sa SetFlowControl()
    */
    enum TFlowControl
    {
    	fcNull=0, /*!< Null Flow Control*/
        fcHardware=1, /*!< Hardware Flow Control*/
        fcXonXoff=2  /*!< Xon\\Xoff Flow Control*/
    };

public:
	//! Constructor
	/*!
        Set All member to Zero except DCBlength=sizeof(DCB)	   
	*/
	TMyDCB()
	{
		ZeroMemory(this, sizeof(DCB)); 
		DCBlength=sizeof(DCB); 
	}
	
	//! Constructor
	/*!
	   \param dcb Set all member is same as dcb 
	*/
	TMyDCB(const DCB &dcb)
	{
	   *this=dcb;
	}
	
	//! Assign dcb
	/*!
	   \param dcb Set all member is same as dcb 
	*/	
	TMyDCB& operator=(const DCB &dcb)
	{
		CopyMemory(this, &dcb, sizeof(DCB)); 
		return *this; 
	}

public:	
    //! Fill this DCB structure with string. 
    /*!
        The Build function fills this DCB structure with values specified in a device-control string. 
        The device-control string uses the syntax of the mode command.
        \param strDCB Pointer to a null-terminated string that specifies device-control information. 
            The string must have the same form as the mode command's command-line arguments. 
            For example,\n
            the following string specifies a baud rate of 1200, no parity, 8 data bits, and 1 stop bit:\n
            "baud=1200 parity=N data=8 stop=1".\n
            The device name is ignored if it is included in the string, but it must specify a valid device, as follows:\n
            "COM1: baud=1200 parity=N data=8 stop=1".\n
            For further information on mode command syntax, refer to the end-user documentation for your operating system.
            
        \return If the function succeeds, return true.
    */
	bool Build(LPCTSTR strDCB)
	{
		return BuildCommDCB(strDCB, this); 
	}

    //! Fill this DCB structure for Flow Control
    /*!
        \param FlowControl One of enum TFlowControl indicate Flow Control type.
        \sa TFlowControl   
    */
	void SetFlowControl(TFlowControl FlowControl)
	{
		fOutxCtsFlow=(FlowControl==fcHardware); 
		fInX=fOutX=(FlowControl==fcXonXoff); 
		fRtsControl=(FlowControl==fcHardware)?RTS_CONTROL_HANDSHAKE:RTS_CONTROL_ENABLE; 
	}

	TFlowControl GetFlowControl()
	{
		TFlowControl Result = fcNull;
		if(fInX && fOutX) Result = fcXonXoff;
		if(fOutxCtsFlow && fRtsControl == RTS_CONTROL_HANDSHAKE) Result = fcHardware;
		return Result;
	}
};

//! A template Class for Comm-Port
/*!
    A Comm-Port class with policy-mode
    \param p_rd Read Policy
    \param p_wt Write Policy
    
    e.g. TMyComm<BlockRd,BlockWrt> TBlockComm;
    
    Location of p_rd and p_wt can be swaped. 
*/
template<class p_rd, class p_wt> class TMyComm
    :public p_rd, public p_wt
{
private:
	TMyComm(const TMyComm&); 
	TMyComm & operator=(const TMyComm&);
     
private:
	TCHAR FPort[20];  
	TMyDCB dcb; 
	
protected:
	HANDLE hComm; /*!< Handle of Comm-port, created by API CreateFile*/

public:
	TMyComm()   
        :hComm(INVALID_HANDLE_VALUE), p_rd(hComm), p_wt(hComm)
    {
    	ZeroMemory(FPort,sizeof(FPort));
    }
    
    //! Destructor\n
    /*!
        Close Comm-port if it is opened.
    */
	~TMyComm(){
		Close(); 
	}
public:
	// ! Open special port
	/*!
	   \param szPort Points to a null-terminated string that specifies the name of the port. (eg. "COM1").
       \return If the function succeeds, return true. 	   
	*/
	bool Open(LPCTSTR szPort);
	// ! Close opend port
	/*!
       \return If the function succeeds, return true. 	   
	*/
	bool Close();
	
	/*! 
	   \return Return true if port is opened or return false if port is not opened.  	 
    */
	bool Opened() const{
		return hComm!=INVALID_HANDLE_VALUE; 
	}
	
	/*! 
	   \return Return current opened port name.  	 
    */
	LPCTSTR GetPort() const{
		return FPort; 
	}
	
public:
    //! The TransmitCommChar function transmits a specified character ahead of any pending data in the output buffer of the specified communications device.
    /*!
        The TransmitCommChar function is useful for sending an interrupt character (such as a CTRL+C) to a host system.\n 
        If the device is not transmitting, TransmitCommChar cannot be called repeatedly. Once TransmitCommChar places a character in the output buffer, the character must be transmitted before the function can be called again. If the previous character has not yet been sent, TransmitCommChar returns an error.\n 
        Character transmission is subject to normal flow control and handshaking.     
        \param c Specifies the character to be transmitted.
        \return If the function succeeds, return true.
    */ 
	bool TransmitCommChar(char c){
		return Opened()?(::TransmitCommChar(hComm, c)):false; 
	}
	
	//! The EscapeCommFunction function directs a specified communications device to perform an extended function. 
    /*!
	   \param dwFunc Specifies the code of the extended function to perform. This parameter can be one of the following values:
        - SETIR      Sets the serial port to infrared (IR) mode. This value is specific to Windows CE.
        - CLRIR      Sets port to normal serial mode. This value is specific to Windows CE.
        - CLRDTR     Clears the DTR (data-terminal-ready) signal.
        - CLRRTS     Clears the RTS (request-to-send) signal.
        - SETDTR     Sends the DTR (data-terminal-ready) signal.
        - SETRTS     Sends the RTS (request-to-send) signal.
        - SETXOFF    Causes transmission to act as if an XOFF character has been received.
        - SETXON     Causes transmission to act as if an XON character has been received.
        - SETBREAK   Suspends character transmission and places the transmission line in a break state until the EscapeCommFunction is called with the CLRBREAK extended function code. 
        - CLRBREAK   Restores character transmission and places the transmission line in a nonbreak state. 
	   \return If the function succeeds, return true.
	*/
	bool EscapeCommFunction(DWORD dwFunc){
		return Opened()? ::EscapeCommFunction(hComm, dwFunc):false; 
	}
	
	//! Discard all characters from the input buffer of the port
	/*!
	   \return If the function succeeds, return true. 
	*/
	bool ClearRdBuf(){
		return Opened()?(::PurgeComm(hComm, PURGE_RXABORT|PURGE_RXCLEAR)):false; 
	}
	//! Discard all characters from the output buffer of the port
	/*!
	   \return If the function succeeds, return true. 
	*/
	bool ClearWrtBuf(){
		return Opened()? ::PurgeComm(hComm, PURGE_TXABORT|PURGE_TXCLEAR):false; 
	}

public:
    //! Get Handle of opened port
    /*!
        \return Return the Handle of opened port, if port is not opened, return INVALID_HANDLE_VALUE.
    */
	HANDLE GetHandle() const{
		return hComm; 
	}
	
	//! The GetCommModemStatus function retrieves modem control-register values.
	/*!
	   \return Return the 32-bit variable that can be a combination of the following values:
        - MS_CTS_ON    The CTS (clear-to-send) signal is on.
        - MS_DSR_ON    The DSR (data-set-ready) signal is on.  
        - MS_RING_ON   The ring indicator signal is on.
        - MS_RLSD_ON   The RLSD (receive-line-signal-detect) signal is on.
	*/
	DWORD GetCommModemStatus() const{
		if(!Opened()) return 0;  
		DWORD Result=0; 
		::GetCommModemStatus(hComm, &Result); 
		return Result; 
	}
	
	//! Configures the opened port according to the specifications in a device-control block
	/*!
        Configures the opened port according to the specifications in a device-control block (a DCB structure). The function reinitializes all hardware and control settings, but it does not empty output or input queues.
        \param _dcb A DCB structure containing the configuration information for the specified communications device.
        \return If the function succeeds, return true.                      	   
	*/
	bool SetDCB(const DCB&_dcb);
	
	//! Get current control settings(A DCB structure) for the port.
	/*!
	   \return Return current control settings(A DCB structure) for the port.
	   \sa SetDCB(const DCB&_dcb)
	*/
	TMyDCB GetDCB() const{
		return dcb; 
	}
	
	//! Specifies the baud rate at which the communications device operates
	/*!
	   \param BaudRate Can be an actual baud rate value, or one of the following baud rate indexes:
       - CBR_110 	CBR_19200
       - CBR_300 	CBR_38400
       - CBR_600 	CBR_56000
       - CBR_1200	CBR_57600
       - CBR_2400	CBR_115200
       - CBR_4800	CBR_128000
       - CBR_9600	CBR_256000
       - CBR_14400
	   \return If the function succeeds, return true.  
       \sa GetBaudRate()        
	*/
	bool SetBaudRate(DWORD BaudRate);
	
	//! Specifies the parity scheme to be used
	/*!
	   \param Parity Can be one of the following values: 
        - EVENPARITY	Even
        - MARKPARITY	Mark
        - NOPARITY	No parity
        - ODDPARITY	Odd	   
       \return If the function succeeds, return true. 
       \sa GetParity()
	*/
	bool SetParity(BYTE Parity);
	
	//! Specifies the number of bits in the bytes transmitted and received
	/*!
	   \param ByteSize Number of bits/byte, 4~8
	   \return If the function succeeds, return true.
	   \sa GetByteSize(),SetStopBits(BYTE StopBits)
	*/
	bool SetByteSize(BYTE ByteSize);
	
	//! Specifies the number of stop bits to be used
	/*!
	   \param StopBits Can be one of the following values:
       - ONESTOPBIT	1 stop bit
       - ONE5STOPBITS	1.5 stop bits
       - TWOSTOPBITS	2 stop bits
       \return If the function succeeds, return true.
        
       When a DCB structure is used to configure the 8250, the following restrictions apply to the values specified for the ByteSize and StopBits members:
        - The number of data bits must be 5 to 8 bits. 
        - The use of 5 data bits with 2 stop bits is an invalid combination, as is 6, 7, or 8 data bits with 1.5 stop bits.
        
       \sa GetStopBits(),SetByteSize(BYTE ByteSize)                
	*/
	bool SetStopBits(BYTE StopBits);
	
    /*!
        \return Baudrate at which running.
        \sa SetBaudRate(DWORD BaudRate)
    */	
	DWORD GetBaudRate() const{
		return dcb.BaudRate; 
	}
	
	/*!
        \return parity scheme to be used.
        \sa SetParity(BYTE Parity) 
    */
	BYTE GetParity() const{
		return dcb.Parity; 
	}
	
    /*!
        \return The number of bits in the bytes transmitted and received.
        \sa SetByteSize(BYTE ByteSize)
    */
	BYTE GetByteSize() const{
		return dcb.ByteSize; 
	}
	
	/*!
        \return The number of stop bits to be used.
        \sa SetStopBits(BYTE StopBits) 
    */
	BYTE GetStopBits() const{
		return dcb.StopBits; 
	}

public:
    //! Sets the time-out parameters for all read and write operations on the opened port.
    /*!
        \param TimeOuts A COMMTIMEOUTS structure that contains the new time-out values.
        \return If the function succeeds, return true.
        \sa SetTimeouts(DWORD RdItv, DWORD RdMult, DWORD RdConst,DWORD WrtMult, DWORD WrtConst)
    */
	bool SetTimeouts(const COMMTIMEOUTS& TimeOuts);
	
	//! Sets the time-out parameters for all read and write operations on the opened port.
	/*!
	   \param RdItv Specifies the maximum time, in milliseconds, allowed to elapse between the arrival of two characters on the communications line.
       \param RdMult Specifies the multiplier, in milliseconds, used to calculate the total time-out period for read operations. For each read operation, this value is multiplied by the requested number of bytes to be read. 
       \param RdConst Specifies the constant, in milliseconds, used to calculate the total time-out period for read operations. For each read operation, this value is added to the product of the RdMult and the requested number of bytes. 
       \param WrtMult Specifies the multiplier, in milliseconds, used to calculate the total time-out period for write operations. For each write operation, this value is multiplied by the number of bytes to be written. 
       \param WrtConst Specifies the constant, in milliseconds, used to calculate the total time-out period for write operations. For each write operation, this value is added to the product of the WrtMult and the number of bytes to be written.
	   
	   \return If the function succeeds, return true.

        - If a value of MAXDWORD for RdItv, combined with zero values for both the RdMult and RdConst members, specifies that the read operation is to return immediately with the characters that have already been received, even if no characters have been received.\n 	   
        - If a value of zero for both the RdMult and RdConst indicates that total time-outs are not used for read operations.\n
        - If a value of zero for both the WrtMult and WrtConst indicates that total time-outs are not used for write operations.
	*/
	bool SetTimeouts(DWORD RdItv, DWORD RdMult=0, DWORD RdConst=0,
		DWORD WrtMult=0, DWORD WrtConst=0); 
		
	//! Gets the time-out parameters for all read and write operations on the opened port.
	/*!
	   \return The time-out parameters for all read and write operations on the opened port.
	   \sa SetTimeouts(const COMMTIMEOUTS& TimeOuts)
	*/
	COMMTIMEOUTS GetTimeouts() const; 
	//! Specifies input and output buffer size
	/*! 
	    \param In Specifies the recommended size, in bytes, of the device's internal input buffer.
	    \param Out Specifies the recommended size, in bytes, of the device's internal output buffer. 
	    \return If the function succeeds, return true.
	*/
	bool SetBuffers(DWORD In, DWORD Out){
		return Opened()?(::SetupComm(hComm, In, Out)):false; 
	}
};

//---------------------------------------------------------------------------
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::Open(LPCTSTR szPort)
{
	if(Opened())
	{
		if(lstrcmpi(szPort, FPort)==0)
		{
			return true; 
		}
		else
		{
			Close(); 
		}
	}
	lstrcpyn(FPort,szPort,20);
	hComm = CreateFile(FPort, GENERIC_READ | GENERIC_WRITE, 
		0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	if (!Opened()) return false;

	ZeroMemory(&dcb, sizeof(dcb)); 
	if (!GetCommState(hComm, &dcb)){
		Close();
		return false;
	}

	PurgeComm(hComm, PURGE_RXABORT|PURGE_TXABORT); 
	PurgeComm(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);

	if(!p_rd::Open() || !p_wt::Open())
	{
		Close();
		return false;
	}

	return true;
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::Close()
{
	if(Opened())
	{
		p_rd::Close(); 
		p_wt::Close(); 
		CloseHandle(hComm); 
		hComm=INVALID_HANDLE_VALUE;
        ZeroMemory(FPort,sizeof(FPort));
	}
    return true;
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetDCB(const DCB &_dcb)
{
	if(Opened())
    {
    	if(memcmp(&_dcb,&dcb,sizeof(dcb)==0)) return true;
	    DCB tmpDcb=_dcb;
    	if(SetCommState(hComm, &tmpDcb))
	    {
    		dcb=tmpDcb;
        	return true;
	    }
    }
    return false;
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetBaudRate(DWORD BaudRate)
{
	//Micro_DefDCB(BaudRate)
	TMyDCB tmpDcb=dcb;  
    tmpDcb.BaudRate=BaudRate;   
    return SetDCB(tmpDcb);
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetParity(BYTE Parity)
{
	TMyDCB tmpDcb=dcb;
	tmpDcb.Parity=Parity;
	tmpDcb.fParity=Parity?1:0;
	return SetDCB(tmpDcb);
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetByteSize(BYTE ByteSize)
{
	//Micro_DefDCB(ByteSize)
	TMyDCB tmpDcb=dcb;  
    tmpDcb.ByteSize=ByteSize;   
    return SetDCB(tmpDcb);
	
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetStopBits(BYTE StopBits)
{
	//Micro_DefDCB(StopBits)
	TMyDCB tmpDcb=dcb;  
    tmpDcb.StopBits=StopBits;   
    return SetDCB(tmpDcb);
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetTimeouts(const COMMTIMEOUTS& TimeOuts)
{
	if(Opened())
    {
		COMMTIMEOUTS _TimeOuts=TimeOuts;
		bool r = SetCommTimeouts(hComm, &_TimeOuts);
		PurgeComm(hComm, PURGE_RXABORT|PURGE_TXABORT); // When SetTimeouts Change the setting, The Read/Write - Thread often in IO_PENDING, so need Clear to reread.
		return r;
    }
    return false;
}
template<class p_rd, class p_wt>
	bool TMyComm<p_rd, p_wt>::SetTimeouts(
	DWORD RdItv, DWORD RdMult, DWORD RdConst, DWORD WrtMult, DWORD WrtConst)
{
	COMMTIMEOUTS _TimeOuts={RdItv, RdMult, RdConst, WrtMult, WrtConst}; 
	return SetTimeouts(_TimeOuts);
}
template<class p_rd, class p_wt>
	COMMTIMEOUTS TMyComm<p_rd, p_wt>::GetTimeouts() const
{
	COMMTIMEOUTS _TimeOuts={0, 0, 0, 0, 0};
	if(Opened()) GetCommTimeouts(hComm, &_TimeOuts);
	return _TimeOuts; 
}

//! Write Policy for TMyComm, In Block Mode
struct BlockWrt
{
    /*!
        \param _hComm Handle of opened port 
    */
	BlockWrt(HANDLE &_hComm):hComm(_hComm){ZeroMemory(&osWrite, sizeof(osWrite)); }
	
	//! Writes data to opened port
	/*!
	   \param Buf Points to the buffer containing the data to be written to the port.
	   \param Len Specifies the number of bytes to write to the port.
	   \return The return value is the number of bytes actually written.
	*/
	DWORD Write(const void *Buf, DWORD Len){
		if(!Opened()) return 0; 
		DWORD Result=Len; 
		if(Result && !WriteFile(hComm, Buf, Result, &Result, &osWrite))
		{
			Result=0; 
			if (GetLastError() == ERROR_IO_PENDING)
				GetOverlappedResult(hComm, &osWrite, &Result, true); 
		}
		return Result; 
	}
	
	//! clears the buffers for the port and causes all buffered data to be written to the port.
	/*!
        \return If the function succeeds, return true.
	*/
	bool Flush(){
		return Opened()? ::FlushFileBuffers(hComm):false; 
	}

protected:
    //! The Open function is invoked by TMyComm after opened the port  
	bool Open()	{
		if(Opened()) return true; // Already Opened
		ZeroMemory(&osWrite, sizeof(osWrite)); 
		osWrite.hEvent=CreateEvent(NULL, true, false, NULL); 
		return Opened();
	}
	//! The Close function is invoked by TMyComm before close the port
	bool Close(){
		if(!Opened()) return true; // Already Closed
		PurgeComm(hComm, PURGE_TXABORT|PURGE_TXCLEAR); 
		CloseHandle(osWrite.hEvent); 
		osWrite.hEvent=NULL;
		return true;
	}

private:
	HANDLE &hComm; 
	OVERLAPPED osWrite; 
	bool Opened(){
		return osWrite.hEvent!=NULL;
	}
}; 
//! Read Policy for TMyComm, In Block Mode
struct BlockRd{
    /*!
        \param _hComm Handle of opened port 
    */
	BlockRd(HANDLE &_hComm):hComm(_hComm){ZeroMemory(&osRead, sizeof(osRead)); }

	//! Reads a specified number of bytes from opened port
	/*!
	   \param Buf Points to the buffer that receives the data read from the port.
	   \param Len Specifies the number of bytes to be read from the port.
	   \return The return value is the number of bytes actually read.
	*/
	DWORD Read(void *Buf, DWORD Len){
		if(!Opened())return 0; 
		DWORD Result=Len; 
		if (Result && !ReadFile(hComm, Buf, Result, &Result, &osRead))
		{
			Result=0; 
			if (GetLastError() == ERROR_IO_PENDING)
				GetOverlappedResult(hComm, &osRead, &Result, true); 
		}
		return Result; 
	}
protected:	
    //! The Open function is invoked by TMyComm after opened the port  
	bool Open()	{
		if(Opened()) return true; 
		ZeroMemory(&osRead, sizeof(osRead)); 
		osRead.hEvent=CreateEvent(NULL, true, false, NULL); 
		return Opened(); 
	}
	//! The Close function is invoked by TMyComm before close the port
	bool Close() {
		if(!Opened())return true; 
		PurgeComm(hComm, PURGE_RXABORT|PURGE_RXCLEAR); 
		CloseHandle(osRead.hEvent); 
		osRead.hEvent=NULL;
		return true; 
	}
private:
	HANDLE &hComm; 
	OVERLAPPED osRead; 
	bool Opened(){
		return osRead.hEvent!=NULL;
	}
};

# if !defined(__BORLANDC__) || defined(__MT__)

/*! 
    \var typedef void (*TWrtEvent)(HANDLE, void*, DWORD)
    \brief Write Event for Thread Mode Write Policy
    
    \param hComm(HANDLE) Handle of opened port 
    \param Buf(Void*) Points to the buffer containing the data to be written to the port.
    \param Len(DWORD) Len Specifies the number of bytes to write to the port.
*/
typedef void (*TWrtEvent)(HANDLE , void* , DWORD );

//! Memory Allocate Policy for CustomThreadWrt 
struct PolicyDirectAlloc{
    //! Allocate size of memory
	void *Alloc(size_t size){
        return malloc(size);
    }
    
    //! Free memory
    void Free(void *block){
    	free(block);
    }
};

//! Write Policy for TMyComm, with Thread Mode
/*!
    \param AllocPolicy Memory Allocate Policy for Message Queue of Write Thread.
    
    e.g: typedef CostomThreadWrt<PolicyDirectAlloc> DAThreadWrt;
    
*/
template<class AllocPolicy> struct CustomThreadWrt
{
    /*!
        \param _hComm Handle of opened port 
    */
	CustomThreadWrt(HANDLE &_hComm)
    	:hThread(INVALIDTHREAD), OnWritten(NULL),
        WrtPps(_hComm,OnWritten) {}
	
	~CustomThreadWrt(){
		Close(); 
	}

public:
    //! Occurs when data is written to the port.   
	TWrtEvent OnWritten;

    #pragma option push -w-inl
	
	//! Put data to the message queue of the write thread.
	/*!
	   \param Buf Points to the buffer containing the data to be put to the message queue.
	   \param Len Specifies the number of bytes to put to the message queue.
	   \return The return value is the number of bytes actually put.
	*/
	DWORD Write(const void *Buf, DWORD Len)
	{
		if(hThread==INVALIDTHREAD||Len==0)return 0;

		int cx=0;

        if(Len<=8)
		{
			// Direct use Message to transmit the data
			// all data is WPARAM-LPARAM
			TLittleData ld;
			CopyMemory(ld.buf,Buf,Len);
			while(!PostThreadMessage(dwThreadID,WRTM_DATA+Len,ld.wParam,ld.lParam) && ++cx<10) Sleep(1);
		}
		else
		{
			void *NewBuf=WrtPps.Allocator.Alloc(Len);
		    CopyMemory(NewBuf,Buf,Len);
			while(!PostThreadMessage(dwThreadID,WRTM_DATA,Len,(LPARAM)NewBuf) && ++cx<10) Sleep(1);
		}

		return (cx>=10) ? 0 : Len;
	}
	
    #pragma option pop
protected:
    //! The Open function is invoked by TMyComm after opened the port 
	bool Open()
	{
		if(Opened())return true;
        WrtPps.wEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
		hThread=_beginthreadex(0,4096,WrtThreadFunc,&WrtPps,0,&dwThreadID);
		
		if(!Opened()) CloseHandle(WrtPps.wEvent);

		return Opened();
	}
	//! The Close function is invoked by TMyComm before close the port
	bool Close()
	{
		if(!Opened())return true;
        PostThreadMessage(dwThreadID,WRTM_CLOSE,0,0);
        SetEvent(WrtPps.wEvent);
        WaitForSingleObject((HANDLE)hThread,5000);
        CloseHandle(WrtPps.wEvent);
        CloseHandle((HANDLE)hThread);
		hThread=INVALIDTHREAD;
		return true;
	}
private:
	enum {WRTM_DATA=WM_USER+1,WRTM_MAXDATA=WRTM_DATA+8,WRTM_CLOSE};

	union TLittleData{
		struct{
			WPARAM wParam;
			LPARAM lParam;
		};
		char buf[8];
	};

	struct WrtThreadProperties{
    	HANDLE &hComm;
    	TWrtEvent &OnWrt;
	    HANDLE wEvent;              // Overlapped Write Event
        AllocPolicy Allocator;
        WrtThreadProperties(HANDLE &_hComm,TWrtEvent &_OnWrt)
        	:hComm(_hComm),OnWrt(_OnWrt){;}
	}WrtPps;

	unsigned long hThread;
    unsigned dwThreadID;

	bool Opened(){
		return hThread!=INVALIDTHREAD;
	}

	static unsigned __stdcall WrtThreadFunc(void *Properties);
};

template<class AllocPolicy> unsigned __stdcall CustomThreadWrt<AllocPolicy>::WrtThreadFunc(void *Properties)
{
    MSG msg;
	OVERLAPPED osWrite;
    WrtThreadProperties *pps=(WrtThreadProperties*)Properties;

    // force message queue to be created
    ::PeekMessage( &msg, NULL, WM_USER, WM_USER, PM_NOREMOVE );

    ZeroMemory(&osWrite, sizeof(osWrite));
    osWrite.hEvent=pps->wEvent;
    HANDLE &hComm=pps->hComm;

    while(::GetMessage(&msg, NULL, 0, 0) > 0)
    {		
        if(msg.message == WRTM_DATA)
        {
            DWORD Result=0;
            if(msg.wParam>0 && !WriteFile(hComm,(LPCVOID)msg.lParam,msg.wParam,&Result,&osWrite))
            {
                if (GetLastError() == ERROR_IO_PENDING)
                    GetOverlappedResult(hComm, &osWrite, &Result, true);
            }
            if(Result && pps->OnWrt) pps->OnWrt(hComm,(LPVOID)msg.lParam,Result);
            pps->Allocator.Free((void*)msg.lParam);
        }
		else if(msg.message>WRTM_DATA && msg.message<=WRTM_MAXDATA)
		{
			TLittleData ld;
			ld.lParam = msg.lParam;
			ld.wParam = msg.wParam;

            DWORD Result=0;
			if(!WriteFile(hComm,ld.buf,msg.message - WRTM_DATA,&Result,&osWrite))
            {
                if (GetLastError() == ERROR_IO_PENDING)
                    GetOverlappedResult(hComm, &osWrite, &Result, true);
            }
			if(Result && pps->OnWrt) pps->OnWrt(hComm,ld.buf,Result);
		}
        else if(msg.message == WRTM_CLOSE)
            break;
    }

    PurgeComm(hComm, PURGE_TXABORT|PURGE_TXCLEAR);
    //Clear Write Thread
    while(
    	PeekMessage(&msg,0,WRTM_DATA,WRTM_DATA,PM_REMOVE)
        &&
        msg.message==WRTM_DATA
    ) pps->Allocator.Free((void*)msg.lParam);
    
    return 0;
}

typedef CustomThreadWrt<PolicyDirectAlloc> ThreadWrt;

//----------------------------------------------------------

//! Read Event for Thread Mode Read Policy
/*!
    \param hComm(HANDLE) Handle of opened port 
    \param Buf(Void*) Points to the buffer that receives the data read from the port.
    \param Len(DWORD) Len Specifies the number of bytes to be read from the port.
*/
typedef void (*TRdEvent)(HANDLE, void*, DWORD);

//! OnError Event for Thread Mode Read Policy
/*!
    \param hComm(HANDLE) Handle of opened port 
    \param ErrorCode This parameter can be one or more of the following error codes:
    - CE_BREAK	The hardware detected a break condition.
    - CE_DNS	Windows 95 only: A parallel device is not selected.
    - CE_FRAME	The hardware detected a framing error.
    - CE_IOE	An I/O error occurred during communications with the device.
    - CE_MODE	The requested mode is not supported, or the hFile parameter is invalid. If this value is specified, it is the only valid error.
    - CE_OOP	Windows 95 only: A parallel device signaled that it is out of paper.
    - CE_OVERRUN	A character-buffer overrun has occurred. The next character is lost.
    - CE_PTO	Windows 95 only: A time-out occurred on a parallel device.
    - CE_RXOVER	An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.
    - CE_RXPARITY	The hardware detected a parity error.
    - CE_TXFULL	The application tried to transmit a character, but the output buffer was full.
*/
typedef void (*TRdError)(HANDLE, DWORD ErrorCode);

//! OnSignal Event for Thread Mode Read Policy
/*!
    \param EvtMask This parameter can be one or more of the following error codes:
    - EV_BREAK	A break was detected on input.
    - EV_CTS	The CTS (clear-to-send) signal changed state.
    - EV_DSR	The DSR (data-set-ready) signal changed state.
    - EV_ERR	A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
    - EV_RING	A ring indicator was detected.
    - EV_RLSD	The RLSD (receive-line-signal-detect) signal changed state.
    - EV_RXCHAR	A character was received and placed in the input buffer.
    - EV_RXFLAG	The event character was received and placed in the input buffer. The event character is specified in the device's DCB structure, which is applied to a serial port by using the SetCommState function.
    - EV_TXEMPTY	The last character in the output buffer was sent.    
*/
typedef void (*TRdSignal)(HANDLE, DWORD EvtMask);

//! Read Policy for TMyComm, with Thread Mode
/*!
    \param COMMREADBUFFERSIZE Read Buffer of Thread.
    
    e.g. typedef CustomThreadRd<2048> ThreadRd;
*/
template <size_t COMMREADBUFFERSIZE> struct CustomThreadRd
{
    /*!
        \param _hComm Handle of opened port 
    */
	CustomThreadRd(HANDLE &_hComm)
		:hThread(INVALIDTHREAD), OnRead(NULL), OnError(NULL), OnSignal(NULL),
        RdPps(_hComm,OnRead,OnError,OnSignal){;}
        
	~CustomThreadRd(){
		Close();
	}

public:
    //! Occurs when data is read from the port.   
	TRdEvent OnRead;
	//! Occurs when error code is received from the port.   
	TRdError OnError;
	//! Occurs when comm event is received.  
	TRdSignal OnSignal;
    
    //! Do not call Read Function on Thread mode, use OnRead instead.
	DWORD Read(void *Buf, DWORD Len) {
		return 0; 
	}

protected:
    //! The Open function is invoked by TMyComm after opened the port 
	bool Open()
	{
		if(Opened()) return true;
        RdPps.hExit=CreateEvent(NULL, FALSE, FALSE, NULL);
        RdPps.ThreadTerm=false;
		hThread=_beginthreadex(0,4096,RdThreadFunc,&RdPps,0,&dwThreadID);
		if(!Opened()){
			CloseHandle(RdPps.hExit);
		}
		return Opened();

	}
	//! The Close function is invoked by TMyComm before close the port
	bool Close()
	{
		if(!Opened()) return true;
        RdPps.ThreadTerm=true;
        SetEvent(RdPps.hExit);
        WaitForSingleObject((HANDLE)hThread,5000);
        CloseHandle(RdPps.hExit);
        CloseHandle((HANDLE)hThread);
		hThread=INVALIDTHREAD;
		return true;
	}
private:
    struct RdThreadProperties{
        HANDLE &hComm;
        TRdEvent &OnRd;
        TRdError &OnError;
        TRdSignal &OnSignal;
        HANDLE hExit;
        volatile bool ThreadTerm;
        RdThreadProperties(HANDLE &_hComm,TRdEvent &_OnRd,TRdError &_OnErr,TRdSignal &_OnSig)
        	:hComm(_hComm),OnRd(_OnRd),OnError(_OnErr),OnSignal(_OnSig){;}
    }RdPps;

    unsigned long hThread;
    unsigned dwThreadID;

	bool Opened(){
		return hThread!=INVALIDTHREAD;
	}

    static unsigned __stdcall RdThreadFunc(void *Properties);
};

template <size_t COMMREADBUFFERSIZE> unsigned __stdcall CustomThreadRd<COMMREADBUFFERSIZE>::RdThreadFunc(void *Properties)
{
	RdThreadProperties *pps=(RdThreadProperties*)Properties;
    HANDLE &hComm=pps->hComm;
	OVERLAPPED osRead;
	OVERLAPPED osStatus;

    char Buffer[COMMREADBUFFERSIZE];
    bool WaitingRead=false;
    bool WaitingStat=false;
    DWORD Len=0;
    DWORD Evt;

    ZeroMemory(&osRead, sizeof(osRead));
    ZeroMemory(&osStatus, sizeof(osStatus));
    osRead.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
    osStatus.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);

    HANDLE hArray[3]={osStatus.hEvent,osRead.hEvent,pps->hExit};

    if(!SetCommMask(hComm, EV_BREAK|EV_CTS|EV_DSR|EV_ERR|EV_RING|EV_RLSD)) 
		OutputDebugString(_T("error setting communications mask"));

    while(!pps->ThreadTerm)
    {
        if (!WaitingRead)
        {
            if (!ReadFile(hComm, Buffer, COMMREADBUFFERSIZE, &Len, &osRead))
                WaitingRead = (GetLastError() == ERROR_IO_PENDING);
            else if (Len>0 && pps->OnRd)
            	pps->OnRd(hComm, Buffer, Len);
            else
            	Sleep(1);
        }
        if (!WaitingStat)
        {
            if(!WaitCommEvent(hComm, &Evt, &osStatus))
                WaitingStat = (GetLastError() == ERROR_IO_PENDING);
            else if(pps->OnSignal) pps->OnSignal(hComm, Evt);
        }
        if(WaitingRead&&WaitingStat)
        {
            DWORD dwRes = WaitForMultipleObjects(3, hArray, false, INFINITE);
            switch(dwRes)
            {
            case WAIT_OBJECT_0://State
                if(GetOverlappedResult(hComm, &osStatus, &Len, false))
                {
                    if(Evt&EV_ERR)
                    {
                        DWORD ErrorCode=0;
                        if(ClearCommError(hComm, &ErrorCode, NULL)&&pps->OnError)
                            pps->OnError(hComm, ErrorCode);
                    }
                    else if(pps->OnSignal) pps->OnSignal(hComm, Evt);
                }
                WaitingStat=false;
                break;

			case WAIT_OBJECT_0 + 1://Read
                if(GetOverlappedResult(hComm, &osRead, &Len, false))
                {
                    if(Len&&pps->OnRd) pps->OnRd(hComm, Buffer, Len);
                }
                WaitingRead = false;
                break;

			case WAIT_OBJECT_0+2://Exit
                goto EndThread;
            }
        }
    }
EndThread:
	PurgeComm(hComm, PURGE_RXABORT|PURGE_RXCLEAR);
    CloseHandle(osRead.hEvent);
    CloseHandle(osStatus.hEvent);
    return 0;
}
typedef CustomThreadRd<2048> ThreadRd;
#endif

#endif
