idf_component_register(SRCS main.cpp
					Display.cpp
					Gui.cpp
					Theme.cpp
					Lcd.cpp
					Periodic.cpp
					GuiThread.cpp
					widgets/tux_panel.c
					# Status icons like BLE
					fonts/font_fa_14.c

					# Weather icons like clouds etc
					# "fonts/font_fa_weather_32.c"
					fonts/font_fa_weather_42.c
					# "fonts/font_fa_weather_48.c"
					# "fonts/font_fa_weather_56.c"
					# "fonts/font_fa_weather_64.c"

					# Mototype font used in device info
					# "fonts/font_robotomono_12.c"
					fonts/font_robotomono_13.c

					# 7Seg font used for time
					# "fonts/font_7seg_64.c"
					# "fonts/font_7seg_60.c"
					# "fonts/font_7seg_58.c"
					fonts/font_7seg_56.c

                INCLUDE_DIRS . devices ../loki-lib/include
				REQUIRES json LovyanGFX lvgl fatfs fmt Preferences spi_flash lvglpp
				app_update ota esp_event esp_timer spiffs esp_partition
				esp_hw_support
				)

spiffs_create_partition_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT)
#fatfs_create_spiflash_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT PRESERVE_TIME)