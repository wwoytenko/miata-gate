# Race Chrono DIY setup

## Requirements

* miata-gate build and running
* Race Chrono Pro app on your device

## Setup

### Add DIY board

* Go to settings
* Find **Other devices** section
* Press **Add device**
* Chose **RaceChrono DIY**
* Chose **Bluetooth LE**
* Press **Search for devices in range**
* Chose **miata-gate**
* Checkout only **CAN-Bus** option

#### Manually add CAN bus channels

* Create your Vehicle profile, add some info
* Go to **CAN-Bus channels**
* Here you have to add them using a table below by writing a specified values in the fields

# Custom Channels Table

| Channel name               | Channel Postfix | PID  | Equation                               |
|----------------------------|-----------------|------|----------------------------------------|
| Speed (km/h)               |                 | 513  | ((bytestoint(raw,4,2)/100.00)-100)/3.6 |
| Engine RPM (rpm)           |                 | 513  | bytestouint(raw,0,2)/4.00              |
| Coolant temperature (C)    |                 | 1056 | bytesToUint(raw, 0, 1) - 40            |
| Brake position (%)         |                 | 530  | bitsToUint(raw, 44, 1) * 100           |
| Clutch position (%)        |                 | 561  | bitsToUint(raw, 14, 1) * 100           |
| Intake temperature (C)     |                 | 576  | bytestouint(raw,4,1)-40                |
| Engine oil pressure (bar)  |                 | 106  | bytestoint(raw, 0, 2)                  |
| Engine oil temperature (C) |                 | 108  | bytestoint(raw, 0, 2)                  |
| Wheel Speed (km/h)         | FL              | 1200 | ((bytestoint(raw,0,2)/100.00)-100)/3.6 |
| Wheel Speed (km/h)         | FR              | 1200 | ((bytestoint(raw,2,2)/100.00)-100)/3.6 |
| Wheel Speed (km/h)         | RL              | 1200 | ((bytestoint(raw,4,2)/100.00)-100)/3.6 |
| Wheel Speed (km/h)         | RR              | 1200 | ((bytestoint(raw,6,2)/100.00)-100)/3.6 |
| Accelerator position (%)   |                 | 513  | bytestouint(raw,6,1)/2.00              |
