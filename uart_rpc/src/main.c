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

void glow_red(int req_id) {
	LOG(LL_INFO, ("Glowing red"));
	set_colors(req_id, 255, 0, 0);
}

void glow_green(int req_id) {
	LOG(LL_INFO, ("Glowing green"));
	set_colors(req_id, 0, 255, 0);
}

void glow_blue(int req_id) {
	LOG(LL_INFO, ("Glowing blue"));
	set_colors(req_id, 0, 0, 255);
}

void glow_off(int req_id) {
	LOG(LL_INFO, ("Glowing off"));
	set_colors(req_id, 0, 0, 0);
}

void blink(int req_id) {
	int color = i++ % n_colors;
	if (color == 0) {
    	glow_red(req_id);
  	}
	else if (color == 1) {
		glow_green(req_id);
	}
	else if (color == 2) {
		glow_blue(req_id);
	}
	else if (color == 3) {
		glow_off(req_id);
	}
}

void get_flow(int req_id) {
	char message[50]; //, params[20], final_params[100];
	// int intensity;
	create_rpc_request(message, code_method("getFlow"), req_id, "");

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

// static void timer_cb(void *arg) {
// 	/*
// 	* Note: do not use mgos_uart_write to output to console UART (0 in our case).
// 	* It will work, but output may be scrambled by console debug output.
// 	*/
	
// 	// char message[50] = "#Ad255@"; //, params[50]; //, params[20], final_params[100];
// 	// char message[50], params[50]; //, params[20], final_params[100];
// 	// int intensity;
// 	// encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
// 	// encode_params(params, 0, "%d%d%d", r_lvl, g_lvl, b_lvl);
// 	// create_rpc_request(message, code_method("getFlow"), "");

// 	// LOG(LL_INFO, ("Message to be sent: %s", message));
// 	// mgos_uart_printf(UART_NO, message);

// 	blink();
// 	(void) arg;
// }

static void uart_dispatcher(int uart_no, void *arg) {
	// memory bufffers grow when needed. You can enter data at the end
	// or at any arbitrary positions.
	static char message[200] = {'\0'};
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
			LOG(LL_INFO, ("Flow readings %lf", flow_readings));
			sprintf(topic, "v1/devices/me/rpc/response/%d", req_id);
			LOG(LL_INFO, ("Response Topic %s", topic));
			mgos_mqtt_pubf(topic, 1, 0, "{rps: %lf}", flow_readings);
		}
	}
	else if (method == 40) {
		char message[40];
		if (sscanf(params, "s%s", message) == 1) {
			LOG(LL_INFO, ("Message printed on LCD %s", message));
			sprintf(topic, "v1/devices/me/rpc/response/%d", req_id);
			LOG(LL_INFO, ("Response Topic %s", topic));
			char *json_message = json_asprintf("{lcd_message: %Q}", message);
			mgos_mqtt_pub(topic, json_message, strlen(json_message), 1, 0);
		}
	}
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

static void ev_handler(struct mg_connection *c, int ev, void *p, void *user_data) {
	struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;

	if (ev == MG_EV_MQTT_CONNACK) {
		LOG(LL_INFO, ("CONNACK: %d", msg->connack_ret_code));
		struct mg_str topic = mg_mk_str("v1/devices/me/rpc/request/+");
		mgos_mqtt_global_subscribe(topic, ev_handler, NULL);
		LOG(LL_INFO, ("Subscribed to RPC Handler --> %.*s", (int)topic.len, topic.p));
	} 
	else if (ev == MG_EV_MQTT_SUBACK) {
		LOG(LL_INFO, ("Subscription %u acknowledged!", msg->message_id));
	} 
	else if (ev == MG_EV_MQTT_PUBLISH) {
		struct mg_str *s = &msg->payload;
		LOG(LL_INFO, ("Got RPC command: [%.*s]", (int) s->len, s->p));
		int req_id;
		sscanf((&msg->topic)->p, "v1/devices/me/rpc/request/%d", &req_id);
		LOG(LL_INFO, ("RPC req_id: [%d]", req_id));
		
		char *method = NULL, *intensity_str = NULL, *command = NULL;
		int pin;
		
		/* Our subscription is at QoS 1, we must acknowledge messages sent to us. */
		mg_mqtt_puback(c, msg->message_id);
		if (json_scanf(s->p, s->len, "{method: %Q, params: {pin: %d}}", &method, &pin) == 2) {
			LOG(LL_INFO, ("Method: %s, Pin: %d", method, pin));
			if (strcmp(method, "toggleLED")==0) {
				// char *res_topic
				/* Set GPIO pin to a given state */
				mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
				mgos_gpio_toggle(pin);
				
				// TODO:
				// send a response
				// mgos_mqtt_pub(strcat("v1/devices/me/rpc/response/$request_id", , const void *message, size_t len, int qos, bool retain);
			}
		}
		else if (json_scanf(s->p, s->len, "{method: setColor, params: {red_i: %d, green_i: %d, blue_i: %d}}", &red_i, &green_i, &blue_i) == 3) {
			LOG(LL_INFO, ("Got setColor RPC"));
			set_colors(req_id, red_i, green_i, blue_i);
		}
		else if (json_scanf(s->p, s->len, "{method: %Q, params: {command: %Q}}", &method, &command) == 2) {
			LOG(LL_INFO, ("ThingsBoard RPC Remote Shell --> Method: %s & Command: %s",
			method, command));
			if (strcmp(method, "sendCommand") == 0) {
				char lcd_message[40];
				if (strcmp(command, "get_flow") == 0) {
					get_flow(req_id);
				}
				else if (sscanf(command, "lcd_print %s", lcd_message) == 1) {
					lcd_print(req_id, lcd_message);
				}
			}
		}
		else if (json_scanf(s->p, s->len, "{method: %Q, params: %Q}", &method, &intensity_str) == 2) {
			int intensity = atoi(intensity_str);
			if (strcmp(method, "setRedIntensity") == 0) {
				red_i = intensity;
				set_colors(req_id, red_i, green_i, blue_i);
			}
			else if (strcmp(method, "setGreenIntensity") == 0) {
				green_i = intensity;
				set_colors(req_id, red_i, green_i, blue_i);
			}
			else if (strcmp(method, "setBlueIntensity") == 0) {
				blue_i = intensity;
				set_colors(req_id, red_i, green_i, blue_i);
			}
		}
		else {
			mgos_mqtt_pubf((&msg->topic)->p, 1, 0, "{error: {code: %d, message: %Q}}", 500, "unknown command");
		}
	}
	(void) user_data;
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

	// disabling timer
	// mgos_set_timer(4000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
	mgos_gpio_set_button_handler(PRG_BTN, MGOS_GPIO_PULL_NONE, MGOS_GPIO_INT_EDGE_NEG, 50, button_handler, NULL);
	mgos_mqtt_add_global_handler(ev_handler, NULL);
	return MGOS_APP_INIT_SUCCESS;
}
