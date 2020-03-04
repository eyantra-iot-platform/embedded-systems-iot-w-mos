#include "mos_rpc_utils.h"

void concat_params(char* result, char* param1, char* param2) {
  sprintf(result, "%s,%s", param1, param2);
}

void create_rpc_request(char *message, char method, char* params) {
  // # is the starting character
  // @ marks the end
  sprintf(message, "#%c%s@", method, params);
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

int parse_rpc_request(char* method , char* params, const char* message) {
  return (sscanf(message, "%c%s", method, params) == 2);
}

int parse_rpc_response(char* method, char* params, const char* message) {
	return parse_rpc_request(method, params, message);
}

int fetch_rpc_response(char* message, int uart_no) {
	static struct mbuf lb = {0};
	int MSG_FOUND = 0;

	// assert(uart_no == UART_NO);
	size_t rx_av = mgos_uart_read_avail(uart_no);
	if (rx_av == 0) return 0;
	mgos_uart_read_mbuf(uart_no, &lb, rx_av);
	LOG(LL_INFO, ("Received: %.*s", (int) rx_av, lb.buf));

	int i = 0;
	static int START_FOUND = 0;
	static int message_ptr = 0;
	while (i < rx_av) {
		char curr_byte = lb.buf[i++];        
		// if no data, continue
		if (curr_byte == '#') {
			LOG(LL_INFO, ("Start found at %d", i-1));
			START_FOUND = 1;
			message_ptr = 0;
		}
		else if (curr_byte == '@') {
			LOG(LL_INFO, ("Stop found at %d", i-1));
			if (START_FOUND == 1) {
				MSG_FOUND = 1;
				break;
			}
		}
		else {
	  		if (START_FOUND)
				message[message_ptr++] = curr_byte;
		}
	}

	if (MSG_FOUND == 1) {
		message[message_ptr] = '\0';
		START_FOUND = 0;
		MSG_FOUND = 0;
		message_ptr = 0;
		LOG(LL_INFO, ("Received RPC message: %s", message));
		mbuf_remove(&lb, i);
		return 1;
	}
	mbuf_remove(&lb, i);
	return 0;
}
