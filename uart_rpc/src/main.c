#include <string.h>
#include "mgos.h"
#include "utils.h"

#define UART_NO 1

static void timer_cb(void *arg) {
  /*
   * Note: do not use mgos_uart_write to output to console UART (0 in our case).
   * It will work, but output may be scrambled by console debug output.
   */
  static int seq = 0;
  mgos_uart_printf(UART_NO, "Hello, UART%d! %d from ESP1 \r\n", UART_NO, seq++);
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
  ucfg.tx_fc_type = MGOS_UART_FC_SW;
  ucfg.rx_fc_type = MGOS_UART_FC_SW;
  
  // overriding default GPIOs
  ucfg.dev.rx_gpio = 3;
  ucfg.dev.tx_gpio = 1;

  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    return MGOS_APP_INIT_ERROR;
  }

  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
  mgos_uart_set_rx_enabled(UART_NO, true);

  mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
