#include "mgos.h"
#include "mgos_mqtt.h"

#define R_PIN 23

void setLED(int r_lvl, int b_lvl, int g_lvl) {
  mgos_gpio_write(R_PIN, r_lvl);
  mgos_gpio_write(G_PIN, b_lvl);
  mgos_gpio_write(B_PIN, g_lvl);
}

void glowRed() {
  LOG(LL_INFO, ("Glowing Red"));
  setLED(0, 1, 1);
}

void sendData() {
  mgos_mqtt_pub("v1/devices/me/telemetry", );
}

static void ev_handler(struct mg_connection *c, int ev, void *p, void *user_data) {
  struct mg_mqtt_message *msg = (struct mg_mqtt_message *)p;
  if (ev == MG_EV_MQTT_CONNACK) {
    LOG(LL_INFO, ("CONNACK: %d", msg->connack_ret_code));
  }
}

enum mgos_app_init_result mgos_app_init(void) {
  // mgos_set_timer(1000, false, sendData, NULL);
  mgos_mqtt_add_global_handler(ev_handler, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
