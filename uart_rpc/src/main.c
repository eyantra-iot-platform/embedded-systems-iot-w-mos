#include <string.h>
#include "mgos.h"
#include "mgos_mqtt.h"

#include "rpc_common.h"
#include "mos_rpc_utils.h"

#define UART_NO 1
#define PRG_BTN 13
#define BOARD_LED 18

int i = 0;
const int n_colors = 4;
int red_i = 0, green_i = 0, blue_i = 0;

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
	else if (strcmp(method_name, "lcdPrint") == 0) {
		return 40;
	}
	else if (strcmp(method_name, "getLFA") == 0) {
		return 41;
	}
	return -1;
}

void set_colors(int req_id, int r_lvl, int g_lvl, int b_lvl) {
  char message[50], params[50]; //, params[20], final_params[100];
  // int intensity;
  encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
  create_rpc_request(message, code_method("setColor"), req_id, params);

  LOG(LL_INFO, ("Message to be sent: %s", message));
  mgos_uart_printf(UART_NO, message);
}

void get_flow(int req_id) {
	char message[50]; //, params[20], final_params[100];
	// int intensity;
	create_rpc_request(message, code_method("getFlow"), req_id, "");

	LOG(LL_INFO, ("Message to be sent: %s", message));
	mgos_uart_printf(UART_NO, message);
}

void get_lfa(int req_id) {
	char message[50]; //, params[20], final_params[100];
	// int intensity;
	create_rpc_request(message, code_method("getLFA"), req_id, "");

	LOG(LL_INFO, ("Message to be sent: %s", message));
	mgos_uart_printf(UART_NO, message);
}

void lcd_print(int req_id, char* lcd_message) {
	char message[50], params[50]; //, params[20], final_params[100];
	// int intensity;
	encode_params(params, 0, "%s", lcd_message);
	create_rpc_request(message, code_method("lcdPrint"), req_id, params);

	LOG(LL_INFO, ("Message to be sent: %s", message));
	mgos_uart_printf(UART_NO, message);
}

static void uart_dispatcher(int uart_no, void *arg) {
	// memory bufffers grow when needed. You can enter data at the end
	// or at any arbitrary positions.
	static char message[2048] = {'\0'};
	char method, params[200];
	int req_id;
	// default method
	int status = fetch_rpc_response(message, UART_NO);
	if (!status) return;

	int success = parse_rpc_response(&method, &req_id, params, message);
	LOG(LL_INFO, ("Parsed Method: %c, Req ID: %d, Params: %s", method, req_id, params));
	if (!success) return;
	
	char topic[50];
	if (method == 65) {
		double flow_readings;
		if (sscanf(params, "lf%lf", &flow_readings) == 1) {		
			sprintf(topic, "v1/devices/me/rpc/response/%d", req_id);
			LOG(LL_INFO, ("Flow readings %lf", flow_readings));
			mgos_mqtt_pubf(topic, 1, 0, "{rps: %lf}", flow_readings);
		}
	}
	else if (method == 40) {
		char message[40];
		if (sscanf(params, "s%s", message) == 1) {
			LOG(LL_INFO, ("Message printed on LCD %s", message));
			sprintf(topic, "v1/devices/me/rpc/response/%d", req_id);
			char *json_message = json_asprintf("{lcd_message: %Q}", message);
			mgos_mqtt_pub(topic, json_message, strlen(json_message), 1, 0);
		}
	}
	else if (method == 41) {
		int lfa_readings;
		if (sscanf(params, "d%d", &lfa_readings) == 1) {		
			sprintf(topic, "v1/devices/me/rpc/response/%d", req_id);
			LOG(LL_INFO, ("LFA readings %d", lfa_readings));
			mgos_mqtt_pubf(topic, 1, 0, "{line_sensor: %d}", lfa_readings);
		}
	}
  	(void) arg;
}

void button_handler (int pin, void *arg) {
	char req_topic[50], req_msg[200];
	struct json_out jmo = JSON_OUT_BUF(req_msg, sizeof(req_msg));

	LOG(LL_INFO, ("Toggling ... "));

	// toggle ESP32 LED
	mgos_gpio_set_mode(BOARD_LED, MGOS_GPIO_MODE_OUTPUT);
	mgos_gpio_toggle(BOARD_LED);
  
	LOG(LL_INFO, ("Going to send a publish message to all connected lights ... "));
	
	//  TODO:
	// generate random request id topic
	sprintf(req_topic, "v1/devices/me/rpc/request/%d", (int)((float)rand() / RAND_MAX * (500)));
	LOG(LL_INFO, ("RPC Request topic %s", req_topic));
  
	// create send message
	json_printf(&jmo, "{method: %Q, params: {red_i: %d, green_i: %d, blue_i: %d}}", 
	"broadcastColor", 0, 255, 0);
	// uint16_t mgos_mqtt_pub(const char *topic, const void *message, 
	// size_t len, int qos, bool retain)
	mgos_mqtt_pub(req_topic, req_msg, strlen(req_msg), 1, 0);
	LOG(LL_INFO, ("Sent %s to %s", req_topic, req_msg));
	(void) arg;
}

