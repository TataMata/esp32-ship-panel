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

#include "Periodic.hpp"
#include <hal/lv_hal_tick.h>

using namespace ship;

#define LV_TICK_PERIOD_MS 1

static void lv_tick_task(void *arg) {
//   (void)arg;
  lv_tick_inc(LV_TICK_PERIOD_MS);
}

Periodic::Periodic() : _timer(nullptr) {
  /* Create and start a periodic timer interrupt to call lv_tick_inc */
  const esp_timer_create_args_t lv_periodic_timer_args = {
      .callback = &lv_tick_task,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "periodic_gui",
      .skip_unhandled_events = true};

  ESP_ERROR_CHECK(esp_timer_create(&lv_periodic_timer_args, &_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(_timer, LV_TICK_PERIOD_MS * 1000));
}

Periodic::~Periodic() {
  if (_timer)
    esp_timer_delete(_timer);
}