#include "rpc_common.h"

void create_rpc_request(char *message, char method, int req_id, char* params) {
  // # is the starting character
  // @ marks the end
  sprintf(message, "#%c%d%s@", method, req_id, params);
}

void create_rpc_response(char *message, char method, int req_id, char* params) {
  // # is the starting character
  // @ marks the end
  create_rpc_request(message, method, req_id, params);
}

// TODO: Accept empty params
int parse_rpc_request(char* method, int* req_id, char* params, const char* message) {
  strcpy(params, "");
  int scanned_params = sscanf(message, "%c%d%[^\n]s", method, req_id, params);
  if (scanned_params == 3) {
		return 1;
  }
  else if (scanned_params == 2 && strlen(params) == 0) {
		return 1;
  }
  return 0;
}

int parse_rpc_response(char* method, int* req_id, char* params, const char* message) {
	return parse_rpc_request(method, req_id, params, message);
}

void encode_params(char* params, int should_concat, const char* fmt, ...) {
    // every param has a type and a value
    // supported types
    // int -> i; double -> d; string -> s
    // separator is a hash
    // order of parameters matters
    if (should_concat == 0)
        params[0] = '\0';

    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
		char str_repr[30], first_char;
		int UNKWN = 0;

		if (*fmt != '%') {
				fmt++;
				continue;
		}

		first_char = *++fmt;

		if (first_char == 'd') {
			sprintf(str_repr, "d%d", va_arg(args, int));
		} else if (first_char == 's') {
			sprintf(str_repr, "s%s", va_arg(args, char*));
		} else if (first_char == 'c') {
			sprintf(str_repr, "c%c", va_arg(args, int));
		} else if (first_char == 'f') {
			sprintf(str_repr, "f%.3f", va_arg(args, double));
		} else if (first_char == 'l') {
			// NOT TESTED
			if (*(fmt+1) == 'f') {
				sprintf(str_repr, "lf%.3lf", va_arg(args, double));
				++fmt;
			}
			else if (*(fmt+1) == 'd') {
				sprintf(str_repr, "ld%ld", va_arg(args, long));
				++fmt;
			}
		} else {
			UNKWN = 1;
		}

		if (UNKWN == 0) {
			// return count for correctly parsed variables; count++;
			if (strlen(params) != 0)
				sprintf(params, "%s,%s", params, str_repr);
			// printf("%d %s\n", strlen(params), str_repr);
			else
				strcpy(params, str_repr);
		}
	  
	  	fmt++;
    }
    va_end(args);
}
