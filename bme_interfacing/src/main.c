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
#include "mgos_bme280.h"

void log_bme_data(void *arg) {
  struct mgos_bme280* my_bme = mgos_bme280_i2c_create(0x76);

  double temp = mgos_bme280_read_temperature(my_bme);
  double pressure = mgos_bme280_read_pressure(my_bme);
  double humidity = mgos_bme280_read_humidity(my_bme);

  LOG(LL_INFO, ("Temperature: %.2lf, Humidity: %.2lf, Pressure: %.2lf", temp, humidity, pressure));
  mgos_bme280_delete(my_bme);
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_set_timer(5000, true, log_bme_data, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
