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

#ifndef __GUI_HPP
#define __GUI_HPP

#include "Theme.hpp"
#include "widgets/tux_panel.h"
#include <loki/Singleton.h>

namespace ship {

class GuiThread;

class Gui {
public:
  static Gui &instance();

  void init();
  void show();

private:
  Gui() = default;
  ~Gui() = default;

  void setup_styles();
  void setup_background_style();

  void create_page_home(lv_obj_t *parent);

  Theme theme;

  std::unique_ptr<GuiThread> guiThread;

  lv_obj_t *screen_container;
  lv_obj_t *content_container;
  lv_obj_t *panel;
  lv_obj_t *header;
  lv_obj_t *footer;
  lv_obj_t *content;

  lv_coord_t screen_h;
  lv_coord_t screen_w;

  lv_style_t style_content_bg;
  lv_style_t style_ui_island;
  friend struct Loki::CreateStatic<Gui>;
};

} // namespace ship

#endif // __GUI_HPP