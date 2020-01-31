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

#include "mgos.h"
#define PIN 23

volatile int flow_frequency = 0;

float calc_litres(int flow_freq) {
  // https://saiersensor.en.alibaba.com/product/60539487700-803861114/
  // SEN_HZ21WA_flow_sensor_liquid_level_sensor_magnetic_water_flow_sensor.html
  // 1L Water = 516 Pulse
  return flow_freq / 516.0;
}

static void timer_cb(void *arg) {
  LOG(LL_INFO, ("Flow pulses: %d", flow_frequency));
  LOG(LL_INFO, ("Quantity in Litres: %.2f", calc_litres(flow_frequency)));
}

void isr_cb(int pin, void *arg) {
  flow_frequency++;
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_set_timer(5000 /* ms */, MGOS_TIMER_REPEAT, timer_cb, NULL);
  mgos_gpio_setup_input(PIN, MGOS_GPIO_PULL_UP);
  mgos_gpio_set_int_handler_isr(PIN, MGOS_GPIO_INT_EDGE_NEG, isr_cb, NULL);
  mgos_gpio_enable_int(PIN);
  return MGOS_APP_INIT_SUCCESS;
}
