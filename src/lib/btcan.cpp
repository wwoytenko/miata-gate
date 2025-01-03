//
// Created by vvoitenko on 29/12/2024.
//

#include "btcan.h"
#include <ArduinoBLE.h>
#include <Arduino_CAN.h>

#ifdef BOARD_DEBUG_MODE
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE
#endif

#include <DebugLog.h>

#define BOARD_CAN_FILTER_DENY_ALL 0x0
#define BOARD_CAN_FILTER_ALLOW_ALL 0x1
#define BOARD_CAN_FILTER_ALLOW_ONE 0x2
#define BOARD_MAX_ALLOWED_PIDS 20


#define BOARD_BLE_CAN_FILTER_MODE_DENY_ALL 0x1
#define BOARD_BLE_CAN_FILTER_MODE_ALLOW_ALL 0x2
#define BOARD_BLE_CAN_FILTER_MODE_ALLOW_ONE 0x4

uint8_t bleCanFilterMode;
uint8_t bleCanFilterPidCount;
PidNotification bleCanFilterAllowedPIDs[BOARD_MAX_ALLOWED_PIDS];

uint16_t bleCanFilterAllowAllInterval = 100;
uint16_t bleLastNotifyTime = 0;


static void onCanBusFilterWritten(BLEDevice device, BLECharacteristic characteristic) {
	uint8_t buffer[8];
	size_t length = characteristic.readValue(buffer, sizeof(buffer));

	if (length < 1) {
		LOG_ERROR("CAN filter: invalid command length: length = ", length);
		return;
	}

	uint8_t command = buffer[0];
	uint32_t pid;
	uint16_t interval;
	switch (command) {
		case BOARD_CAN_FILTER_DENY_ALL:
			bleCanFilterMode = BOARD_BLE_CAN_FILTER_MODE_DENY_ALL;
			bleCanFilterPidCount = 0;
			LOG_INFO("CAN filter: set: deny all");
			break;
		case BOARD_CAN_FILTER_ALLOW_ALL: // Allow all
			bleCanFilterMode = BOARD_BLE_CAN_FILTER_MODE_ALLOW_ALL;
			bleCanFilterPidCount = 0;
			bleCanFilterAllowAllInterval = (buffer[2] << 8) | buffer[1];
			LOG_DEBUG("CAN filter: received allow all: interval =", bleCanFilterAllowAllInterval);
			LOG_INFO("CAN filter: set: allow all");
			break;
		case BOARD_CAN_FILTER_ALLOW_ONE: // Allow one PID
			if (length < 7) {
				LOG_ERROR("CAN filter: invalid allow one pid command");
				return;
			}
			interval = (buffer[2] << 8) | buffer[1];
			pid = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[4];
			if (bleCanFilterPidCount >= BOARD_MAX_ALLOWED_PIDS) {
				LOG_ERROR("CAN filter: Buffer overflow for PIDs");
				LOG_INFO("CAN filter: skip pid=", pid);
				return;
			}
			LOG_DEBUG("CAN filter: received allow one: pid =", pid, " interval =", interval);
			bleCanFilterAllowedPIDs[bleCanFilterPidCount++] = PidNotification{
					pid: pid,
					interval: interval
			};
			bleCanFilterMode = BOARD_BLE_CAN_FILTER_MODE_ALLOW_ONE;
			LOG_INFO("CAN filter: allowed pid =", pid);
			break;
		default:
			LOG_ERROR("CAN filter: unknown command");
			break;
	}
}


void btSetCanFilterEventHandler(BLEUnsignedLongCharacteristic canBusFilterChar) {
	canBusFilterChar.setEventHandler(BLEWritten, onCanBusFilterWritten);
}
