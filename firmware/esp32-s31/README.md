# ESP32-S31 P0 bring-up (safe mode)

Target hardware: Espressif ESP32-S31-Function-CoreBoard-1 / ESP32-S31-WROOM-3.
Target framework: ESP-IDF v6.1+ supporting `esp32s31`. **Not ESP32-S3.**

This is a standalone prototype, **not a port of the SimpleFOC balance firmware**.
By design there is **no motor/servo output**, motor initialization, or motion command. Do not connect live actuators for first boot.

## Mac / Linux build

Install an ESP-IDF release that supports the `esp32s31` target and source its `export.sh`:

```bash
cd firmware/esp32-s31
idf.py set-target esp32s31
idf.py build
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

Use `ls /dev/cu.*` to identify the actual port. Never run `erase-flash` unless you intend to discard existing firmware.

## Test gates

- T0: board alone, USB serial logs show chipset/IDF version and periodic safe-mode heartbeat.
- T1: verify external power **OFF**, 3.3 V signal levels, common ground, breakout J2 wiring and pullups; only then enable I2C probe in `idf.py menuconfig`.
- T2: connect **one** AS5600 (expected 0x36) or MPU6050 (expected 0x68, or 0x69 if AD0 high), scan and record address.
- T3: investigate second AS5600 requiring its own I2C bus or an address-multiplexing solution; the old firmware used two buses.
- T4: independently review UART servo half-duplex physical transceiver and motor driver power stages before writing drivers.

Do not infer passing any tests solely because firmware compiles.

Reference: https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32s31/esp32-s31-function-coreboard-1/user_guide.html
