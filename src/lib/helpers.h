#ifndef HELPERS_H
#define HELPERS_H

#define VCC 5.0
#define ADC_MAX 1023

void bubbleSort(int *arr, int size);

float convertToVoltage(int rawValue, float vcc, int adcMax);

float convertToVoltage(int rawValue);


void setBit(uint16_t &state, uint16_t error);

void clearBit(uint16_t &state, uint16_t error);

bool hasBit(uint16_t state, uint16_t error);

void setError(uint16_t &state, uint16_t error);

void clearError(uint16_t &state, uint16_t error);

bool hasError(uint16_t state, uint16_t error);

#endif
