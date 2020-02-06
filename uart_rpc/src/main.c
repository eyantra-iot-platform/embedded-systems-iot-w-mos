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
  static char *old_start, *new_start, *end; //, *le;
  static char message[200];
  static struct mbuf lb = {0};
  int END_FLAG = 0;

  char parts[200];

  assert(uart_no == UART_NO);
  size_t rx_av = mgos_uart_read_avail(uart_no);
  if (rx_av == 0) return;
  mgos_uart_read_mbuf(uart_no, &lb, rx_av);
  LOG(LL_INFO, ("Received: %.*s", (int) rx_av, lb.buf));
  // mbuf_remove(&lb, rx_av);
  /* Handle all the wonderful possibilities of different line endings. */
  struct mg_str b = mg_mk_str_n(lb.buf, lb.len);

  new_start = (char *) mg_strchr(b, '#');
  end = (char *) mg_strchr(b, '@');
  // le = (char *) mg_strchr(b, '\0');

  if (end == NULL) {
    // points to the end char or one plus the length of read bytes
    end = lb.buf + rx_av;
  } else {
    if (new_start != NULL || old_start != NULL)
      END_FLAG = 1;
  }

  LOG(LL_INFO, ("Reached after end_ptr check!"));
  if (new_start != NULL) {
    if (end < new_start) {
      if (old_start != NULL) {
        strncpy(parts, b.p, (end - b.p));
        strcat(message, parts);
        mbuf_remove(&lb, (end - lb.buf));
      }
    }
    while (new_start <= end && new_start != NULL) {
      LOG(LL_INFO, ("New string %.*s", (int)b.len, b.p));
      old_start = new_start;
      b = mg_mk_str_n(new_start + 1, lb.len - (new_start - b.p));
      
      new_start = (char *) mg_strchr(b, '#');
    }
    // from old_start to end
    strncpy(parts, old_start + 1, end - old_start - 1);
    strcat(message, parts);
  } else {
    if (old_start != NULL) {
      // from buffer start to end
      strncpy(parts, b.p, end - b.p);
      strcat(message, parts);
    }
  }
  
  if (END_FLAG) {
    LOG(LL_INFO, ("Parsed message: %s", message));
    old_start = NULL;
    new_start = NULL;
    end = NULL;
    message[0] = '\0';
    END_FLAG = 0;
  }
  // if (start != NULL && end != NULL) {
  //   size_t llen = end - start;
  //   if (llen == 0) return;
  //   struct mg_str line = mg_mk_str_n(start, llen);
  //   //  * Now do something useful with "line" which contains the line data,
  //   /*
  //   * NUL-terminated for her pleasure.
  //   */
  //   LOG(LL_INFO, ("UART RPC%d> '%.*s'", uart_no, (int) line.len, line.p));
  //   mbuf_remove(&lb, llen + 1);
  // } 
  // else if (start != NULL && end == NULL) {
  //   strcat(message, );
  // } else if (start == NULL &&  != NULL) {
  //   le = lf;
  // } 
  // else {
  //   le = MIN(end, lf);
  // }
  // *le = '\0';
  // LOG(LL_INFO, ("ALL Bytes %d> '%.*s'", uart_no, (int) b.len, b.p));

  // if (mg_vcasecmp(&line, "hi") == 0) {
  //   mgos_uart_printf(UART_NO, "Hello!\r\n");
  // } else {
  //   mgos_uart_printf(UART_NO, "You said '%.*s'.\r\n", (int) line.len, line.p);
  // }
  /* Finally, remove the line data from the buffer. */
  // size upto end
  mbuf_remove(&lb, (end - lb.buf));
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
