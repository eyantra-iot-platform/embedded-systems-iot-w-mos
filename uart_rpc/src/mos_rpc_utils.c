#include "rpc_common.h"
#include "mos_rpc_utils.h"

int fetch_rpc_response(char* message, int uart_no) {
	static struct mbuf lb = {0};
	int MSG_FOUND = 0;

	// assert(uart_no == UART_NO);
	size_t rx_av = mgos_uart_read_avail(uart_no);
	if (rx_av == 0) return 0;
	mgos_uart_read_mbuf(uart_no, &lb, rx_av);
	LOG(LL_INFO, ("> %.*s", (int) rx_av, lb.buf));

	int i = 0;
	static int START_FOUND = 0;
	static int message_ptr = 0;
	while (i < rx_av) {
		char curr_byte = lb.buf[i++];        
		// if no data, continue
		if (curr_byte == '#') {
			// LOG(LL_INFO, ("Start found at %d", i-1));
			START_FOUND = 1;
			message_ptr = 0;
		}
		else if (curr_byte == '@') {
			// LOG(LL_INFO, ("Stop found at %d", i-1));
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

int fetch_rpc_request(char* message, int uart_no) {
	return fetch_rpc_response(message, uart_no);
}
