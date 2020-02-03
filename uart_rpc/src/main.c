#include <string.h>
#include "mgos.h"
#define UART_NO 1

static char method_code(char* method) {
  // every unique device gets a unique tens place bit
  // for example 1X for Valve, 2X for flow meter, 3X for RGB LED
  if (strcmp(method, "getValve") == 0) {
    return 10;
  }
  else if (strcmp(method, "setValve") == 0) {
    return 11;
  }
  else if (strcmp(method, "toggleValve") == 0) {
    return 12;
  }
  else if (strcmp(method, "getFlow") == 0) {
    return 20;
  }
  else if (strcmp(method, "setRed") == 0) {
    return 30;
  }
  else if (strcmp(method, "setGreen") == 0) {
    return 31;
  }
  else if (strcmp(method, "setBlue") == 0) {
    return 32;
  }
  return -1;
}

static void encode_params(char* param, char* data_type, void* value) {
  // every param has a type and a value
  // supported types
  // int -> i; double -> d; string -> s
  // separator is a hash
  // order of parameters matters
  if (strcmp(data_type, "int") == 0) {
    int* int_val = (int*)value;
    sprintf(param, "i%d", *int_val);
  }
  else if (strcmp(data_type, "double") == 0) {
    double* dbl_val = (double*)value;
    sprintf(param, "d%lf", *dbl_val);
  }
  else if (strcmp(data_type, "string") == 0) {
    char* conv_val = (char*)value;
    sprintf(param, "s%s", conv_val);
  }
}

static void create_uart_message(char *message, char method, char* params) {
  // # is the starting character
  // @ marks the end
  sprintf(message, "#%c%s@", method, params);
}

static void timer_cb(void *arg) {
  /*
   * Note: do not use mgos_uart_write to output to console UART (0 in our case).
   * It will work, but output may be scrambled by console debug output.
   */
  char message[50], params[20];
  int intensity = 125; 
  encode_params(params, "int", &intensity);
  create_uart_message(message, 31, params);
  
  LOG(LL_INFO, ("Message to be sent: %s", message));

  mgos_uart_printf(UART_NO, message);
  
  (void) arg;
}

static void uart_dispatcher(int uart_no, void *arg) {
  // memory bufffers grow when needed. You can enter data at the end
  // or at any arbitrary positions.
  static struct mbuf lb = {0};
  assert(uart_no == UART_NO);
  size_t rx_av = mgos_uart_read_avail(uart_no);
  if (rx_av == 0) return;
  mgos_uart_read_mbuf(uart_no, &lb, rx_av);
  /* Handle all the wonderful possibilities of different line endings. */
  struct mg_str b = mg_mk_str_n(lb.buf, lb.len);
  char *cr = (char *) mg_strchr(b, '\r');
  char *lf = (char *) mg_strchr(b, '\n');
  char *le;
  if (cr == NULL && lf == NULL) {
    return;
  } else if (cr != NULL && lf == NULL) {
    le = cr;
  } else if (cr == NULL && lf != NULL) {
    le = lf;
  } else {
    le = MIN(cr, lf);
  }
  *le = '\0';
  size_t llen = le - lb.buf;
  if (llen == 0) return;
  struct mg_str line = mg_mk_str_n(lb.buf, llen);
  //  * Now do something useful with "line" which contains the line data,
  /*
   * NUL-terminated for her pleasure.
   */
  LOG(LL_INFO, ("UART%d> '%.*s'", uart_no, (int) line.len, line.p));
  if (mg_vcasecmp(&line, "hi") == 0) {
    mgos_uart_printf(UART_NO, "Hello!\r\n");
  } else {
    mgos_uart_printf(UART_NO, "You said '%.*s'.\r\n", (int) line.len, line.p);
  }
  /* Finally, remove the line data from the buffer. */
  mbuf_remove(&lb, llen + 1);
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

  mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
