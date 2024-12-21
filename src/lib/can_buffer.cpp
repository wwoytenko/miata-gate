//
// Created by vvoitenko on 30/12/2024.
//

#include "can_buffer.h"
#include <DebugLog.h>

CanBuffer::CanBuffer() : itemCount(0) {}

void CanBuffer::bitMapSet(uint32_t canId) {
	uint16_t byteIndex = canId / 8;
	if (byteIndex >= BOARD_PID_BITMAP_SIZE) {
		return;
	}
	uint8_t bitOffset = canId % 8;
	pidBitMap[byteIndex] |= (1 << bitOffset);
}

void CanBuffer::bitMapClear(uint32_t canId) {
	uint16_t byteIndex = canId / 8;
	if (byteIndex >= BOARD_PID_BITMAP_SIZE) {
		return;
	}
	uint8_t bitOffset = canId % 8;
	pidBitMap[byteIndex] &= ~(1 << bitOffset);
}

bool CanBuffer::bitMapIsSet(uint32_t canId) {
	uint16_t byteIndex = canId / 8;
	if (byteIndex >= BOARD_PID_BITMAP_SIZE) {
		return false;
	}
	uint8_t bitOffset = canId % 8;
	return pidBitMap[byteIndex] & (1 << bitOffset);
}

bool CanBuffer::Append(uint32_t pid, const uint8_t (&data)[8], uint8_t length) {
	if (pid >= BOARD_PID_BITMAP_SIZE * 8) {
		LOG_DEBUG("CanBuffer: skip pid: out of the lim: pid =", pid);
		// In case PID is higher than 11 bytes
		return false;
	}
	LOG_DEBUG("CanBuffer: itemCount =", itemCount);
	int idx = itemCount;
	if (bitMapIsSet(pid)) {
		// PID already exists, find its index
		LOG_DEBUG("CanBuffer: PID exists: pid =", pid);
		idx = findIndexByPid(pid);
	} else if (itemCount < BOARD_CAN_BUFFER_SIZE) {
		LOG_DEBUG("CanBuffer: PID does not exist: pid =", pid);
		// PID does not exist, use the next available index
		idx = itemCount++;
	} else {
		LOG_DEBUG("CanBuffer: buffer overflow: pid =", pid);
		// Buffer is full and PID does not exist
		return false;
	}
	LOG_DEBUG("CanBuffer: adding value: idx =", idx, "itemCount =", itemCount);
	bitMapSet(pid);
	buf[idx].pid = pid;
	memcpy(buf[idx].data, data, length);
	buf[idx].length = length;

	return true;
}

void CanBuffer::Clear() {
	for (uint8_t &i: pidBitMap) {
		i = 0;
	}
	itemCount = 0;
}

CanMessage *CanBuffer::begin() {
	return buf;
}

CanMessage *CanBuffer::end() {
	return buf + itemCount;
}


int CanBuffer::findIndexByPid(uint32_t canId) {
	for (int i = 0; i < itemCount; ++i) {
		if (buf[i].pid == canId) {
			return i;
		}
	}
	return -1;
}

uint8_t CanBuffer::length() {
	return itemCount;
}

