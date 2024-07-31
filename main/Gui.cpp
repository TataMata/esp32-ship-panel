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

#include "Gui.hpp"
#include "GuiThread.hpp"
#include "events/gui_events.hpp"
#include "log_tag.hpp"
#include <esp_partition.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <mutex>

LV_IMG_DECLARE(dev_bg)
// LV_IMG_DECLARE(tux_logo)

// LV_FONT_DECLARE(font_7seg_64)
// LV_FONT_DECLARE(font_7seg_60)
// LV_FONT_DECLARE(font_7seg_58)
LV_FONT_DECLARE(font_7seg_56)

// LV_FONT_DECLARE(font_robotomono_12)
LV_FONT_DECLARE(font_robotomono_13)

LV_FONT_DECLARE(font_fa_14)
#define FA_SYMBOL_BLE "\xEF\x8A\x94"      // 0xf294
#define FA_SYMBOL_SETTINGS "\xEF\x80\x93" // 0xf0ad

/*********************
 *      DEFINES
 *********************/
#define HEADER_HEIGHT 0
#define FOOTER_HEIGHT 0

using namespace ship;

static const lv_font_t *font_large;
static const lv_font_t *font_normal;
static const lv_font_t *font_symbol;
static const lv_font_t *font_fa;
static const lv_font_t *font_xl;
static const lv_font_t *font_xxl;

Gui &Gui::instance() {
  using GuiSingleton =
      Loki::SingletonHolder<Gui, Loki::CreateStatic, Loki::NoDestroy>;

  return GuiSingleton::Instance();
}

void Gui::init() {
  screen_h = lv_obj_get_height(lv_scr_act());
  screen_w = lv_obj_get_width(lv_scr_act());

  theme.init();
  bool darkTheme = true;
  theme.setTheme(darkTheme);

  setup_styles();
}

void Gui::setup_styles() {
  font_symbol = &lv_font_montserrat_14;
  font_normal = &lv_font_montserrat_14;
  font_large = &lv_font_montserrat_16;
  font_xl = &lv_font_montserrat_24;
  font_xxl = &lv_font_montserrat_32;
  font_fa = &font_fa_14;

  // UI ISLANDS
  lv_style_init(&style_ui_island);
  lv_style_set_bg_color(&style_ui_island, theme.getBgColor());
  lv_style_set_bg_opa(&style_ui_island, LV_OPA_80);
  lv_style_set_border_color(&style_ui_island, theme.getBgColor());
  // lv_style_set_border_opa(&style_ui_island, LV_OPA_80);
  lv_style_set_border_width(&style_ui_island, 1);
  lv_style_set_radius(&style_ui_island, 10);

  setup_background_style();
}

void Gui::setup_background_style() {
  /* CONTENT CONTAINER BACKGROUND */
  lv_style_init(&style_content_bg);
  lv_style_set_bg_opa(&style_content_bg, LV_OPA_50);
  lv_style_set_radius(&style_content_bg, 0);

  // Enabling wallpaper image slows down scrolling perf etc...
#if defined(CONFIG_WALLPAPER_IMAGE)
    // Image Background
    // CF_INDEXED_8_BIT for smaller size - resolution 480x480
    // NOTE: Dynamic loading bg from SPIFF makes screen perf bad
    if (lv_fs_is_ready('F'))
    { // NO SD CARD load default
        ESP_LOGW(TAG, "Loading - F:/bg/dev_bg9.bin");
        lv_style_set_bg_img_src(&style_content_bg, "F:/bg/dev_bg9.bin");
    }
    else
    {
        ESP_LOGW(TAG, "Loading - from firmware");
        lv_style_set_bg_img_src(&style_content_bg, &dev_bg);
    }
    // lv_style_set_bg_img_src(&style_content_bg, &dev_bg);
    //  lv_style_set_bg_img_opa(&style_content_bg,LV_OPA_50);
#else
    ESP_LOGW(TAG, "Using Gradient");
    // Gradient Background
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_make(31, 32, 34);
    grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
    grad.stops[0].frac = 150;
    grad.stops[1].frac = 190;
    lv_style_set_bg_grad(&style_content_bg, &grad);
#endif
}

void Gui::show() {
  guiThread = std::make_unique<GuiThread>();

  std::lock_guard<GuiThread> lock(*guiThread);
  init();
  // screen_container is the root of the UX
  screen_container = lv_obj_create(NULL);

  lv_obj_set_size(screen_container, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_pad_all(screen_container, 0, 0);
  lv_obj_align(screen_container, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_border_width(screen_container, 0, 0);
  lv_obj_set_scrollbar_mode(screen_container, LV_SCROLLBAR_MODE_OFF);

  // Gradient / Image Background for screen container
  lv_obj_add_style(screen_container, &style_content_bg, 0);

  // CONTENT CONTAINER
  content_container = lv_obj_create(screen_container);
  lv_obj_set_size(content_container, screen_w,
                  screen_h - HEADER_HEIGHT - FOOTER_HEIGHT);
  lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, HEADER_HEIGHT);
  lv_obj_set_style_border_width(content_container, 0, 0);
  lv_obj_set_style_bg_opa(content_container, LV_OPA_TRANSP, 0);

  lv_obj_set_flex_flow(content_container, LV_FLEX_FLOW_COLUMN);

  create_page_home(content_container);

  // Load main screen with animation
  // lv_scr_load(screen_container);
  lv_scr_load_anim(screen_container, LV_SCR_LOAD_ANIM_FADE_IN, 1000, 100, true);
  // Send default page load notification => HOME
  lv_msg_send(MSG_PAGE_HOME, NULL);
}

void Gui::create_page_home(lv_obj_t *parent) {
  /* HOME PAGE PANELS */
  panel = tux_panel_create(parent, "", 130);
  lv_obj_add_style(panel, &style_ui_island, 0);
  // tux_panel_devinfo(parent);
}