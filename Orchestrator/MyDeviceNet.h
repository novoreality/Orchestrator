
#pragma once

#include <windows.h>
#include "DeviceNet.h" // This is the header file distributed from Molex
#include "RobotDefines.h"

class FRobotSyncController;

#define EM_END_THREAD WM_USER+2
#define EM_IO_EVENT WM_USER+3

static HMODULE hGetProcIDDLL;

static dnsLoadDriver DNS_LoadDriver;
static dnsOpenCard DNS_OpenCard;
static dnsAddDevice DNS_AddDevice;
static dnsStartScan DNS_StartScan;
static dnsOnline DNS_Online;
static dnsSendDeviceExplicit DNS_SendDeviceExplicit;
static dnsRegisterDeviceEvent DNS_RegisterDeviceEvent;
static dnsUnRegisterDeviceEvent DNS_UnRegisterDeviceEvent;
static dnsRegisterClientEvent DNS_RegisterClientEvent;
static dnsUnRegisterClientEvent DNS_UnRegisterClientEvent;
static dnsStopScan DNS_StopScan;
static dnsOffline DNS_Offline;
static dnsDeleteDevice DNS_DeleteDevice;
static dnsCloseCard DNS_CloseCard;
static dnsFreeDriver DNS_FreeDriver;
static dnsGetDeviceStatus DNS_GetDeviceStatus;
static dnsGetDeviceEvent DNS_GetDeviceEvent;
static dnsReadDeviceIo DNS_ReadDeviceIo;
static dnsReceiveDeviceExplicit DNS_ReceiveDeviceExplicit;
static dnsIoActive DNS_IoActive;
static dnsWriteDeviceIo DNS_WriteDeviceIo;

class FDeviceNet
{
public:
	bool CreateConnection(uint8_t MacId);
	bool SendData(elementSize(&Data)[ELEMENT_NUMBER]);
	bool CloseConnection();
	bool ReadData(elementSize(&Buffer)[ELEMENT_NUMBER]);
	void Cleanup();
	bool ClearIo();
private:
	void LoadDLL();
	bool LoadDriver();
	bool OpenCard();
	bool GoOnline();
	bool AddDevice(uint8_t MacId);
	bool StartScan();
	bool UnRegisterDeviceEvent();
	void PreventEStop(elementSize(&Data)[ELEMENT_NUMBER]);

	bool Connected;
};