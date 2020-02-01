#include "mgos.h"

#define IN11 21
#define IN12 22
#define EN1 23

void toggle_valve(void * arg) {
  LOG(LL_INFO, ("Toggling valve ..."));
  mgos_gpio_toggle(IN11);
  mgos_gpio_toggle(IN12);
  mgos_gpio_write(EN1, 1);
  mgos_msleep(mgos_sys_config_get_latch_duration());
  mgos_gpio_write(EN1, 0);
  (void)arg;
}


enum mgos_app_init_result mgos_app_init(void) {
  mgos_gpio_setup_output(IN11, 1);
  mgos_gpio_setup_output(IN12, 0);
  mgos_gpio_set_mode(EN1, MGOS_GPIO_MODE_OUTPUT);
  
  mgos_set_timer(4000 /* ms */, MGOS_TIMER_REPEAT, toggle_valve, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
