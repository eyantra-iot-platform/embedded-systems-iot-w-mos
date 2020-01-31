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

volatile int pulse_frequency = 0;
volatile int pulse_per_second = 0;

float litre_per_min;

float calc_litres(int pulse_frequency) {
  // https://saiersensor.en.alibaba.com/product/60539487700-803861114/
  // SEN_HZ21WA_flow_sensor_liquid_level_sensor_magnetic_water_flow_sensor.html
  // 1L Water = 516 Pulse
  // OR https://www.e-gizmo.net/oc/kits%20documents/SEN-HZ21WA%20Flowmeter/SEN-HZ21WA%20Flowmeter.pdf
  // flow_freq * 2.25 / 1000;
  return pulse_frequency / 516.0;
}

void calc_rate(void* arg) {
  // Litres per min
  mgos_gpio_disable_int(PIN);
  litre_per_min = (pulse_per_second * 60.0) / 1000;
  pulse_per_second = 0;
  (void)arg;
  mgos_gpio_enable_int(PIN);
}

static void isr_cb(int pin, void *arg) {
  pulse_frequency++;
  pulse_per_second++;
}

static void report(void *arg) {
  mgos_gpio_disable_int(PIN);
  LOG(LL_INFO, ("Pulse per second: %d & Total pulse count: %d", pulse_per_second, pulse_frequency));
  LOG(LL_INFO, ("Quantity: %.2f Litres", calc_litres(pulse_frequency)));
  LOG(LL_INFO, ("Rate: %.2f Litres/min", litre_per_min));
  mgos_gpio_enable_int(PIN);
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_set_timer(3000 /* ms */, MGOS_TIMER_REPEAT, report, NULL);
  mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, calc_rate, NULL);
  mgos_gpio_setup_input(PIN, MGOS_GPIO_PULL_UP);
  mgos_gpio_set_int_handler_isr(PIN, MGOS_GPIO_INT_EDGE_NEG, isr_cb, NULL);
  mgos_gpio_enable_int(PIN);  
  return MGOS_APP_INIT_SUCCESS;
}
