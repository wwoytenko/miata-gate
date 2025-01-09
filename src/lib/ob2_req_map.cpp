//
// Created by vadim on 1/8/25.
//

#include "ob2_req_map.h"

#ifdef BOARD_DEBUG_MODE
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE
#endif

#include <DebugLog.h>

/************* OBD2Entry *********************/

OBD2Entry::OBD2Entry(const uint32_t id, const uint32_t pid, const uint8_t mode,
					 const uint16_t interval, uint8_t len) : id(id), pid(pid), mode(mode),
															 interval(interval), length(len) {
}


void OBD2Entry::increment() {
	updatedAt = millis();
}


bool OBD2Entry::isNeedToUpdate() const {
	return (millis() - updatedAt) >= interval;
}

/************* OBD2Entry *********************/

/************* BroadcastEntry *********************/

void BroadcastEntry::increment() {
	sentMessagesCount++;
}

bool BroadcastEntry::isNeedToUpdate() const {
	return sentMessagesCount % sendEachNMessage == 0;
}

/************* BroadcastEntry *********************/


/************* EntryMap *********************/

EntryMap::EntryMap(uint8_t obd2Size, OBD2Entry obd2[], uint8_t broadcastSize,
				   BroadcastEntry broadcast[]) : obd2EntrySize(obd2Size), obd2Entries(obd2),
												 broadcastEntrySize(broadcastSize),
												 broadcastEntries(broadcast) {
};


const OBD2Entry *EntryMap::getNextObd2Entry() {
	if (obd2EntrySize == 0) {
		return nullptr;
	}
	uint8_t realIdx;
	for (uint8_t i = 0; i < obd2EntrySize; i++) {
		realIdx = (lastObdEntry + i) % obd2EntrySize;
		if (obd2Entries[realIdx].isNeedToUpdate()) {
			obd2Entries[realIdx].increment();
			lastObdEntry = (realIdx + 1) % obd2EntrySize;
			return &obd2Entries[realIdx];
		}
		LOG_DEBUG("entry map: skip id", obd2Entries[realIdx].id, "pid", obd2Entries[realIdx].pid);
	}

	return nullptr;
}

bool EntryMap::isBroadcastMsgAllowed(const uint32_t pid) {
	if (broadcastEntrySize == 0) {
		return false;
	}
	for (uint8_t i = 0; i < broadcastEntrySize; i++) {
		LOG_DEBUG("l pid = ", pid, "r pid =", broadcastEntries[i].pid);
		if (broadcastEntries[i].pid == pid) {
			broadcastEntries[i].increment();
			return broadcastEntries[i].isNeedToUpdate();
		}
	}
	return false;
}
