# Send commands from ESP32 Mongoose OS to Atmega2560

## Overview

This is an empty app, serves as a skeleton for building Mongoose OS
apps from # Send commands from ESP32 Mongoose OS to Atmega2560

## Notes

1. UART 1 of ESP32 communicates with UART 0 of ATmega 2560.

## Documentation

1. https://www.silabs.com/documents/public/application-notes/an0059.0-uart-flow-control.pdf

## Future API

```
typedef struct {

} rpc_object;

enum rpc_method_type {REQUEST=0, RESPONSE=1};

typedef struct {
    char name[255];
    enum rpc_method_type type;
    char fmt[511];
    char return_type;
    
} rpc_method;

int getFlowMeterRead() {
    return 10;
}

float totalVolumeOfLiquid(int pin) {
    return 30.5452;
}

typedef struct {
    
} rpc_channel;

rpc_channel rpc_channel_uart();
rpc_channel rpc_channel_i2c();
rpc_channel rpc_channel_spi();
rpc_channel rpc_channel_custom();

rpc_object rpc_init(rpc_channel channel, rpc_method methods[]);

rpc_send(rpc_object, "methodName", ...);
```