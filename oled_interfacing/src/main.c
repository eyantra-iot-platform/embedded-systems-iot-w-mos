#include "mgos.h"
#include "sh1106.h"

enum mgos_app_init_result mgos_app_init(void) {
  // bool oled_init = mgos_sh1106_init();
  // if (!oled_init)
  //   return MGOS_APP_INIT_ERROR;
  // struct mgos_sh1106* oled = mgos_sh1106_get_global();
  // mgos_sh1106_draw_string(oled, mgos_sys_config_get_x(), mgos_sys_config_get_y(), "Hello OLED!");
  // // mgos_sh1106_draw_circle(oled, 20, 20, 10, SH1106_COLOR_WHITE);
  // mgos_sh1106_fill_rectangle(oled, 20, 20, 50, 30, SH1106_COLOR_WHITE);
  // mgos_sh1106_refresh(oled, false);
  // return MGOS_APP_INIT_SUCCESS;
  bool oled_init = mgos_sh1106_init();
  if (!oled_init)
    return MGOS_APP_INIT_ERROR;
  struct mgos_sh1106* oled = mgos_sh1106_get_global();
  mgos_sh1106_clear(oled);
  mgos_sh1106_draw_string(oled, mgos_sys_config_get_x(), mgos_sys_config_get_y(), "Lorem ipsum dolor sit");
  mgos_sh1106_draw_circle(oled, 20, 20, 10, SH1106_COLOR_WHITE);
  // mgos_sh1106_fill_rectangle(oled, 20, 20, 50, 30, sh1106_COLOR_WHITE);
  mgos_sh1106_draw_pixel(oled, 64, 32, SH1106_COLOR_WHITE);
  mgos_sh1106_flip_display(oled, true, true);
  // mgos_sh1106_invert_display(oled, true);
  mgos_sh1106_refresh(oled, false);
  return MGOS_APP_INIT_SUCCESS;
}
