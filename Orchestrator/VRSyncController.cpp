#include "VRSyncController.h"

VrSyncController::VrSyncController(PCSTR Host, PCSTR Port)
{
	VrConn = new SocketWrapper(Host, Port);
};

void VrSyncController::DataReceived(char(&Buffer)[DEFAULT_BUFLEN])
{
	EVrEvent Event = DecodeEvent(Buffer);
	HandleEvent(Event);
};

bool VrSyncController::StartVr()
{
	char Buffer[DEFAULT_BUFLEN];
	EncodeEvent(EVrEvent::StartVr, Buffer);
	return VrConn->SendData(Buffer);
};

void VrSyncController::EncodeEvent(EVrEvent Event, char(&Buffer)[DEFAULT_BUFLEN])
{
	switch (Event) {
	case EVrEvent::StartVr:
		strncpy(Buffer, "start", sizeof(Buffer));
		break;
	case EVrEvent::EndVr:
		strncpy(Buffer, "end", sizeof(Buffer));
		break;
	}
};

EVrEvent VrSyncController::DecodeEvent(char(&Buffer)[DEFAULT_BUFLEN])
{			
	
	if (strcmp(Buffer, "ready") == 0)
	{
		return EVrEvent::VrReady;
	}
	return EVrEvent::VrNoEvent;
};

void VrSyncController::HandleEvent(EVrEvent Event)
{
	switch (Event) {
		case EVrEvent::VrReady:
			OnVrReady();
			break;
	}
};

void VrSyncController::Tick()
{
	char Buffer[DEFAULT_BUFLEN];
	if (VrConn && VrConn->ReceiveData(Buffer))
	{
		DataReceived(Buffer);
	};

}

void VrSyncController::OnVrReady()
{
	bIsReady = true;
}

bool VrSyncController::IsReady()
{
	return bIsReady;
}

void VrSyncController::SetIsReady(bool IsReady)
{
	bIsReady = IsReady;
}
