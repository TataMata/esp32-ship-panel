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

#include "main.hpp"

#include "Display.hpp"
#include "Gui.hpp"
#include "Lcd.hpp"
#include "soc/rtc.h"
#include <esp_chip_info.h>
#include <esp_partition.h>
#include <esp_ota_ops.h>
#include <spi_flash_mmap.h>
#include <esp_flash.h>
#include <esp_spiffs.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <nvs_flash.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <log_tag.hpp>
#include <misc/lv_fs.h>

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(TUX_EVENTS);
#define READ_BUFF_SIZE 32

using namespace ship;

esp_err_t lv_print_readme_txt(const char *filename) {
  // F for Flash (SPIFF/FAT)
  // S for SD CARD
  if (lv_fs_is_ready(filename[0]))
    ESP_LOGE(TAG, "%c Drive is ready", filename[0]);
  else
    ESP_LOGE(TAG, "%c Drive is NOT ready", filename[0]);

  lv_fs_file_t f;
  lv_fs_res_t res;
  res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
  if (res != LV_FS_RES_OK) {
    ESP_LOGE(TAG, "Failed to open %s, %d", filename, res);
    return ESP_FAIL;
  }

  uint32_t read_num;
  uint8_t buf[READ_BUFF_SIZE];
  res = lv_fs_read(&f, buf, READ_BUFF_SIZE - 1, &read_num);
  if (res != LV_FS_RES_OK || read_num != READ_BUFF_SIZE - 1) {
    ESP_LOGE(TAG, "Failed to read from %s", filename);
    return ESP_FAIL;
  }

  // Display the read contents from the file
  ESP_LOGI(TAG, "Read from %s : %s", filename, buf);
  lv_fs_close(&f);
  return ESP_OK;
}

esp_err_t init_spiff() {
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                .partition_label = NULL,
                                .max_files = 5,
                                .format_if_mount_failed = false};

  // Use settings defined above to initialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return ESP_FAIL;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)",
             esp_err_to_name(ret));
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }
  return ESP_OK;

  // esp_vfs_spiffs_unregister(NULL);
  // ESP_LOGI(TAG, "SPIFFS unmounted");
}

extern "C" void app_main(void) {
  esp_log_level_set(TAG, ESP_LOG_DEBUG); // enable DEBUG logs for this App

  // Print device info
  ESP_LOGE(TAG, "\n%s", device_info().c_str());

  // Initialize NVS
  esp_err_t err = nvs_flash_init();

  // NVS partition contains new data format or unable to access
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  init_spiff();

  ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());

  Display &display = Display::instance();
  display.init(std::make_shared<Lcd>());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      TUX_EVENTS, ESP_EVENT_ANY_ID, tux_event_handler, NULL, NULL));

  lv_print_readme_txt("F:/readme.txt"); // SPIFF / FAT
//   lv_print_readme_txt("S:/readme.txt"); // SDCARD

  Gui &gui = Gui::instance();
  gui.show();
}

static const char *get_id_string(esp_event_base_t base, int32_t id) {
  // if (base == TUX_EVENTS) {
  switch (id) {
  case TUX_EVENT_DATETIME_SET:
    return "TUX_EVENT_DATETIME_SET";
  case TUX_EVENT_OTA_STARTED:
    return "TUX_EVENT_OTA_STARTED";
  case TUX_EVENT_OTA_IN_PROGRESS:
    return "TUX_EVENT_OTA_IN_PROGRESS";
  case TUX_EVENT_OTA_ROLLBACK:
    return "TUX_EVENT_OTA_ROLLBACK";
  case TUX_EVENT_OTA_COMPLETED:
    return "TUX_EVENT_OTA_COMPLETED";
  case TUX_EVENT_OTA_FAILED:
    return "TUX_EVENT_OTA_FAILED";
  case TUX_EVENT_OTA_ABORTED:
    return "TUX_EVENT_OTA_ABORTED";
  case TUX_EVENT_WEATHER_UPDATED:
    return "TUX_EVENT_WEATHER_UPDATED";
  case TUX_EVENT_THEME_CHANGED:
    return "TUX_EVENT_THEME_CHANGED";
  case TUX_EVENT_BRIGHTNESS_CHANGED:
    return "TUX_EVENT_BRIGHTNESS_CHANGED";
  default:
    return "TUX_EVENT_UNKNOWN";
  }
  //}
}

static void tux_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data) {
  ESP_LOGW(TAG, "tux_event_handler => %s:%s", event_base,
           get_id_string(event_base, event_id));
  if (event_base != TUX_EVENTS)
    return; // bye bye - me not invited :(
}

static std::string device_info() {
  std::string s_chip_info = "";

  /* Print chip information */
  esp_chip_info_t chip_info;
  uint32_t flash_size;
  esp_chip_info(&chip_info);

  // CPU Speed - 80Mhz / 160 Mhz / 240Mhz
  rtc_cpu_freq_config_t conf;
  rtc_clk_cpu_freq_get_config(&conf);

  multi_heap_info_t info;
  heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
  float psramsize =
      (info.total_free_bytes + info.total_allocated_bytes) / (1024.0 * 1024.0);

  const esp_partition_t *running = esp_ota_get_running_partition();
  esp_app_desc_t running_app_info;

  if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
    s_chip_info += fmt::format("Firmware Ver : {}\n", running_app_info.version);
    s_chip_info +=
        fmt::format("Project Name : {}\n", running_app_info.project_name);
    // running_app_info.time
    // running_app_info.date
  }
  s_chip_info += fmt::format("IDF Version  : {}\n\n", esp_get_idf_version());

  s_chip_info += fmt::format("Controller   : {} Rev.{}\n", CONFIG_IDF_TARGET,
                             chip_info.revision);
  // s_chip_info += fmt::format("\nModel         : {}",chip_info.model); //
  // esp_chip_model_t type
  s_chip_info +=
      fmt::format("CPU Cores    : {}\n",
                  (chip_info.cores == 2) ? "Dual Core" : "Single Core");
  s_chip_info += fmt::format("CPU Speed    : {}Mhz\n", conf.freq_mhz);
  if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
    s_chip_info += fmt::format(
        "Flash Size   : {}MB {}\n", flash_size / (1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "[embedded]"
                                                      : "[external]");
  }
  s_chip_info += fmt::format(
      "PSRAM Size   : {}MB {}\n", static_cast<int>(round(psramsize)),
      (chip_info.features & CHIP_FEATURE_EMB_PSRAM) ? "[embedded]"
                                                    : "[external]");

  s_chip_info += fmt::format(
      "Connectivity : {}{}{}\n",
      (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "2.4GHz WIFI" : "NA",
      (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
      (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
  // s_chip_info += fmt::format("\nIEEE 802.15.4 :
  // {}",string((chip_info.features & CHIP_FEATURE_IEEE802154) ? "YES" : "NA"));

  // ESP_LOGE(TAG,"\n%s",device_info().c_str());
  return s_chip_info;
}
