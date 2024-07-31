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

#include "GuiThread.hpp"
#include "sdkconfig.h"
#include <lvgl.h>

using namespace ship;

GuiThread::GuiThread() {
  _semaphore = xSemaphoreCreateMutex();
  if (!_semaphore) {
    ESP_LOGE(TAG, "Create mutex for LVGL failed");
    throw std::runtime_error("Create mutex for LVGL failed");
  }

  void *const parameters = this;
#if CONFIG_FREERTOS_UNICORE == 0
  int err = xTaskCreatePinnedToCore(task_handler, "lv gui", 1024 * 8,
                                    parameters, 3, &_task_handle, 1);
#else
  int err = xTaskCreatePinnedToCore(task_handler, "lv gui", 1024 * 8,
                                    parameters, 3, &_task_handle, 0);
#endif
}

GuiThread::~GuiThread() {
  vTaskDelete(_task_handle);
  vSemaphoreDelete(_semaphore);
}

void GuiThread::lock() {
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  if (_task_handle != task) {
    xSemaphoreTake(_semaphore, portMAX_DELAY);
  }
}

void GuiThread::unlock() {
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  if (_task_handle != task) {
    xSemaphoreGive(_semaphore);
  }
}

void GuiThread::task_handler(void *arg) {
  ESP_LOGI(TAG, "Start to run LVGL");
  GuiThread *gui = reinterpret_cast<GuiThread *>(arg);
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Try to take the semaphore, call lvgl related function on success */
    if (pdTRUE == xSemaphoreTake(gui->_semaphore, portMAX_DELAY)) {
      lv_task_handler();
      xSemaphoreGive(gui->_semaphore);
    }
  }
}
