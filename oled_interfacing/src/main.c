#include "mgos.h"
#include "ssd1306.h"

enum mgos_app_init_result mgos_app_init(void) {
  bool oled_init = mgos_ssd1306_init();
  if (!oled_init)
    return MGOS_APP_INIT_ERROR;
  struct mgos_ssd1306* oled = mgos_ssd1306_get_global();
  mgos_ssd1306_draw_string(oled, mgos_sys_config_get_x(), mgos_sys_config_get_y(), "Hello OLED!");
  // mgos_ssd1306_draw_circle(oled, 20, 20, 10, SSD1306_COLOR_WHITE);
  mgos_ssd1306_fill_rectangle(oled, 20, 20, 50, 30, SSD1306_COLOR_WHITE);
  mgos_ssd1306_refresh(oled, false);
  return MGOS_APP_INIT_SUCCESS;
}
