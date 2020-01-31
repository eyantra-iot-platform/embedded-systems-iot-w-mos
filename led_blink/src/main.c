
#include "mgos.h"

int LED_PIN = 18;


enum mgos_app_init_result mgos_app_init(void) {

	//Sets LED_PIN as Output pin. 0 indicates LOW output level initially
	mgos_gpio_setup_output(LED_PIN, 0);

	//Set LED_PIN output level HIGH
	mgos_gpio_write(LED_PIN, 1);
	
	//Add a delay. Note, delay is in milliseconds
	mgos_msleep(1000);

	//Set LED_PIN output level LOW
	mgos_gpio_write(LED_PIN, 0);


  return MGOS_APP_INIT_SUCCESS;
}
