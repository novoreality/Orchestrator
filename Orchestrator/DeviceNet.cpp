#include "MyDeviceNet.h" // These are our own prototypes
#include "RobotSyncController.h"
#include <windows.h>

CHAR* DriverName = "ssdn32.dll";
CHAR* CardName = "DN3-PCI-0000";
DWORD CardHandle;
DWORD ErrorCode;
DNS_STATUS DeviceStatus = { 0 };
DNS_DEVICE_CFG DeviceConf = { 0 };
DNS_SCANNER_CFG ScannerConf = { 0 };
DWORD cardFlags;
BYTE ServiceData[100] = { 0 };

void FDeviceNet::LoadDLL()
{
	
	hGetProcIDDLL = LoadLibrary(TEXT("dnscan32.dll"));

	DNS_LoadDriver = (dnsLoadDriver)GetProcAddress(hGetProcIDDLL, "DNS_LoadDriver");
	DNS_OpenCard = (dnsOpenCard)GetProcAddress(hGetProcIDDLL, "DNS_OpenCard");
	DNS_AddDevice = (dnsAddDevice)GetProcAddress(hGetProcIDDLL, "DNS_AddDevice");
	DNS_StartScan = (dnsStartScan)GetProcAddress(hGetProcIDDLL, "DNS_StartScan");
	DNS_Online = (dnsOnline)GetProcAddress(hGetProcIDDLL, "DNS_Online");
	DNS_SendDeviceExplicit = (dnsSendDeviceExplicit)GetProcAddress(hGetProcIDDLL, "DNS_SendDeviceExplicit");
	DNS_RegisterDeviceEvent = (dnsRegisterDeviceEvent)GetProcAddress(hGetProcIDDLL, "DNS_RegisterDeviceEvent");
	DNS_UnRegisterDeviceEvent = (dnsUnRegisterDeviceEvent)GetProcAddress(hGetProcIDDLL, "DNS_UnRegisterDeviceEvent");
	DNS_RegisterClientEvent = (dnsRegisterClientEvent)GetProcAddress(hGetProcIDDLL, "DNS_RegisterClientEvent");
	DNS_UnRegisterClientEvent = (dnsUnRegisterClientEvent)GetProcAddress(hGetProcIDDLL, "DNS_UnRegisterClientEvent");
	DNS_StopScan = (dnsStopScan)GetProcAddress(hGetProcIDDLL, "DNS_StopScan");
	DNS_Offline = (dnsOffline)GetProcAddress(hGetProcIDDLL, "DNS_Offline");
	DNS_DeleteDevice = (dnsDeleteDevice)GetProcAddress(hGetProcIDDLL, "DNS_DeleteDevice");
	DNS_CloseCard = (dnsCloseCard)GetProcAddress(hGetProcIDDLL, "DNS_CloseCard");
	DNS_FreeDriver = (dnsFreeDriver)GetProcAddress(hGetProcIDDLL, "DNS_FreeDriver");
	DNS_GetDeviceStatus = (dnsGetDeviceStatus)GetProcAddress(hGetProcIDDLL, "DNS_GetDeviceStatus");
	DNS_GetDeviceEvent = (dnsGetDeviceEvent)GetProcAddress(hGetProcIDDLL, "DNS_GetDeviceEvent");
	DNS_ReadDeviceIo = (dnsReadDeviceIo)GetProcAddress(hGetProcIDDLL, "DNS_ReadDeviceIo");
	DNS_ReceiveDeviceExplicit = (dnsReceiveDeviceExplicit)GetProcAddress(hGetProcIDDLL, "DNS_ReceiveDeviceExplicits");
	DNS_IoActive = (dnsIoActive)GetProcAddress(hGetProcIDDLL, "DNS_IoActive");
	DNS_WriteDeviceIo = (dnsWriteDeviceIo)GetProcAddress(hGetProcIDDLL, "DNS_WriteDeviceIo");
}

bool FDeviceNet::CreateConnection(uint8_t MacId)
{

	LoadDLL();

	if (!LoadDriver())
	{
		return false;
	}

	if (!OpenCard())
	{
		return false;
	}

	if (!AddDevice(MacId))
	{
		return false;
	}

	if (!GoOnline())
	{
		return false;
	}

	if (!StartScan())
	{
		return false;
	}

	ClearIo();
	Connected = true;
	return true;
}

