#pragma once

#include "RobotDefines.h"

class FDeviceNet;

class FRobotSyncController
{
public:
	bool Init(const char* RobotId);
	void DataReceived(elementSize Data[ELEMENT_NUMBER]);
	bool StartRobot();
	void Tick();
	bool IsReady();
	void SetIsReady(bool IsReady);
	void ClearIo();

private:
	void EncodeEvent(ERobotEvent Event, elementSize Data[ELEMENT_NUMBER]);
	ERobotEvent DecodeEvent(elementSize Data[ELEMENT_NUMBER]);
	void HandleEvent(ERobotEvent Event);
	void OnRobotReady();

	FDeviceNet *RobotConn;
	bool bIsReady = false;
};
