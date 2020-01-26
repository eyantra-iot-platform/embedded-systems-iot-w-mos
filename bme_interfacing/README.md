# Reading data from a BME280

## Overview

Interfacing BME280 to read temperature, humidity and pressure.

BME sensor reports "temperature" in "degree celsius", "humidity" in "RH (relative humidity)"
and pressure in "hPA" (need to divide by 100).

Temperature -> -40 to 85 degree celsius (+-1 degree celsius)
Humidity    ->  0 to 100% RH (+-3%)
Pressure    -> 300 hPa to 1100 hPa (+-1hPa)

## Notes

1. Uses address 0x76. Else you'll get *BMP/BME280 device not found.*
2. You need to specify enable i2c and specify SDA and SDL in mos.yml or at runtime.
3. Converting from pressure to humidity is possible. Resource added in docs.

## Documentation

1. https://github.com/mongoose-os-libs/bme280/blob/master/src/mgos_bme280.c
2. https://github.com/mongoose-os-libs/i2c/blob/master/mos.yml
3. https://lastminuteengineers.com/bme280-arduino-tutorial/
4. https://github.com/adafruit/Adafruit_BME280_Library/blob/master/Adafruit_BME280.cpp#L507
