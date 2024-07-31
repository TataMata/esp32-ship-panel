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

#ifndef __DISPLAY_HPP
#define __DISPLAY_HPP

#include <memory>
#include "device_conf.hpp"
#include <loki/Singleton.h>
#include <lvgl.h>

// class Preferences;

namespace ship {

class Lcd;
class Periodic;

/**
 * Display class is meant to be a singleton and to manage the display - lvgl and
 * lcd
 */

class Display {
public:
  const uint16_t screenWidth = TFT_WIDTH;
  const uint16_t screenHeight = TFT_HEIGHT;

  static Display &instance();
  esp_err_t init(const std::shared_ptr<Lcd> &tft);

private:
  Display();
  ~Display() = default;
  static void flush(lv_disp_drv_t *disp, const lv_area_t *area,
                    lv_color_t *color_p);
  static void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

  lv_disp_t *disp;
  std::shared_ptr<Lcd> _lcd;
  std::unique_ptr<Periodic> _periodic;
  lv_disp_draw_buf_t draw_buf;

  friend struct Loki::CreateStatic<Display>;
};

} // namespace ship

#endif // __DISPLAY_HPP