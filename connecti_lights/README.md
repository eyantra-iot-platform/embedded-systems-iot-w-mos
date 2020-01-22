# A blank Mongoose OS app

## Overview

Tap on your lantern and all others will glow to your color wherever they are.
Meant for teaching MQTT, Cloud RPC and Rule Engine.

## Mongoose OS MQTT Client

MQTT 3.1.1 Client

## Connecting to Wifi

Two ways. Before flashing using config_schema.

TODO: `COMING SOON`

OR

using RPC:

`sudo mos wifi "ssid" "pass"`

## Notes

1. No messages by default. Only when you connect to Wifi the net and mqtt
event messages start showing up.
2. It is possible to have a "backup" server that device will connect to if it
fails to connect to the primary server. Present under mqtt1 in config (use
config-get to see).

## Important Links

https://github.com/cesanta/mongoose-os/blob/master/fw/examples/c_mqtt/src/main.c
https://mongoose-os.com/docs/mongoose-os/api/net/mqtt.md
https://cesanta.com/docs/mqtt/api.html
https://github.com/cesanta/mongoose/blob/master/examples/mqtt_client/mqtt_client.c
