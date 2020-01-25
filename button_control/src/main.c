#include "mgos.h"

#define R_PIN 23
#define G_PIN 22
#define B_PIN 21
#define PRG_BTN 0

int r_pin_state = 0;

void button_handler (int pin, void *arg) {
  LOG(LL_INFO, ("Toggling"));
  mgos_gpio_set_mode(23, MGOS_GPIO_MODE_OUTPUT);
  // toggle
  r_pin_state = (r_pin_state == 0) ? 1 : 0;
  mgos_gpio_write(23, r_pin_state);
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_gpio_set_mode(PRG_BTN, MGOS_GPIO_MODE_INPUT);
  mgos_gpio_set_int_handler(PRG_BTN, MGOS_GPIO_INT_EDGE_NEG, button_handler, NULL);
  mgos_gpio_enable_int(PRG_BTN);
  
  // mgos_set_timer(1000, false, sendData, NULL);
  // mgos_mqtt_add_global_handler(ev_handler, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
