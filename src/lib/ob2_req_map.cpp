//
// Created by vadim on 1/8/25.
//

#include "ob2_req_map.h"

/************* OBD2Entry *********************/

OBD2Entry::OBD2Entry(const uint16_t id, const uint16_t pid, const uint8_t mode,
                     const uint16_t interval) : id(id), pid(pid), mode(mode),
                                                interval(interval) {
}


void OBD2Entry::increment() {
    updatedAt = millis() & 0xFFFF;
}


bool OBD2Entry::isNeedToUpdate() const {
    return static_cast<uint16_t>((millis() & 0xFFFF) - updatedAt) >= interval;
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
    for (uint8_t i = 0; i < obd2EntrySize; i++) {
        if (obd2Entries[i].isNeedToUpdate()) {
            obd2Entries[i].increment();
            return &obd2Entries[i];
        }
    }
    return nullptr;
}

bool EntryMap::isBroadcastMsgAllowed(const uint32_t pid) {
    for (uint8_t i = 0; i < broadcastEntrySize; i++) {
        if (broadcastEntries[i].pid == pid) {
            broadcastEntries[i].increment();
            return broadcastEntries[i].isNeedToUpdate();
        }
    }
    return false;
}
