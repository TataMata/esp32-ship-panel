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

#include "Theme.hpp"

using namespace ship;

Theme::Theme(bool isDark) : dark(isDark) {
}

void Theme::init() { _colorPrimary = lv_palette_main(LV_PALETTE_BLUE); }

void Theme::setTheme(bool dark) {
  if (dark) {
    _colorSecondary = lv_palette_main(LV_PALETTE_GREEN);
    bg_color = lv_palette_darken(LV_PALETTE_GREY, 5);
  } else {
    _colorSecondary = lv_palette_main(LV_PALETTE_RED);
    bg_color = lv_color_hex(0xBFBFBD);
  }
}