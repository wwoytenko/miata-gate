//
// Created by vvoitenko on 29/12/2024.
//

#ifndef MX5_H
#define MX5_H

// CAN ID (hex)	CAN ID (decimal)	Number of packets received over a 10 second period
//081	129	1000
//201	513	1000
//212	530	100
//231	561	400
//240	576	100
//420	1056	100
//4B0	1200	1000


// This build is for mazda mx5 nc without DCS
// https://github.com/timurrrr/RaceChronoDiyBleDevice/blob/master/can_db/mazda_mx5_nc.md
//#define MX5_VENDOR_STEERING_ANGLE 0x81
// 513
#define MX5_VENDOR_RPM_SPEED_THROTTLE 0x201
// 530
#define MX5_VENDOR_BRAKE_SWITCH 0x212
// MX5_VENDOR_GEAR_BOX - Is related to transmission, probably has more significative data for AT cars.
// On MT cars only interesting bit is the clutch switch
// 561
#define MX5_VENDOR_GEAR_BOX 0x231
// MX5_VENDOR_ENGINE_RELATED - Contains engine related data such as cooling water temperature, air intake temperature spark angle, calculated load,
// throttle valve position only interesting bit is the clutch switch
// 576
#define MX5_VENDOR_ENGINE_RELATED 0x240
// 1056
#define MX5_VENDOR_COOLANT 0x420
// 1200
#define MX5_VENDOR_WHEELS_SPEED 0x4B0

#define MX5_VENDOR_RPM_SPEED_THROTTLE_DIVIDER 5
#define MX5_VENDOR_BRAKE_SWITCH_DIVIDER 1
#define MX5_VENDOR_GEAR_BOX_DIVIDER 2
#define MX5_VENDOR_ENGINE_RELATED_DIVIDER 1
#define MX5_VENDOR_COOLANT_DIVIDER 1
#define MX5_VENDOR_WHEELS_SPEED_DIVIDER 5

#endif //MX5_H
