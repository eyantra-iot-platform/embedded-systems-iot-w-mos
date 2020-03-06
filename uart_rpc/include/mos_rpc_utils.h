#ifndef UTILS_H
#define UTILS_H

#include <mgos.h>
#include "mos_rpc_utils.h"

/*
[DEPRECATED]
Concatenate parameters of a UART-RPC message, both request and response,
with a delimeter.
*/
// void concat_params(char* result, char* param1, char* param2);

/*
Fetch RPC request
*/
int fetch_rpc_request(char* message, int uart_no);

/*
Fetch RPC response
*/
int fetch_rpc_response(char* message, int uart_no);

#endif
