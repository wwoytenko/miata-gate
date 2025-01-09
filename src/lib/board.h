//
// Created by vvoitenko on 29/12/2024.
//

#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "ob2_req_map.h"

#ifdef BOARD_DEBUG_MODE
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE
#endif

#include <DebugLog.h>

#define BOARD_ERROR_NONE 0x0
#define BOARD_ERROR_CAN_INIT 0x1
#define BOARD_ERROR_SENSOR_READ 0x2
#define BOARD_ERROR_BLE_INIT 0x4

#define BOARD_BLE_SERVICE_NAME "miata-gate"

#define BOARD_BLE_HANDLE_INTERVAL 1000
//#define BOARD_RACE_CHRONO_SYNC_INTERVAL 30
#define BOARD_FUNCTIONAL_REQUEST_ODB_INTERVAL 50
#define BOARD_REQUEST_BY_IDENT_ODB_INTERVAL 100

#define BOARD_OIL_TEMPERATURE_CAN_PID 0x6C
#define BOARD_OIL_PRESSURE_CAN_PID 0x6A

#define BOARD_OBD2_FUNCTIONAL_REQUEST_LENGTH 0x02
#define BOARD_OBD2_REQUEST_BY_IDENT_LENGTH 0x06

// broadcast request in can network
#define BOARD_OBD2_FUNCTIONAL_REQUEST_CAN_ID 0x7DF

#define BOARD_OBD2_ECU_REQUEST_CAN_ID 0x7E0
#define BOARD_OBD2_ECU_RESPONSE_CAN_ID 0x7E8

#define BOARD_OBD2_RESPONSE_MARKER 0x40

#define BOARD_OBD2_REQUEST_MODE_SHOW_CURRENT_DATA 0x1
#define BOARD_OBD2_REQUEST_MODE_READ_DATA_BY_IDENTIFIER 0x22

#define BOARD_RACE_CHRONO_BT_MAIN_CHARACTERISTIC_UUID "00000001-0000-1000-8000-00805f9b34fb"
#define BOARD_RACE_CHRONO_BT_MAIN_CHARACTERISTIC_PAYLOAD_SIZE 20
#define BOARD_RACE_CHRONO_BT_CAN_BUS_FILTER_CHARACTERISTIC_UUID "00000002-0000-1000-8000-00805f9b34fb"
#define BOARD_ERROR_PRINT_INTERVAL 1000

class Board {
    BLEService bleService;
    BLECharacteristic canBusMainChar;
    BLEUnsignedLongCharacteristic canBusFilterChar;

    uint16_t errors{};
    bool bleStateConnected{};
    String bleDeviceCurrent{};
    uint8_t raceChronoCanBuf[20]{};
    uint8_t oilPresCanData[8]{};
    uint8_t oilTempCanData[8]{};

    uint16_t mx5VendorRpmSpeedThrottleCounter{};
    uint16_t mx5VendorBrakeSwitchCounter{};
    uint16_t mx5VendorGearBoxCounter{};
    uint16_t mx5VendorEngineRelatedCounter{};
    uint16_t mx5VendorCoolantCounter{};
    uint16_t mx5VendorWheelsSpeedCounter{};
    uint8_t canBusReqBuf[8]{};
    EntryMap &entryMap;

    uint8_t oilPresPin;
    uint8_t oilTempPin;
    float oilTempR;
    int sensorOilTemp{};
    float sensorOilPres{};

    unsigned long sinceBLEHandle{};
    unsigned long sinceRequestSensors{};
    unsigned long sinceErrorPrinted{};


    void initCanBus();

    void checkSensors();

    void initBLE();

    void tryInit();

    void requestSensorsAndSendToRaceChrono();

    void printBleConnected();

    void handleBLE();

    void raceChronoSendCanData(uint32_t pid, const uint8_t data[8], uint8_t length);

    void scanCanBusAndSendToRaceChrono();

    void printError();

    void obd2Request();

public:
    Board(uint8_t oilPresPin, uint8_t oilTempPin, float oilTempR, EntryMap &entryMap);

    uint16_t init();

    void handle();
};


#endif //BOARD_H
