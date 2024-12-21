#ifndef SENSORS_H
#define SENSORS_H

#include <cstdint>

#define BOSCH_0261230482_ERROR (-999)

int calculateMedian(int *buf, int size);

int readTemperature(const uint8_t pin, const float r);

float readOilPressure(const uint8_t pin, float vMin, float vMax, float pMin, float pMax);

float readOilPressure(const uint8_t pin);

int convertToTemperature(int rawValue, float r);

int convertToTemperature(int rawValue, float r, float vcc);

#endif //SENSORS_H