void sub_handler(struct mg_connection *nc, const char *topic, int topic_len,
const char *msg, int msg_len, void *ud) {
	int ERR_FLAG = 1;
	LOG(LL_INFO, ("Got RPC command: [%.*s]", msg_len, msg));
	int req_id;
	sscanf(topic, "v1/devices/me/rpc/request/%d", &req_id);
	LOG(LL_INFO, ("RPC req_id: [%d]", req_id));
	
	char *method = NULL, *param_message = NULL, *command = NULL;
	int pin;
	
	/* Our subscription is at QoS 1, we must acknowledge messages sent to us. */
	// TODO:
	// mg_mqtt_puback(nc, msg->message_id);
	if (json_scanf(msg, msg_len, "{method: %Q, params: {pin: %d}}", &method, &pin) == 2) {
		LOG(LL_INFO, ("Method: %s, Pin: %d", method, pin));
		if (strcmp(method, "toggleLED")==0) {
			ERR_FLAG = 0;
			// char *res_topic
			/* Set GPIO pin to a given state */
			mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
			mgos_gpio_toggle(pin);
			
			// TODO:
			// send a response
			// mgos_mqtt_pub(strcat("v1/devices/me/rpc/response/$request_id", , const void *message, size_t len, int qos, bool retain);
		}
	}
	else if (json_scanf(msg, msg_len, "{method: %Q, params: {red_i: %d, green_i: %d, blue_i: %d}}", 
	&method, &red_i, &green_i, &blue_i) == 4) {
		if (strcmp(method, "setColor") == 0) {
			ERR_FLAG = 0;
			LOG(LL_INFO, ("Got setColor RPC"));
			set_colors(req_id, red_i, green_i, blue_i);
		} 
	}
	else if (json_scanf(msg, msg_len, "{method: %Q, params: %Q}", &method, &param_message) == 2) {
		if (strcmp(method, "setRedIntensity") == 0) {
			ERR_FLAG = 0;
			int intensity = atoi(param_message);
			red_i = intensity;
			set_colors(req_id, red_i, green_i, blue_i);
		}
		else if (strcmp(method, "setGreenIntensity") == 0) {
			ERR_FLAG = 0;
			int intensity = atoi(param_message);
			green_i = intensity;
			set_colors(req_id, red_i, green_i, blue_i);
		}
		else if (strcmp(method, "setBlueIntensity") == 0) {
			ERR_FLAG = 0;
			int intensity = atoi(param_message);
			blue_i = intensity;
			set_colors(req_id, red_i, green_i, blue_i);
		}
		else if (strcmp(method, "lcd_print") == 0) {
			ERR_FLAG = 0;
			lcd_print(req_id, param_message);
		}
	}
	else if (json_scanf(msg, msg_len, "{method: %Q}", &method) == 1) {
		if (strcmp(method, "get_flow") == 0) {
			ERR_FLAG = 0;
			get_flow(req_id);
		}
		else if (strcmp(method, "get_lfa") == 0) {
			ERR_FLAG = 0;
			get_lfa(req_id);
		}
	}

	if (ERR_FLAG == 1) {
		LOG(LL_INFO, ("{error: {code: %d, message: %s}}", 500, "unknown command"));
	}
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
	
	// increasing buffer size
	ucfg.rx_buf_size = 2048;
	ucfg.tx_buf_size = 2048;

	// overriding default GPIOs
	ucfg.dev.rx_gpio = 33;
	ucfg.dev.tx_gpio = 32;

	if (!mgos_uart_configure(UART_NO, &ucfg)) {
		return MGOS_APP_INIT_ERROR;
	}

	mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
	mgos_uart_set_rx_enabled(UART_NO, true);

	// disabling timer
	// mgos_set_timer(4000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
	mgos_gpio_set_button_handler(PRG_BTN, MGOS_GPIO_PULL_NONE, MGOS_GPIO_INT_EDGE_NEG, 50, button_handler, NULL);

	// disabling ev_handler
	// mgos_mqtt_add_global_handler(ev_handler, NULL);
	mgos_mqtt_sub("v1/devices/me/rpc/request/+", sub_handler, NULL);
	return MGOS_APP_INIT_SUCCESS;
}
