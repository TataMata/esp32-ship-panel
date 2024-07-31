/*
MIT License

Copyright (c) 2022 Sukesh Ashok Kumar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Display.hpp"
#include "log_tag.hpp"
#include <functional>
#include "Lcd.hpp"
#include "Periodic.hpp"

#define LV_TICK_PERIOD_MS 1
#define BUFF_SIZE 40
#define LVGL_DOUBLE_BUFFER

namespace ship {

static void lv_tick_task(void *arg) {
  (void)arg;
  lv_tick_inc(LV_TICK_PERIOD_MS);
}

} // namespace ship

using namespace ship;

Display &Display::instance() {
  using DisplaySingleton =
      Loki::SingletonHolder<Display, Loki::CreateStatic, Loki::NoDestroy>;

  return DisplaySingleton::Instance();
}

Display::Display() {}

esp_err_t Display::init(const std::shared_ptr<Lcd> &tft) {
  _lcd = tft;
  assert(_lcd != nullptr && "lcd is null");
  lv_init();     // Initialize lvgl

  Lcd& lcd = *_lcd;

  lcd.setRotation(2);
  lcd.setColorDepth(16);
  int brightness = 128; // prefs.getUInt("brightness", 128);
  ESP_LOGI(TAG, "Setting brightness: %d", brightness);
#if defined(LVGL_DOUBLE_BUFFER)
  // EXT_RAM_BSS_ATTR lv_color_t * buf1 = (lv_color_t *)malloc(screenWidth *
  // BUFF_SIZE * sizeof(lv_color_t)); EXT_RAM_BSS_ATTR lv_color_t * buf2 =
  // (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));

  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(
      screenWidth * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(
      screenWidth * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * BUFF_SIZE);
#else
  // EXT_RAM_BSS_ATTR lv_color_t * buf1 = (lv_color_t *)malloc(screenWidth *
  // BUFF_SIZE * sizeof(lv_color_t));
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(
      screenWidth * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * BUFF_SIZE);
#endif

  /*** LVGL : Setup & Initialize the display device driver ***/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = Display::flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.sw_rotate = 1;
  disp = lv_disp_drv_register(&disp_drv);

  //*** LVGL : Setup & Initialize the input device driver ***
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = Display::touchpadRead;
  lv_indev_drv_register(&indev_drv);

  _periodic = std::make_unique<Periodic>();

  return ESP_OK;
}

// Display callback to flush the buffer to screen
void Display::flush(lv_disp_drv_t *disp, const lv_area_t *area,
                    lv_color_t *color_p) {
  Display &instance = Display::instance();
  instance._lcd->write(area, color_p);

  lv_disp_flush_ready(disp);
}

// Touchpad callback to read the touchpad
void Display::touchpadRead(lv_indev_drv_t *indev_driver,
                           lv_indev_data_t *data) {
  Display &instance = Display::instance();
  uint16_t touchX, touchY;
  bool touched = instance._lcd->getTouch(touchX, touchY);

  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    // Set the coordinates
    data->point.x = touchX;
    data->point.y = touchY;
  }
}
