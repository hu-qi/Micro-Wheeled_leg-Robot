/*
 * ESP32-S31 safe P0 board bring-up. No motor or servo commands.
 * External I2C connections must be verified before enabling the probe.
 */
#include <stdio.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_chip_info.h"
#include "esp_idf_version.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

static const char *TAG = "s31_p0";
#ifndef CONFIG_WL_I2C_PROBE_ENABLE
#define CONFIG_WL_I2C_PROBE_ENABLE 0
#endif

static void probe_i2c(void)
{
#if CONFIG_WL_I2C_PROBE_ENABLE
    const i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = CONFIG_WL_I2C_SDA_GPIO,
        .scl_io_num = CONFIG_WL_I2C_SCL_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus = NULL;
    esp_err_t err = i2c_new_master_bus(&bus_cfg, &bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C initialization failed: %s", esp_err_to_name(err));
        return;
    }
    for (int addr = 0x08; addr <= 0x77; ++addr) {
        if (i2c_master_probe(bus, addr, 30) == ESP_OK) {
            ESP_LOGI(TAG, "I2C device at 0x%02x", addr);
        }
    }
    ESP_ERROR_CHECK(i2c_del_master_bus(bus));
#else
    ESP_LOGI(TAG, "I2C scan disabled; enable after verifying 3.3 V wiring and GPIO allocation");
#endif
}

void app_main(void)
{
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(TAG, "P0 bring-up / SAFE MODE: motor PWM and servo output not configured");
    ESP_LOGI(TAG, "ESP-IDF %s, %d CPU cores, revision %d", esp_get_idf_version(), chip.cores, chip.revision);
    uint32_t flash_size = 0;
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        ESP_LOGI(TAG, "Flash size: %lu bytes", (unsigned long)flash_size);
    }
    probe_i2c();
    while (1) {
        ESP_LOGI(TAG, "heartbeat: safe mode, no actuator output");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
