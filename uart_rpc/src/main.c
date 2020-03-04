#include <string.h>
#include "mgos.h"
#include "utils.h"

#define UART_NO 1

int i = 0;
const int n_colors = 4;

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
    return 65; // CHANGED
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

void setLED(int r_lvl, int g_lvl, int b_lvl) {
  char message[50], params[50]; //, params[20], final_params[100];
  // int intensity;
  encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
  create_rpc_request(message, code_method("setColor"), params);

  LOG(LL_INFO, ("Message to be sent: %s", message));
  mgos_uart_printf(UART_NO, message);
}

void glowRed() {
  LOG(LL_INFO, ("Glowing red"));
  setLED(255, 0, 0);
}

void glowGreen() {
  LOG(LL_INFO, ("Glowing green"));
  setLED(0, 255, 0);
}

void glowBlue() {
  LOG(LL_INFO, ("Glowing blue"));
  setLED(0, 0, 255);
}

void glowOff() {
  LOG(LL_INFO, ("Glowing off"));
  setLED(0, 0, 0);
}

void blink() {
  int color = i++ % n_colors;
  if (color == 0) {
    glowRed();
  }
  else if (color == 1) {
    glowGreen();
  }
  else if (color == 2) {
    glowBlue();
  }
  else if (color == 3) {
    glowOff();
  }
}

static void timer_cb(void *arg) {
  /*
   * Note: do not use mgos_uart_write to output to console UART (0 in our case).
   * It will work, but output may be scrambled by console debug output.
   */
  
  // char message[50] = "#Ad255@"; //, params[50]; //, params[20], final_params[100];
  // char message[50], params[50]; //, params[20], final_params[100];
  // int intensity;
  // encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
  // encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
  // create_rpc_request(message, code_method("getFlow"), "");

  // LOG(LL_INFO, ("Message to be sent: %s", message));
  // mgos_uart_printf(UART_NO, message);

  blink();
  (void) arg;
}

static void uart_dispatcher(int uart_no, void *arg) {
	// memory bufffers grow when needed. You can enter data at the end
	// or at any arbitrary positions.
    static char message[200] = {'\0'};
    char method, params[200];
    // default method
    int status = fetch_rpc_response(message, UART_NO);
	if (!status)
		return;

    parse_rpc_response(&method, params, message);
    
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
