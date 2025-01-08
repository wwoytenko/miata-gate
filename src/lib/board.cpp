//
// Created by vvoitenko on 29/12/2024.
//

#include "board.h"
#include <ArduinoBLE.h>
#include <Arduino_CAN.h>
#include "sensors.h"
#include "btcan.h"
#include "helpers.h"
#include "mx5.h"
#include "obd2.h"


/*
 * DEBUG_PID print CAN message with HEX dump for PID and DATA
 *  module - name of module or logical part of a code where it executed
 *  operation - operation that was done or some additional info
 *  msg - CAN message
 * */
#ifdef BOARD_DEBUG_MODE
#define DEBUG_PID(module, operation, msg)                                    \
	do {                                                        \
		LOG_DEBUG(module, ":", operation, "pid =", msg.id);         \
		msg.printTo(Serial);                                    \
		Serial.println();                                       \
	} while (0)
#else
#define DEBUG_PID(module, operation, msg)
#endif

#include "ob2_req_map.h"


Board::Board(uint8_t oilPresPin, uint8_t oilTempPin, float oilTempR, EntryMap &em) : bleService(
        "00001ff8-0000-1000-8000-00805f9b34fb"),
    canBusMainChar(
        BOARD_RACE_CHRONO_BT_MAIN_CHARACTERISTIC_UUID,
        BLERead | BLENotify,
        BOARD_RACE_CHRONO_BT_MAIN_CHARACTERISTIC_PAYLOAD_SIZE,
        false),
    canBusFilterChar(
        BOARD_RACE_CHRONO_BT_CAN_BUS_FILTER_CHARACTERISTIC_UUID,
        BLEWrite
    ), entryMap(em) {
    this->oilPresPin = oilPresPin;
    this->oilTempPin = oilTempPin;
    this->oilTempR = oilTempR;
    // TODO: Though we set a BT handler the settings provided does not impact somehow on the Board logic
    //		 Consider should we implement this logic?
    btSetCanFilterEventHandler(canBusFilterChar);
}


uint16_t Board::init() {
    initCanBus();
    checkSensors();
    initBLE();
    return errors;
}

void Board::handle() {
    obd2Request();
    printError();
    tryInit();
    handleBLE();
    scanCanBusAndSendToRaceChrono();
    requestSensorsAndSendToRaceChrono();
}

void Board::checkSensors() {
    // Try to read analog data
    // Only thermistor read can return error, and since this is two one sensor
    // if any return error we can decide it as an sensor failure
    sensorOilTemp = readTemperature(oilTempPin, oilTempR);
    if (sensorOilTemp == BOSCH_0261230482_ERROR) {
        setError(errors, BOARD_ERROR_SENSOR_READ);
        LOG_WARN("sensor status: failed");
        return;
    }
    clearError(errors, BOARD_ERROR_SENSOR_READ);
    LOG_INFO("sensor status: ok");
}

void Board::initCanBus() {
    if (!CAN.begin(CanBitRate::BR_500k)) {
        setError(errors, BOARD_ERROR_CAN_INIT);
        LOG_WARN("CAN bus status: failed");
        return;
    }
    clearError(errors, BOARD_ERROR_CAN_INIT);
    LOG_INFO("CAN bus: ok");
}

void Board::initBLE() {
    if (!BLE.begin()) {
        LOG_WARN("BT status: failed");
        setError(errors, BOARD_ERROR_BLE_INIT);
        return;
    }
    clearError(errors, BOARD_ERROR_BLE_INIT);

    BLE.setLocalName(BOARD_BLE_SERVICE_NAME);
    BLE.setDeviceName(BOARD_BLE_SERVICE_NAME);
    BLE.setAdvertisedService(bleService);
    bleService.addCharacteristic(canBusMainChar);
    bleService.addCharacteristic(canBusFilterChar);
    BLE.addService(bleService);
    LOG_INFO("BT status: advertising");
    BLE.advertise();
    LOG_INFO("BT status: ready to connect");
}

void Board::tryInit() {
    if (errors == BOARD_ERROR_NONE) {
        return;
    }
    if (hasError(errors, BOARD_ERROR_CAN_INIT)) {
        initCanBus();
    }
    if (hasError(errors, BOARD_ERROR_BLE_INIT)) {
        initBLE();
    }
}

void Board::requestSensorsAndSendToRaceChrono() {
    if (millis() - sinceRequestSensors <= 50) {
        return;
    }
    sensorOilTemp = readTemperature(oilTempPin, oilTempR);
    sensorOilPres = readOilPressure(oilPresPin);

    if (sensorOilTemp == BOSCH_0261230482_ERROR) {
        setError(errors, BOARD_ERROR_SENSOR_READ);
        sinceRequestSensors = millis();
        return;
    }
    LOG_DEBUG("oil temp value: ", sensorOilTemp);
    LOG_DEBUG("oil pres value: ", sensorOilPres);

    // Add this data to CAN data package
    oilTempCanData[0] = (sensorOilTemp >> 8) & 0xFF;
    oilTempCanData[1] = (sensorOilTemp) & 0xFF;
    raceChronoSendCanData(BOARD_OIL_TEMPERATURE_CAN_PID, oilTempCanData, 2);
    oilPresCanData[0] = (int(round(sensorOilPres * 100)) >> 8) & 0xFF;
    oilPresCanData[1] = int(round(sensorOilPres * 100)) & 0xFF;
    raceChronoSendCanData(BOARD_OIL_PRESSURE_CAN_PID, oilPresCanData, 2);
    clearError(errors, BOARD_ERROR_SENSOR_READ);
    sinceRequestSensors = millis();
}

