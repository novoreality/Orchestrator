#include "MyDeviceNet.h"
#include "RobotSyncController.h"

// Map from robot name to MacId. To be replaced later by a config file.
std::map<const char*, uint8_t> RobotIdMappings = { { "NovoOne", 10 } };

bool FRobotSyncController::Init(const char* RobotId)
{
	RobotConn = new FDeviceNet(); // Eventually may want to make this configurable
	uint8_t MacId = 10;
	if (!RobotConn->CreateConnection(MacId)) {
		return false;
	};
	return true;
};

void FRobotSyncController::DataReceived(elementSize Data[ELEMENT_NUMBER])
{
	ERobotEvent Event = DecodeEvent(Data);
	HandleEvent(Event);
};

bool FRobotSyncController::StartRobot()
{
	elementSize Data[ELEMENT_NUMBER];
	EncodeEvent(ERobotEvent::StartRobot, Data);
	return RobotConn->SendData(Data);
};

void FRobotSyncController::EncodeEvent(ERobotEvent Event, elementSize Data[ELEMENT_NUMBER])
{
	switch (Event) {
		case ERobotEvent::StartRobot :
			Data[0] |= 1 << (BIT_START_ROBOT - 1);
			break;
		case ERobotEvent::EndRobot :
			Data[0] |= 1 << (BIT_STOP_ROBOT - 1);
			break;
	}
};

ERobotEvent FRobotSyncController::DecodeEvent(elementSize Data[ELEMENT_NUMBER])
{
	elementSize FirstByte = Data[0];
	if (CHECK_BIT(FirstByte, BIT_ROBOT_READY))
	{
		return ERobotEvent::RobotReady;
	}
	return ERobotEvent::NoEvent;
};

void FRobotSyncController::HandleEvent(ERobotEvent Event)
{
	switch (Event) {
		case ERobotEvent::RobotReady :
			OnRobotReady();
			break;
		case ERobotEvent::NoEvent :
			SetIsReady(false);
			break;
	}
};

void FRobotSyncController::Tick()
{
	elementSize Data[ELEMENT_NUMBER];
	if (RobotConn && RobotConn->ReadData(Data))
	{
		DataReceived(Data);
	};
	
}

void FRobotSyncController::OnRobotReady()
{
	bIsReady = true;
}

bool FRobotSyncController::IsReady()
{
	return bIsReady;
}

void FRobotSyncController::SetIsReady(bool IsReady)
{
	bIsReady = IsReady;
}

void FRobotSyncController::ClearIo()
{
	RobotConn->ClearIo();
}


