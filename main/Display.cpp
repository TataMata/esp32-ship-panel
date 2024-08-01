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
#include <vector>

#define LV_TICK_PERIOD_MS 1
#define BUFF_SIZE 40
#define LVGL_DOUBLE_BUFFER

using namespace ship;
using namespace lvgl::core;
using namespace lvgl::draw;

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
  lcd.setBrightness(brightness);
  const uint16_t fb_size = TFT_WIDTH * BUFF_SIZE;
  const uint16_t alloc_size = fb_size * sizeof(lv_color_t);
#if defined(LVGL_DOUBLE_BUFFER)
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(alloc_size, MALLOC_CAP_DMA);
  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(alloc_size, MALLOC_CAP_DMA);

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, fb_size);
#else
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(alloc_size, MALLOC_CAP_DMA);
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, fb_size);
#endif

  /*** LVGL : Setup & Initialize the display device driver ***/
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.user_data = static_cast<void *>(this);
  auto cb = [](lv_disp_drv_t *drv, const lv_area_t *area,
              lv_color_t *color_map) {
    auto obj = reinterpret_cast<Display *>(drv->user_data);
    obj->flush(area, color_map);
  };
  disp_drv.flush_cb = cb;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.sw_rotate = 1;
  lv_obj = LvPointerType(lv_disp_drv_register(&disp_drv));

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
void Display::flush(const lv_area_t *area, lv_color_t *color_p) {
  _lcd->write(area, color_p);

  flush_ready();
}

void Display::flush_ready() {
  lv_disp_flush_ready(&disp_drv);
}

  // Touchpad callback to read the touchpad
  void Display::touchpadRead(lv_indev_drv_t * indev_driver,
                             lv_indev_data_t * data) {
    Display &instance = Display::instance();
    uint16_t touchX, touchY;
    bool touched = instance._lcd->getTouch(&touchX, &touchY);

    if (!touched) {
      data->state = LV_INDEV_STATE_REL;
    } else {
      data->state = LV_INDEV_STATE_PR;

      // Set the coordinates
      data->point.x = touchX;
      data->point.y = touchY;
    }
}

void Display::update_driver() {
  lv_disp_drv_update(this->raw_ptr(), &this->disp_drv);
}

void Display::set_default() { lv_disp_set_default(this->raw_ptr()); }

bool Display::is_default() const {
  return lv_disp_get_default() == this->raw_ptr();
}

lv_coord_t Display::get_hor_res() const {
  return lv_disp_get_hor_res(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_ver_res() const {
  return lv_disp_get_ver_res(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_physical_hor_res() const {
  return lv_disp_get_physical_hor_res(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_physical_ver_res() const {
  return lv_disp_get_physical_ver_res(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_offset_x() const {
  return lv_disp_get_offset_x(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_offset_y() const {
  return lv_disp_get_offset_y(const_cast<lv_disp_t *>(this->raw_ptr()));
}

bool Display::get_antialiasing() const {
  return lv_disp_get_antialiasing(const_cast<lv_disp_t *>(this->raw_ptr()));
}

lv_coord_t Display::get_dpi() const { return lv_disp_get_dpi(this->raw_ptr()); }

void Display::set_rotation(lv_disp_rot_t rotation) {
  lv_disp_set_rotation(this->raw_ptr(), rotation);
}

lv_disp_rot_t Display::get_rotation() const {
  return lv_disp_get_rotation(const_cast<lv_disp_t *>(this->raw_ptr()));
}

Object Display::get_scr_act() const {
  return Object(lv_disp_get_scr_act(const_cast<lv_disp_t *>(this->raw_ptr())),
                false);
}

Object Display::get_scr_prev() const {
  return Object(lv_disp_get_scr_prev(const_cast<lv_disp_t *>(this->raw_ptr())),
                false);
}

Object Display::get_layer_top() const {
  return Object(lv_disp_get_layer_top(const_cast<lv_disp_t *>(this->raw_ptr())),
                false);
}

Object Display::get_layer_sys() const {
  return Object(lv_disp_get_layer_sys(const_cast<lv_disp_t *>(this->raw_ptr())),
                false);
}

void Display::set_theme(Theme &th) {
  lv_disp_set_theme(this->raw_ptr(), th.raw_ptr());
}

Theme Display::get_theme() const {
  return Theme(lv_disp_get_theme(const_cast<lv_disp_t *>(this->raw_ptr())),
               false);
}

void Display::set_bg_color(lv_color_t color) {
  lv_disp_set_bg_color(this->raw_ptr(), color);
}

void Display::set_bg_image(const ImageDescriptor &img) {
  lv_disp_set_bg_image(this->raw_ptr(),
                       static_cast<const void *>(img.raw_ptr()));
}

void Display::set_bg_image(const std::string &path) {
  lv_disp_set_bg_image(this->raw_ptr(),
                       static_cast<const void *>(path.c_str()));
}

void Display::set_bg_opa(lv_opa_t value) {
  lv_disp_set_bg_opa(this->raw_ptr(), value);
}

void Display::enable_invalidation(bool en) {
  lv_disp_enable_invalidation(this->raw_ptr(), en);
}

bool Display::is_invalidation_enabled() {
  return lv_disp_is_invalidation_enabled(this->raw_ptr());
}

uint32_t Display::get_inactive_time() const {
  return lv_disp_get_inactive_time(this->raw_ptr());
}

void Display::trig_activity() { lv_disp_trig_activity(this->raw_ptr()); }

void Display::clean_dcache() { lv_disp_clean_dcache(this->raw_ptr()); }
