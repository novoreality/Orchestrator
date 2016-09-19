#pragma once

#include <map>
#include <cstdint>

#define BIT_ESTOP_ROBOT 1
#define BIT_START_ROBOT 2
#define BIT_STOP_ROBOT 4
#define BIT_ROBOT_READY 8

#define ELEMENT_NUMBER 4
typedef uint8_t elementSize;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos-1)))

enum ERobotEvent {
	StartRobot,
	EndRobot,
	RobotReady,
	NoEvent
};

enum RobotStatus {
	Disconnected,
	Connected,
	Ready
};