#pragma once

#include <string.h>
#include "VrDefines.h"
#include "SocketWrapper.h"

enum EVrEvent {
	StartVr,
	EndVr,
	VrReady,
	VrNoEvent
};


class VrSyncController
{
public:
	VrSyncController(PCSTR Host, PCSTR Port);
	void DataReceived(char (&Buffer)[DEFAULT_BUFLEN]);
	bool StartVr();
	void Tick();
	bool IsReady();
	void SetIsReady(bool IsReady);

private:
	void EncodeEvent(EVrEvent Event, char(&Buffer)[DEFAULT_BUFLEN]);
	EVrEvent DecodeEvent(char(&Buffer)[DEFAULT_BUFLEN]);
	void HandleEvent(EVrEvent Event);
	void OnVrReady();

	SocketWrapper *VrConn;
	bool bIsReady = false;
};