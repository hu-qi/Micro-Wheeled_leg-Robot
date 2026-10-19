# ESP32-S31 migration P0: source-to-board interface audit

**Status:** schematic cross-check and physical testing pending. Never treat a historical ESP32 pin number as a drop-in S31 connection.

## Legacy wiring (verified in `3.Software/wl_pro_robot/wl_pro_robot.ino`)

| Device / purpose | Original firmware connection | S31 status |
| --- | --- | --- |
| Left BLDC driver (3 PWM + enable) | GPIO32, 33, 25, enable 22 | **UNASSIGNED**; driver L6234-related electrical contract must be checked |
| Right BLDC driver (3 PWM + enable) | GPIO26, 27, 14, enable 12 | **UNASSIGNED** |
| AS5600 #1, bus 0 | SDA 19 / SCL 18, 400 kHz | **UNASSIGNED** |
| AS5600 #2, bus 1 | SDA 23 / SCL 5, 400 kHz | **UNASSIGNED** |
| MPU6050 IMU | shared bus 1 with AS5600 #2 | **UNASSIGNED** |
| Serial FEETECH STS servos | `Serial2.begin(1000000)`, IDs 1,2 | **UNASSIGNED**; half-duplex transceiver/power needs verification |
| Battery ADC | ESP32 GPIO35 / ADC1 channel 7 | **UNASSIGNED**; divider ratio, max ADC input, and calibration must be checked |
| Battery indicator | GPIO13 | **UNASSIGNED** |
| Console + commander | `Serial` / 115200 baud | Use board USB Serial/JTAG for P0 logging; do not reserve legacy serial pins blindly |

## Verified J2 board breakout from Espressif documentation

- Pins J2-3/J2-4: GPIO58 TX0 / GPIO59 RX0, already console-related; avoid for first mapping.
- J2-5: GPIO61 BOOT; reserve.
- J2-6: GPIO60 RGB LED; reserve.
- Candidate breakouts for future mapping include J2-7 GPIO2, J2-9 GPIO0, J2-10 GPIO3, J2-12 GPIO1, J2-32 GPIO4, plus J2-11/13–25 GPIO49/47/48/45/46/43/44/40/42/39/38/37/36/35.
- J2-26–31 are marked SDIO functions GPIO20–25: verify multiplexing and boot constraints before allocation.
- J2-35/36 provide 3V3 and J2-39/40 5V. **Power pins are NOT logic-level compatibility guarantees.**
- Board already uses other chip functions for Ethernet, audio, USB, boot. Check the official board schematic and ESP32-S31 GPIO restrictions, then choose a nonconflicting mapping.

## Interfaces to validate before hardware enable

1. S31 MCPWM count, 3-phase waveforms, phase polarity, dead-time, gate enable and motor driver voltage. Use an oscilloscope with motor supply removed.
2. Two simultaneous I2C buses for same-address AS5600s, plus MPU6050 bus sharing; account for noise and pullups.
3. STS3032 UART at 1 Mbps: verify 3.3 V tolerance, half-duplex conversion/enable, servo independent power.
4. Battery sensing: resistor network, absolute maximum ratings, ADC scaling.
5. Safe state machine: boot-disabled drivers, command timeout, tilt cutoff, bus failure stop, brownout handling, explicit arming.
6. Real-time control benchmarking: sample period, jitter, priority inversion; no networking or AI in hard control loop.

## First software deliverable

`firmware/esp32-s31` is a **non-actuating** ESP-IDF bring-up application with optional I2C address probe and serial heartbeat. It deliberately does not guess or energize motor or servo pins.

## References

- Original code: https://github.com/hu-qi/Micro-Wheeled_leg-Robot/blob/master/3.Software/wl_pro_robot/wl_pro_robot.ino
- Board guide (J2 table, power, peripherals): https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32s31/esp32-s31-function-coreboard-1/user_guide.html
- ESP32-S31 getting started: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s31/get-started/linux-macos-start-project.html
