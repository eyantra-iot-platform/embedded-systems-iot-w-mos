# Reading data from a BME280

## Overview

Interfacing BME280 to read temperature, humidity and pressure.

BME sensor reports "temperature" in "degree celsius", humidity in ""

## Notes

1. Uses address 0x76. Else you'll get *BMP/BME280 device not found.*
2. You need to specify enable i2c and specify SDA and SDL in mos.yml or at runtime.

## Documentation

1. https://github.com/mongoose-os-libs/bme280/blob/master/src/mgos_bme280.c
2. https://github.com/mongoose-os-libs/i2c/blob/master/mos.yml
