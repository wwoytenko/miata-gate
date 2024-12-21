#include "sensors.h"
#include "helpers.h"

#include <Arduino.h>

const int ANALOG_READ_BUF_SIZE = 20;
const float BOSCH_0261230482_PRESSURE_V_MIN = 0.5;
const float BOSCH_0261230482_PRESSURE_V_MAX = 4.5;
const float BOSCH_0261230482_PRESSURE_P_MIN = 0.0;
const float BOSCH_0261230482_PRESSURE_P_MAX = 10.0;

int analogReadBuf[ANALOG_READ_BUF_SIZE];

const int NUM_POINTS = 37;
const int16_t BOSCH_0261230482_TEMPERATURE_TABLE[NUM_POINTS] = {
		-40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90,
		95,
		100, 105, 110, 115, 120, 125, 130, 135, 140
};

const float BOSCH_0261230482_RESISTANCE_TABLE[NUM_POINTS] = {
		44864, 33676, 25524, 19525, 15067, 11724, 9195, 7266, 5784, 4636, 3740, 3037, 2480, 2038, 1683, 1398, 1167,
		978.9,
		825, 698.5, 594, 507.2, 434.9, 374.3, 323.4, 280.4, 244, 213, 186.6, 164, 144.5, 127.8, 113.3, 100.7, 89.8,
		80.2,
		71.9
};

int analogReadWithMedian(uint8_t pin) {
	for (int &i: analogReadBuf) {
		i = analogRead(pin);
	}
	return calculateMedian(analogReadBuf, ANALOG_READ_BUF_SIZE);
}


int readTemperature(const uint8_t pin, const float r) {
	const int rawValue = analogReadWithMedian(pin);
	return convertToTemperature(rawValue, r);
}

int calculateMedian(int *buf, const int size) {
	bubbleSort(buf, size);
	if (size % 2 == 0) {
		return (buf[size / 2 - 1] + buf[size / 2]) / 2;
	}
	return buf[size / 2];
}

float getTemperature(float measuredResistance) {
	for (int i = 0; i < NUM_POINTS - 1; i++) {
		if (measuredResistance <= BOSCH_0261230482_RESISTANCE_TABLE[i] && measuredResistance >=
																		  BOSCH_0261230482_RESISTANCE_TABLE[i + 1]) {
			// Linear interpolation
			float t1 = BOSCH_0261230482_TEMPERATURE_TABLE[i];
			float t2 = BOSCH_0261230482_TEMPERATURE_TABLE[i + 1];
			float r1 = BOSCH_0261230482_RESISTANCE_TABLE[i];
			float r2 = BOSCH_0261230482_RESISTANCE_TABLE[i + 1];

			return t1 + ((measuredResistance - r1) * (t2 - t1)) / (r2 - r1);
		}
	}
	return BOSCH_0261230482_ERROR;
}


int convertToTemperature(int rawValue, float r, float vcc) {
	const float outVoltage = convertToVoltage(rawValue);

	if (outVoltage <= 0.0) {
		return -273;
	}
	float thermistorResistance = r * (outVoltage / (vcc - outVoltage));
	return int(getTemperature(thermistorResistance));
}

int convertToTemperature(const int rawValue, const float r) {
	return convertToTemperature(rawValue, r, VCC);
}


float readOilPressure(const uint8_t pin, float vMin, float vMax, float pMin, float pMax) {
	const int rawValue = analogReadWithMedian(pin);
	const float outVoltage = convertToVoltage(rawValue);
	return pMin + ((outVoltage - vMin) / (vMax - vMin)) * (pMax - pMin);
}


float readOilPressure(const uint8_t pin) {
	float res = readOilPressure(
			pin,
			BOSCH_0261230482_PRESSURE_V_MIN,
			BOSCH_0261230482_PRESSURE_V_MAX,
			BOSCH_0261230482_PRESSURE_P_MIN,
			BOSCH_0261230482_PRESSURE_P_MAX
	);
	if (res < 0) {
		return 0.0;
	}
	return res;
}
