//
// Created by vvoitenko on 29/12/2024.
//

#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_CAN.h>
#include "can_buffer.h"

#define BOARD_ERROR_NONE 0x0
#define BOARD_ERROR_CAN_INIT 0x1
#define BOARD_ERROR_SENSOR_READ 0x2
#define BOARD_ERROR_BLE_INIT 0x4

#define BOARD_BLE_SERVICE_NAME "miata-gate"

#define BOARD_BLE_HANDLE_INTERVAL 1000
//#define BOARD_RACE_CHRONO_SYNC_INTERVAL 30
#define BOARD_REQUEST_ODB_INTERVAL 100

#define BOARD_OIL_TEMPERATURE_CAN_PID 0x6C
#define BOARD_OIL_PRESSURE_CAN_PID 0x6A

class Board {
private:
	BLEService bleService;
	BLECharacteristic canBusMainChar;
	BLEUnsignedLongCharacteristic canBusFilterChar;

	uint16_t errors{};
	bool bleStateConnected{};
	String bleDeviceCurrent{};
	uint8_t raceChronoCanBuf[20]{};
	uint8_t oilPresCanData[8]{};
	uint8_t oilTempCanData[8]{};
	uint8_t obdRequestMax;
	uint8_t obdRequestCurr{};

	uint16_t mx5VendorRpmSpeedThrottleCounter{};
	uint16_t mx5VendorBrakeSwitchCounter{};
	uint16_t mx5VendorGearBoxCounter{};
	uint16_t mx5VendorEngineRelatedCounter{};
	uint16_t mx5VendorCoolantCounter{};
	uint16_t mx5VendorWheelsSpeedCounter{};

	uint8_t oilPresPin;
	uint8_t oilTempPin;
	float oilTempR;
	int sensorOilTemp{};
	float sensorOilPres{};

	unsigned long sinceBLEHandle{};
	unsigned long sinceObdReq{};
	unsigned long sinceRequestSensors{};


	void initCanBus();

	void checkSensors();

	void initBLE();

	void tryInit();

	void requestSensorsAndSendToRaceChrono();

	void printBleConnected();

	void handleBLE();

	void raceChronoSendCanData(uint32_t pid, const uint8_t data[8], uint8_t length);

	void requestStandardOBDData();

	void scanCanBusAndSendToRaceChrono();


public:
	Board(uint8_t oilPresPin, uint8_t oilTempPin, float oilTempR);

	uint16_t init();

	void handle();
};


#endif //BOARD_H