void Board::printBleConnected() {
    if (hasError(errors, BOARD_ERROR_BLE_INIT)) {
        LOG_DEBUG("cannot check connected devices: BLE has error state");
        return;
    }
    BLEDevice central = BLE.central();
    LOG_DEBUG("ble vars: central =", central, " bleStateConnected =", bleStateConnected);
    if (central && !bleStateConnected) {
        bleStateConnected = true;
        bleDeviceCurrent = central.address();
        LOG_DEBUG("BT device connected: ", bleDeviceCurrent);
    } else if (bleStateConnected) {
        bleStateConnected = false;
        bleDeviceCurrent = "";
        LOG_DEBUG("BT device disconnected: ", central.address());
    }
}

void Board::handleBLE() {
    if (millis() - sinceBLEHandle <= BOARD_BLE_HANDLE_INTERVAL) {
        return;
    }
    LOG_DEBUG("handle BLE events");
    BLE.poll();
    printBleConnected();
    sinceBLEHandle = millis();
}

void Board::raceChronoSendCanData(uint32_t pid, const uint8_t *data, uint8_t length) {
    if (!BLE.connected()) {
        LOG_DEBUG("race chrono: no active BT connection");
        return;
    }
    if (hasError(errors, BOARD_ERROR_BLE_INIT)) {
        LOG_DEBUG("race chrono: do not send can data: ble is not inited");
        return;
    }
    LOG_DEBUG("race chrono: send pid", pid);

    raceChronoCanBuf[0] = pid & 0xFF;
    raceChronoCanBuf[1] = (pid >> 8) & 0xFF;
    raceChronoCanBuf[2] = (pid >> 16) & 0xFF;
    raceChronoCanBuf[3] = (pid >> 24) & 0xFF;

    memcpy(raceChronoCanBuf + 4, data, length); // Copy CAN data to buffer

    int status = canBusMainChar.writeValue(raceChronoCanBuf, 4 + length, true);
    if (status <= 0) {
        LOG_WARN("race chrono: error writing CAN data to BT: pid =", pid, " len =", length);
        return;
    }
}

void Board::obd2Request() {
    if (hasError(errors, BOARD_ERROR_CAN_INIT)) {
        LOG_DEBUG("obd request by ident: skip can scanning: can is not inited");
        return;
    }
    const OBD2Entry *entry = entryMap.getNextObd2Entry();
    if (!entry) {
        return;
    }
    canBusReqBuf[0] = BOARD_OBD2_REQUEST_BY_IDENT_LENGTH;
    canBusReqBuf[1] = entry->mode;
    canBusReqBuf[2] = uint8_t(entry->pid & 0xFF);
    canBusReqBuf[3] = uint8_t((entry->pid >> 8) & 0xFF);

    // Perform functional OBD2 request
    CanMsg const request(entry->id, sizeof(canBusReqBuf), canBusReqBuf);
    const int rc = CAN.write(request);
    if (rc < 0) {
        LOG_ERROR("obd request by ident: error writing: pid =", entry->id);
    }
}

void Board::scanCanBusAndSendToRaceChrono() {
    if (!CAN.available()) {
        LOG_DEBUG("can scanner: can is empty");
        return;
    }
    CanMsg const msg = CAN.read();
    DEBUG_PID("can scanner", "sniffed package", msg);
    // There is dummy switch mapping instead of Map of pid usage, though it might be a better idea instead of hardcode.
    // Though it is not as fast as hardcoding

    if (msg.data[1] == (BOARD_OBD2_RESPONSE_MARKER | BOARD_OBD2_REQUEST_MODE_SHOW_CURRENT_DATA)) {
        raceChronoSendCanData(msg.data[2], &msg.data[3], msg.data_length - 2);
    } else if (msg.data[1] == (BOARD_OBD2_RESPONSE_MARKER | BOARD_OBD2_REQUEST_MODE_READ_DATA_BY_IDENTIFIER)) {
        // For req by ident it is likely 2 bytes PID
        uint16_t pid = (uint16_t(canBusReqBuf[3]) << 8) | uint16_t(canBusReqBuf[2]);
        raceChronoSendCanData(pid, &msg.data[4], msg.data_length - 3);
    } else if (entryMap.isBroadcastMsgAllowed(msg.id)) {
        raceChronoSendCanData(msg.id, msg.data, msg.data_length);
    } else {
        DEBUG_PID("can scanner", "skipped pid", msg);
    }
}

void Board::printError() {
    if (sinceErrorPrinted - millis() < BOARD_ERROR_PRINT_INTERVAL) {
        return;
    }
    if (hasError(errors, BOSCH_0261230482_ERROR)) {
        LOG_ERROR("oil temperature read error: sensor disconnected");
        LOG_ERROR("oil pressure read error: sensor disconnected");
    }
    sinceErrorPrinted = millis();
}
