/*
Generates a UART-RPC method character code for a given method name
*/
char code_method (char* method_name);

/*
Encode parameters of a UART-RPC message, both request and response
*/
void encode_params(char* param, char* data_type, void* value);

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

/*
Parse a response message from UART-RPC
*/
// void parse_uart_response(char* message, char method, char* params);
