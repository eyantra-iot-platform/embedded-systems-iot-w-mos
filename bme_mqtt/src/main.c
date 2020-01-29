#include "mgos.h"
#include "mgos_bme280.h"
#include "mgos_mqtt.h"

void log_bme_data_tb(void *arg) {
  char msg[100];
  int m_id;
  struct json_out jmo = JSON_OUT_BUF(msg, sizeof(msg));
  struct mgos_bme280* my_bme = mgos_bme280_i2c_create(0x76);

  double temp = mgos_bme280_read_temperature(my_bme);
  double pressure = mgos_bme280_read_pressure(my_bme) / 100;
  double humidity = mgos_bme280_read_humidity(my_bme);


  json_printf(&jmo, "{temperature: %.2f, humidity: %.2f, pressure: %.2f}}", temp, humidity, pressure);
  
  LOG(LL_INFO, ("Publishing %s", msg));
  
  m_id = mgos_mqtt_pub("v1/devices/me/telemetry", msg, strlen(msg), 1, 0);
  
  LOG(LL_INFO, ("Message ID: %d", m_id));
  mgos_bme280_delete(my_bme);

  (void)arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_set_timer(5000, true, log_bme_data_tb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
