#include "mgos.h"

int LED_PIN = 18;

static void timer_cb(void *arg) {
  
  //s_tick_tock is a boolean variable that we will toggle
  static bool s_tick_tock = false;
  
  //We log our output here. LL_INFO is pre-defined level
  LOG(LL_INFO,("%s", (s_tick_tock ? "Tick" : "Tock")));
  
  //variable is toggled here
  s_tick_tock = !s_tick_tock;

  //We use gpio_toggle to flip the value of the output pin
  mgos_gpio_toggle(LED_PIN);

}



enum mgos_app_init_result mgos_app_init(void) {

  //We set our GPIO pin as output
  mgos_gpio_setup_output(LED_PIN, 0);

  //We call software timer function
  mgos_set_timer(1000, MGOS_TIMER_REPEAT, timer_cb, NULL);
  
  return MGOS_APP_INIT_SUCCESS;
}