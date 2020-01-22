# Blinking an RGB Led

## Overview

The goal of this app is to teach basic GPIO functions, timers and
config_schema.

## Config-Schema

How to write [mos.yml](https://mongoose-os.com/docs/mongoose-os/userguide/build.md#mosyml-file-format-reference)?

## Using RPC to change blink duration in real-time

`sudo mos call Config.Set "{\"config\": {\"blink_app\": {\"time\": 2000}}}"`

OR

`sudo mos config-set blink_app.time=2000`

## Notes

1. Arduino has sleep but in MOS we use timers.
