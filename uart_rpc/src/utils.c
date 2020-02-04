#include <stdio.h>
#include <string.h>

#include "utils.h"

char code_method(char* method_name) {
  // every unique device gets a unique tens place bit
  // for example 1X for Valve, 2X for flow meter, 3X for RGB LED
  if (strcmp(method_name, "getValve") == 0) {
    return 10;
  }
  else if (strcmp(method_name, "setValve") == 0) {
    return 11;
  }
  else if (strcmp(method_name, "toggleValve") == 0) {
    return 12;
  }
  else if (strcmp(method_name, "getFlow") == 0) {
    return 20;
  }
  else if (strcmp(method_name, "setRed") == 0) {
    return 30;
  }
  else if (strcmp(method_name, "setGreen") == 0) {
    return 31;
  }
  else if (strcmp(method_name, "setBlue") == 0) {
    return 32;
  }
  else if (strcmp(method_name, "setColor") == 0) {
    return 33;
  }
  return -1;
}

void encode_params(char* param, char* data_type, void* value) {
  // every param has a type and a value
  // supported types
  // int -> i; double -> d; string -> s
  // separator is a hash
  // order of parameters matters
  if (!strcmp(data_type, "int")) {
    int* int_val = (int*)value;
    sprintf(param, "i%d", *int_val);
  }
  else if (!strcmp(data_type, "double")) {
    double* dbl_val = (double*)value;
    sprintf(param, "d%.3lf", *dbl_val);
  }
  else if (!strcmp(data_type, "string")) {
    char* conv_val = (char*)value;
    sprintf(param, "s%s", conv_val);
  }
}

void concat_params(char* result, char* param1, char* param2) {
  sprintf(result, "%s,%s", param1, param2);
}

void create_rpc_request(char *message, char method, char* params) {
  // # is the starting character
  // @ marks the end
  sprintf(message, "#%c%s@", method, params);
}

void parse_uart_response() {}
