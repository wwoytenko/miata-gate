//
// Created by vvoitenko on 30/12/2024.
//

#ifndef MIATA_GATE_CAN_BUFFER_H
#define MIATA_GATE_CAN_BUFFER_H

#include <Arduino.h>

#define BOARD_CAN_BUFFER_SIZE 20
#define BOARD_PID_BITMAP_SIZE ((1 << 11) / 8)


struct CanMessage {
	bool updated;
	uint16_t storeCounter;
	uint32_t pid;
	uint8_t data[8];
	uint8_t length;
};

class CanBuffer {
public:
	CanBuffer();

//	bool HasPid(uint16_t canId);

	bool Append(uint32_t canId, const uint8_t (&data)[8], uint8_t length);

	void Clear();

	// Iterator
	CanMessage *begin();

	CanMessage *end();

	uint8_t length();

private:
	int itemCount;
	uint8_t pidBitMap[BOARD_PID_BITMAP_SIZE]{};
	CanMessage buf[BOARD_CAN_BUFFER_SIZE]{};

	void bitMapSet(uint32_t canId);

	void bitMapClear(uint32_t canId);

	bool bitMapIsSet(uint32_t canId);

	int findIndexByPid(uint32_t canId);

};

#endif //MIATA_GATE_CAN_BUFFER_H
