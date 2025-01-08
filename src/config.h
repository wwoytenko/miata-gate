//
// Created by vadim on 1/8/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include "lib/ob2_req_map.h"
#include "lib/mx5.h"
#include "lib/board.h"

#define BOARD_OBD2_ECU_REQUEST_CAN_ID 0x7E0

inline OBD2Entry obd2Reqs[]{
    OBD2Entry(
        BOARD_OBD2_ECU_REQUEST_CAN_ID,
        MAZDA_MX5_BRAKE_SWITCH,
        BOARD_OBD2_REQUEST_MODE_READ_DATA_BY_IDENTIFIER,
        50
    ),
};

inline BroadcastEntry broadcastEntries[]{
    BroadcastEntry(MX5_VENDOR_RPM_SPEED_THROTTLE),
    BroadcastEntry(MX5_VENDOR_BRAKE_SWITCH),
    BroadcastEntry(MX5_VENDOR_GEAR_BOX),
    BroadcastEntry(MX5_VENDOR_ENGINE_RELATED),
    BroadcastEntry(MX5_VENDOR_COOLANT),
    BroadcastEntry(MX5_VENDOR_WHEELS_SPEED)
};

inline EntryMap em(1, obd2Reqs, 6, broadcastEntries);


#endif //CONFIG_H
