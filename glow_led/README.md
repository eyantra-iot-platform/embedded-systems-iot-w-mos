# Blinking an RGB Led

## Overview

The goal of this app is to teach basic GPIO functions, timers, logging and
config_schema.

## Notes

1. Arduino has sleep but in Mongoose OS we use timers.

## GPIO and Timers

``No need to include mgos_gpio.h``

Documentation available at [GPIO](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_gpio.h.md)
and [Timers](https://mongoose-os.com/docs/mongoose-os/api/core/mgos_timers.h.md).

## Logging

`LOG(LL_INFO, ("string with param", param, "string"))`

## Config-Schema

How to write [mos.yml](https://mongoose-os.com/docs/mongoose-os/userguide/build.md#mosyml-file-format-reference)?

## Using RPC to change blink duration in real-time

`sudo mos call Config.Set "{\"config\": {\"blink_app\": {\"time\": 2000}}}"`

OR

`sudo mos config-set blink_app.time=2000`
