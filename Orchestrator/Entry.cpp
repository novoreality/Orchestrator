#include "RobotSyncController.h"
#include "VRSyncController.h"
#include <iostream>


void main()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	FRobotSyncController *RobotSyncController = new FRobotSyncController();
	const char* Name = "NovoOne";
	RobotSyncController->Init(Name);

	VrSyncController *VrSyncControllerInst = new VrSyncController("127.0.0.1", "27015");

	while (1)
	{
		VrSyncControllerInst->Tick();
		RobotSyncController->Tick();

		if (VrSyncControllerInst->IsReady() && RobotSyncController->IsReady())
		{
			RobotSyncController->StartRobot();
			VrSyncControllerInst->StartVr();

			Sleep(250); // You need this before clearing so the robot has time to process the ready signal

			RobotSyncController->SetIsReady(false);
			VrSyncControllerInst->SetIsReady(false);

			RobotSyncController->ClearIo();
		}

		Sleep(1000);
	}
}