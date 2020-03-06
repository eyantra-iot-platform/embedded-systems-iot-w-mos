#ifndef RPC_COMMON_H
#define RPC_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* Create UART-RPC request message */
void create_rpc_request(char* request, char method, int req_id, char* params);

/* Create UART-RPC response message */
void create_rpc_response(char* response, char method, int req_id, char* params);

/* Parse UART-RPC request message */
int parse_rpc_request(char* method, int* req_id, char* params, const char* message);

/*
Parse a response message from UART-RPC
*/
int parse_rpc_response(char* method, int* req_id, char* params, const char* message);

/*
Encode parameters of a UART-RPC message, both request and response
*/
void encode_params(char* params, int should_concat, const char* fmt, ...);

#endif