/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

#include "mgos.h"
#include "mgos_app.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos_uart.h"

#define UART_NO 1

static void timer1_cb(void *arg) {
  
  mgos_uart_printf(UART_NO, "R\r\n");
  
}

static void timer2_cb(void *arg) {
  
  mgos_uart_printf(UART_NO, "W\r\n");
  
}

static void timer3_cb(void *arg) {
  
  mgos_uart_printf(UART_NO, "G\r\n");
  
}

/*
 * Dispatcher can be invoked with any amount of data (even none at all) and
 * at any time. Here we demonstrate how to process input line by line.
 */
static void uart_dispatcher(int uart_no, void *arg) {
  
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
  
  
  /*
   * Now do something useful with "line" which contains the line data,
   */
  
  LOG(LL_INFO, ("UART%d> '%.*s'", uart_no, (int) line.len, line.p));
  
  // if (mg_vcasecmp(&line, "hi") == 0) {
  //   mgos_uart_printf(UART_NO, "Hello!\r\n");
  // } else {
  //   mgos_uart_printf(UART_NO, "You said '%.*s'.\r\n", (int) line.len, line.p);
  // }
  
  /* Finally, remove the line data from the buffer. */
  mbuf_remove(&lb, llen + 1);
  
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  
  //We create a uart configuration struct variable
  struct mgos_uart_config ucfg;

  //We set the default parameters to the configuration
  mgos_uart_config_set_defaults(UART_NO, &ucfg);
  
  /*
   * At this point it is possible to adjust baud rate, pins and other settings.
   * 115200 8-N-1 is the default mode, but we set it anyway
   */
  
  ucfg.baud_rate = 115200;
  ucfg.num_data_bits = 8;
  ucfg.parity = MGOS_UART_PARITY_NONE;
  ucfg.stop_bits = MGOS_UART_STOP_BITS_1;
  
  //We set the GPIO pins to be used for communication
  ucfg.dev.rx_gpio = 33;
  ucfg.dev.tx_gpio = 32;
  ucfg.dev.cts_gpio = -1;
  ucfg.dev.rts_gpio = -1;

  //We will return an error if the configuration isn't right
  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    return MGOS_APP_INIT_ERROR;
  }

  //We use multiple software timers to send data
  mgos_set_timer(250, MGOS_TIMER_REPEAT, timer1_cb, NULL);
  mgos_set_timer(330, MGOS_TIMER_REPEAT, timer2_cb, NULL);
  mgos_set_timer(470, MGOS_TIMER_REPEAT, timer3_cb, NULL);

  //Set a callback function that gets called when there is data in the input buffer
  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);
  mgos_uart_set_rx_enabled(UART_NO, true);

  
  return MGOS_APP_INIT_SUCCESS;
}
