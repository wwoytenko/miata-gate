//
// Created by vvoitenko on 29/12/2024.
//

#ifndef MIATA_GATE_BTCAN_H
#define MIATA_GATE_BTCAN_H

#include <Arduino.h>
#include <ArduinoBLE.h>

struct PidNotification {
	uint32_t pid;
	uint16_t interval;
	uint16_t lastNotifyTime;
};


void btSetCanFilterEventHandler(BLEUnsignedLongCharacteristic canBusFilterChar);

#endif //MIATA_GATE_BTCAN_H
