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
#include <lvglpp/lv_wrapper.h>
#include <lvglpp/core/object.h>
#include <lvglpp/core/theme.h>
#include <lvglpp/draw/image.h>
// class Preferences;

namespace ship {

class Lcd;
class Periodic;

/**
 * Display class is meant to be a singleton and to manage the display - lvgl and
 * lcd
 */

class Display : public lvgl::PointerWrapper<lv_disp_t, lv_disp_remove> {
public:
  const uint16_t screenWidth = TFT_WIDTH;
  const uint16_t screenHeight = TFT_HEIGHT;

  static Display &instance();
  esp_err_t init(const std::shared_ptr<Lcd> &tft);

  /** \fn void set_default()
   *  \brief Sets display as default.
   */
  void set_default();

  /** \fn bool is_default() const
   *  \brief Tells if display is set as default.
   *  \returns true if display is default display, false otherwise.
   */
  bool is_default() const;

  /** \fn lv_coord_t get_hor_res() const
   *  \brief Gets horizontal display resolution.
   *  \returns horizontal resolution.
   */
  lv_coord_t get_hor_res() const;

  /** \fn lv_coord_t get_ver_res() const
   *  \brief Gets vertical display resolution.
   *  \returns vertical resolution.
   */
  lv_coord_t get_ver_res() const;

  /** \fn lv_coord_t get_physical_hor_res() const
   *  \brief Gets physical horizontal display resolution.
   *  \returns physical horizontal resolution.
   */
  lv_coord_t get_physical_hor_res() const;

  /** \fn lv_coord_t get_physical_ver_res() const
   *  \brief Gets physical vertical display resolution.
   *  \returns physical vertical resolution.
   */
  lv_coord_t get_physical_ver_res() const;

  /** \fn lv_coord_t get_offset_x() const
   *  \brief Gets horizontal offset.
   *  \returns horizontal offset.
   */
  lv_coord_t get_offset_x() const;

  /** \fn lv_coord_t get_offset_y() const
   *  \brief Gets vertical offset.
   *  \returns vertical offset.
   */
  lv_coord_t get_offset_y() const;

  /** \fn bool get_antialiasing() const
   *  \brief Tells if antialiasing is enabled.
   *  \returns true if antialiasing is enabled, false otherwise.
   */
  bool get_antialiasing() const;

  /** \fn lv_coord_t get_dpi() const
   *  \brief Gets display DPI.
   *  \returns display DPI value.
   */
  lv_coord_t get_dpi() const;

  /** \fn void set_rotation(lv_disp_rot_t rotation)
   *  \brief Sets display rotation.
   *  \param rotation: display rotation code.
   */
  void set_rotation(lv_disp_rot_t rotation);

  /** \fn lv_disp_rot_t get_rotation() const
   *  \brief Gets display rotation.
   *  \returns display rotation code.
   */
  lv_disp_rot_t get_rotation() const;

  /** \fn Object get_scr_act() const
   *  \brief Gets active screen on this display.
   *  \returns object representing active screen.
   */
  lvgl::core::Object get_scr_act() const;

  /** \fn Object get_scr_prev() const
   *  \brief Gets the screen before the active screen on this display.
   *  \returns object representing previous screen.
   */
  lvgl::core::Object get_scr_prev() const;

  /** \fn Object get_layer_top() const
   *  \brief Gets access to the top layer.
   *  \returns object representing the top layer.
   */
  lvgl::core::Object get_layer_top() const;

  /** \fn Object get_layer_sys() const
   *  \brief Gets access to the sys layer.
   *  \returns object representing the sys layer.
   */
  lvgl::core::Object get_layer_sys() const;

  /** \fn void set_theme(Theme & th)
   *  \brief Sets active theme on display.
   *  \param th: theme object.
   */
  void set_theme(lvgl::core::Theme &th);

  /** \fn Theme get_theme() const
   *  \brief Gets active theme.
   *  \returns theme object.
   */
  lvgl::core::Theme get_theme() const;

  /** \fn void set_bg_color(lv_color_t color)
   *  \brief Sets display background color.
   *  \param color: color to set.
   */
  void set_bg_color(lv_color_t color);

  /** \fn void set_bg_image(const draw::ImageDescriptor & img)
   *  \brief Sets display background image.
   *  \param img: image buffer.
   */
  void set_bg_image(const lvgl::draw::ImageDescriptor &img);

  /** \fn void set_bg_image(const std::string & path)
   *  \brief Sets display background image.
   *  \param path: path to image file.
   */
  void set_bg_image(const std::string &path);

  /** \fn void set_bg_opa(lv_opa_t value)
   *  \brief Sets display background opacity.
   *  \param value: opacity value.
   */
  void set_bg_opa(lv_opa_t value);

  /** \brief Set state of display invalidation.
   *  \param en: if true, invalidation is enabled; if false, disabled.
   */
  void enable_invalidation(bool en);

  /** \brief Get state of display invalidation.
   *  \returns true if invalidation is enabled, false otherwise.
   */
  bool is_invalidation_enabled();

  /** \fn uint32_t get_inactive_time() const
   *  \brief Gets elapsed time since last activity.
   *  \returns elapsed time in ms.
   */
  uint32_t get_inactive_time() const;

  /** \fn void trig_activity()
   *  \brief Triggers activity on the display.
   */
  void trig_activity();

  /** \fn void clean_dcache()
   *  \brief Cleans display cache.
   */
  void clean_dcache();

protected:
  Display();
  ~Display() = default;
  static void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
  void flush(const lv_area_t *area, lv_color_t *color_p);
  void update_driver();
  void flush_ready();

  lv_disp_drv_t disp_drv;
  std::shared_ptr<Lcd> _lcd;
  std::unique_ptr<Periodic> _periodic;
  lv_disp_draw_buf_t draw_buf;

  friend struct Loki::CreateStatic<Display>;
};

} // namespace ship

#endif // __DISPLAY_HPP