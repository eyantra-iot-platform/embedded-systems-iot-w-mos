#include <string.h>
#include "mgos.h"
#include "utils.h"

#define UART_NO 1

static void timer_cb(void *arg) {
  /*
   * Note: do not use mgos_uart_write to output to console UART (0 in our case).
   * It will work, but output may be scrambled by console debug output.
   */
  char message[50]; //, params[20], final_params[100];
  // int intensity;
  
  create_rpc_request(message, code_method("getFlow"), "i200,i15,i150");

  LOG(LL_INFO, ("Message to be sent: %s", message));

  mgos_uart_printf(UART_NO, message);
  (void) arg;
}

static void uart_dispatcher(int uart_no, void *arg) {
  // memory bufffers grow when needed. You can enter data at the end
  // or at any arbitrary positions.
  static char message[200] = {'\0'};
  
  static struct mbuf lb = {0};
  int MSG_FOUND = 0;

  assert(uart_no == UART_NO);
  size_t rx_av = mgos_uart_read_avail(uart_no);
  if (rx_av == 0) return;
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

    char method, params[200];
    parse_rpc_request(&method, params, message);
		
		if (method == 65) {
      int flow_readings;
      if (sscanf(params, "d%d", &flow_readings) == 1) {
        LOG(LL_INFO, ("Flow readings %d", flow_readings));
      }
		}
		// else if (method == 30) {
		// 	int red_i;
		// 	if (sscanf(params, "i%d", &red_i) == 1) {
		// 		brightness(red_i, 0, 0);
		// 		// uart0_puts("#%ci255@");
		// 	}
		// }
		// else if (method == 31) {
		// 	int green_i;
		// 	if (sscanf(params, "i%d", &green_i) == 1) {
		// 		brightness(0, green_i, 0);
		// 	}
		// }
		// else if (method == 32) {
		// 	int blue_i;
		// 	if (sscanf(params, "i%d", &blue_i) == 1) {
		// 		brightness(0, 0, blue_i);
		// 	}
		// }
		// else if (method == 33) {
		// 	int red_i, green_i, blue_i;
		// 	if (sscanf(params, "i%d,i%d,i%d", &red_i, &green_i, &blue_i) == 3) {
		// 		brightness(red_i, green_i, blue_i);
		// 	}
		// }
  }

  mbuf_remove(&lb, i);
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_uart_config ucfg;
  mgos_uart_config_set_defaults(UART_NO, &ucfg);
  /*
   * At this point it is possible to adjust baud rate, pins and other settings.
   * 115200 8-N-1 is the default mode, but we set it anyway
   */
  ucfg.baud_rate = 9600;
  ucfg.num_data_bits = 8;
  ucfg.parity = MGOS_UART_PARITY_NONE;
  ucfg.stop_bits = MGOS_UART_STOP_BITS_1;
  // ucfg.tx_fc_type = MGOS_UART_FC_SW;
  // ucfg.rx_fc_type = MGOS_UART_FC_SW;
  
  // overriding default GPIOs
  ucfg.dev.rx_gpio = 33;
  ucfg.dev.tx_gpio = 32;

  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    return MGOS_APP_INIT_ERROR;
  }

  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
  mgos_uart_set_rx_enabled(UART_NO, true);

  mgos_set_timer(4000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
