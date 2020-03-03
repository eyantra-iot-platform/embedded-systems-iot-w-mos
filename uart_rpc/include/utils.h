#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*
Concatenate parameters of a UART-RPC message, both request and response,
with a delimeter.
*/
void concat_params(char* result, char* param1, char* param2);

/*
Creates a request message for UART-RPC that takes method code and encoded params
between # and @.
*/
void create_rpc_request(char* message, char method, char* params);

/* Create UART-RPC response message */
void create_rpc_response(char* response, const char method, const char* params);

/*
Encode parameters of a UART-RPC message, both request and response
*/
void encode_params(char* params, int should_concat, const char* fmt, ...);

/* Parse UART-RPC request message */
int parse_rpc_request(char method, char* params, const char* message);

/*
Parse a response message from UART-RPC
*/
int parse_rpc_response(char method, char* params, const char* message);

#endif
