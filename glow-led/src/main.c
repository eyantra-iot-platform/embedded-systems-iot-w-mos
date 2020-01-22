#include "mgos.h"

#define R_PIN 23
#define G_PIN 22
#define B_PIN 21

int i = 0;
const int n_colors = 3;

void setLED(int r_lvl, int b_lvl, int g_lvl) {
  mgos_gpio_write(R_PIN, r_lvl);
  mgos_gpio_write(G_PIN, b_lvl);
  mgos_gpio_write(B_PIN, g_lvl);
}

void glowRed() {
  LOG(LL_INFO, ("Glowing red"));
  setLED(0, 1, 1);
}

void glowGreen() {
  LOG(LL_INFO, ("Glowing green"));
  setLED(1, 0, 1);
}

void glowBlue() {
  LOG(LL_INFO, ("Glowing blue"));
  setLED(1, 1, 0);
}

void blink() {
  int color = i++ % n_colors;
  if (color == 0) {
    glowRed();
  }
  else if (color == 1) {
    glowGreen();
  }
  else if (color == 2) {
    glowBlue();
  }
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_gpio_set_mode(R_PIN, MGOS_GPIO_MODE_OUTPUT);
  mgos_gpio_set_mode(G_PIN, MGOS_GPIO_MODE_OUTPUT);
  mgos_gpio_set_mode(B_PIN, MGOS_GPIO_MODE_OUTPUT);

  mgos_set_timer(mgos_sys_config_get_blink_app_time(), true, blink, NULL);
 
  return MGOS_APP_INIT_SUCCESS;
}