bool FDeviceNet::CloseConnection()
{
	DNS_DeleteDevice(CardHandle, DeviceConf.MacId);
	DNS_CloseCard(CardHandle);
	DNS_FreeDriver();
	//FreeLibrary(hGetProcIDDLL);
	return true;
}

bool FDeviceNet::SendData(elementSize(&Data)[ELEMENT_NUMBER])
{
	PreventEStop(Data);
	if (!DNS_WriteDeviceIo(CardHandle, DeviceConf.MacId, DNS_OUTPUT1, Data, sizeof(Data)))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when sending data: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::LoadDriver()
{
	if (!DNS_LoadDriver(DriverName))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when loading driver: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::OpenCard()
{
	// open a card connection
	cardFlags = 0;
	// clear use interrupts flag
	cardFlags &= ~0x01;
	// clear Allow overlap flag
	cardFlags &= ~0x02;

	if (!DNS_OpenCard(&CardHandle, CardName, NULL, cardFlags))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when opening card: %d"), ErrorCode);
		return false;
	}
	return true;
}

//bool FDeviceNet::RegisterDeviceEvent(uint8 MacId)
//{
//	if (!DNS_RegisterDeviceEvent(CardHandle, MacId, DNS_IO1_EVENT, ThreadId, EM_IO_EVENT, 0))
//	{
//		ErrorCode = (uint32)GetLastError();
//		UE_LOG(LogTemp, Error, TEXT("Error when registering device event: %d"), ErrorCode);
//		return false;
//	}
//	return true;
//}

bool FDeviceNet::UnRegisterDeviceEvent()
{
	if (!DNS_UnRegisterDeviceEvent(CardHandle, ScannerConf.MacId, DNS_IO1_EVENT))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when unregistering device event: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::GoOnline() {

	ScannerConf.MacId = 0;
	ScannerConf.BaudRate = DN_500K;
	ScannerConf.Flags = 0;

	if (!DNS_Online(CardHandle, &ScannerConf))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when going online: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::AddDevice(uint8_t MacId) {

	DeviceConf.MacId = MacId;
	DeviceConf.VendorId = 418;
	DeviceConf.DeviceType = 12;
	DeviceConf.ProductCode = 14;
	DeviceConf.Flags = SS_P;
	DeviceConf.Input1Size = 4;
	DeviceConf.Input1Offset = 0x1000;
	DeviceConf.Output1Size = 4;
	DeviceConf.Output1Offset = 0x1004;

	if (!DNS_AddDevice(CardHandle, &DeviceConf))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when adding device: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::StartScan() 
{
	if (!DNS_StartScan(CardHandle))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when starting scan: %d"), ErrorCode);
		return false;
	}
	return true;
}

bool FDeviceNet::ReadData(elementSize (&Buffer)[ELEMENT_NUMBER])
{
	BYTE EventBuffer;
	if (!Connected)
	{
		return false;
	}
	if (!DNS_GetDeviceEvent(CardHandle, DeviceConf.MacId, DNS_IO1_EVENT, &EventBuffer))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when reading device event: %d"), ErrorCode);
		return false;
	}
	if (EventBuffer != DNS_INPUT_DATA_UPDATE)
	{
		return false;
	}
	if (!DNS_ReadDeviceIo(CardHandle, DeviceConf.MacId, DNS_INPUT1, &Buffer, sizeof(Buffer)))
	{
		ErrorCode = GetLastError();
		//UE_LOG(LogTemp, Error, TEXT("Error when reading IO: %d"), ErrorCode);
		return false;
	}
	return true;
}


void FDeviceNet::PreventEStop(elementSize(&Data)[ELEMENT_NUMBER])
{
	Data[0] |= 1 >> (BIT_ESTOP_ROBOT - 1);
}


bool FDeviceNet::ClearIo()
{
	elementSize Data[ELEMENT_NUMBER] = { }; // initialize to all 0's
	return SendData(Data);
}

void FDeviceNet::Cleanup()
{
	ClearIo();
	DNS_StopScan(CardHandle); // stop scannning
	DNS_Offline(CardHandle); // go offline
	DNS_DeleteDevice(CardHandle, ScannerConf.MacId); // remove the device from the scanlist
	DNS_CloseCard(CardHandle); // close the card connection
	DNS_FreeDriver(); // unload the hardware driver DLL
}