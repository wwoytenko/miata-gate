//
// Created by vadim on 1/8/25.
//

#ifndef OB2_REQ_MAP_H
#define OB2_REQ_MAP_H

#include "Arduino.h"
#include <Arduino_CAN.h>

class OBD2Entry {
public:
	OBD2Entry(uint16_t id, uint16_t pid, uint8_t mode, uint16_t interval);

	void increment();

	bool isNeedToUpdate() const;

	// id - is a unit ID in CAN network
	uint16_t id = 0;
	// pid - is a parameter that we are requesting
	uint16_t pid = 0;
	// mode - is a mode that used for parameter requests (support 0x01 and 0x22)
	uint8_t mode = 0;

private:
	// updatedAt - when the entry was updated last time
	uint16_t updatedAt = 0;
	// updatedAt - when the entry was updated last time
	uint16_t interval = 0;
};

class BroadcastEntry {
public:
	BroadcastEntry(const uint16_t pid) : pid(pid) {
	};

	void increment();

	bool isNeedToUpdate() const;

	// pid - is a broadcast pack PID
	uint16_t pid{};

private:
	// sendEachNMessage - divider that shows how frequently we have to send a message
	uint16_t sendEachNMessage = 0;
	// sentMessagesCount - a counter of send messages of this PID
	uint16_t sentMessagesCount = 0;
};


// EntryMap - contains list of ODB2 requests and list of allowed broadcast PIDs
class EntryMap {
public:
	EntryMap(uint8_t obd2Size, OBD2Entry obd2[], uint8_t broadcastSize, BroadcastEntry broadcast[]);

	// getNextObd2Entry - returns next entry to request in OBD. Need to retrun nullptr or smth some flag that we don't
	// have ant. It returns pinter to the entry and increment a counter
	const OBD2Entry *getNextObd2Entry();

	// isCanMsgAllowed - check if PID provided is allowed. If yes it increments the counter
	bool isBroadcastMsgAllowed(uint32_t pid);

private:
	uint8_t obd2EntrySize = 0;
	uint8_t lastObdEntry = 0;
	OBD2Entry *obd2Entries;
	uint8_t broadcastEntrySize = 0;
	BroadcastEntry *broadcastEntries;
};


#endif //OB2_REQ_MAP_H
