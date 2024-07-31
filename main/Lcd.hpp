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

#ifndef __LCD_HPP
#define __LCD_HPP

#include "device_conf.hpp"
#include <lvgl.h>

namespace ship {

class Lcd {
public:
  Lcd();
  ~Lcd() = default;

  void setRotation(int rotation);

  void setColorDepth(int depth);

  void write(const lv_area_t *area, lv_color_t *color_p);

  bool getTouch(uint16_t& x, uint16_t& y);

private:
  LGFX lcd;
};

inline void Lcd::setRotation(int rotation) {
  lcd.setRotation(rotation);
}

inline void Lcd::setColorDepth(int depth) {
  lcd.setColorDepth(depth);
}

inline bool Lcd::getTouch(uint16_t& x, uint16_t& y) {
  return lcd.getTouch(&x, &y);
}

} // namespace ship

#endif // __LCD_HPP