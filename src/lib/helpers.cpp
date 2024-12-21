#include <Arduino.h>
#include "helpers.h"

#define VCC_MAX 5.0
#define ADC_MAX 1024


void bubbleSort(int *arr, int size) {
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

float convertToVoltage(const int rawValue, const float vcc, const int adcMax) {
	return rawValue * (vcc / adcMax);
}

float convertToVoltage(const int rawValue) {
	return convertToVoltage(rawValue, VCC_MAX, ADC_MAX);
}


void setBit(uint16_t &state, uint16_t error) {
	state |= error;
}

void clearBit(uint16_t &state, uint16_t error) {
	state &= ~error;
}

bool hasBit(uint16_t state, uint16_t error) {
	return state & error;
}

void setError(uint16_t &state, uint16_t error) {
	setBit(state, error);
}

void clearError(uint16_t &state, uint16_t error) {
	clearBit(state, error);
}

bool hasError(uint16_t state, uint16_t error) {
	return hasBit(state, error);
}