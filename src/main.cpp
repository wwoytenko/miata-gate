#include <Arduino.h>
#include "lib/sensors.h"
#include <WiFiS3.h>
#include "lib/board.h"

#define BOARD_DEBUG_MODE
#ifdef BOARD_DEBUG_MODE
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE
#endif

#include <DebugLog.h>

#define SERIAL_BAUD 115200

const int OIL_PRESSURE_PIN = A3;
const int OIL_THERMISTOR_PIN = A0;
const float OIL_THERMISTOR_R = 5060.0;

Board board(
		OIL_PRESSURE_PIN,
		OIL_THERMISTOR_PIN,
		OIL_THERMISTOR_R
);

void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial) {
		// TODO: Blink LED in this case
	}

	uint16_t errors = board.init();
	if (errors == 0) {
		LOG_INFO("system status: initialized successfully");
	} else {
		LOG_WARN("system status: initialized with errors");
	}
}


void loop() {
	board.handle();
	delayMicroseconds(50);
}
