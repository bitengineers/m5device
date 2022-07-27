#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "m5device.h"

#define TAG "m5device"

bool m5device_is_initialized = false;
static bool _m5device_i2c_is_initialized = false;
SemaphoreHandle_t sem = NULL;

/**
 * M5device initialization
 */
esp_err_t m5device_init(void)
{
  if (! m5device_is_initialized) {
    if (sem == NULL) {
      vSemaphoreCreateBinary(sem);
    }

#ifdef CONFIG_M5STICKC_PLUS_I2C
    ESP_ERROR_CHECK(m5device_i2c_init());
#endif // CONFIG_M5STICKC_PLUS_I2C

    if (semTake(sem, (TickType_t) 10) == pdTRUE) {
      m5device_is_initialized = true;
      semGive(sem);
    } else {
      ESP_LOGI(TAG, "In Initialization, semaphoreTake failed.");
      return ESP_ERR_TIMEOUT;
    }
  }

  return ESP_OK;
}


esp_err_t m5device_deinit(void)
{
  if (m5device_is_initialized) {
    if (semTake(sem, (TickType_t) 10) == pdTRUE) {
      m5device_is_initialized = false;
      semGive(sem);
    } else {
      ESP_LOGI(TAG, "In Deinitialization, semaphoreTake failed.");
      return ESP_ERR_TIMEOUT;
    }
  }

  return ESP_OK;
}


esp_err_t m5device_i2c_init(void)
{
  int i2c_port = I2C_NUM_0;
  gpio_pullup_t pullup = GPIO_PULLUP_DISABLE;
  i2c_mode_t mode = I2C_MODE_MASTER;
  uint32_t clk = 0;

#ifdef CONFIG_M5STICKC_PLUS_I2C_NUM_1
  i2c_port = I2C_NUM_1;
#endif // CONFIG_M5STICKC_PLUS_I2C_NUM_1

#ifdef CONFIG_M5STICKC_PLUS_PULLUP_ENABLE
  pullup = GPIO_PULLUP_ENABLE;
#endif // CONFIG_M5STICKC_PLUS_PULLUP_ENABLE

#ifdef CONFIG_M5STICKC_PLUS_I2C_SLAVE
  mode = I2C_MODE_SLAVE;
#endif // CONFIG_M5STICKC_PLUS_I2C_SLAVE

#ifdef CONFIG_M5STICKC_PLUS_I2C_CLK
  clk = (int)(CONFIG_M5STICKC_PLUS_I2C_CLK);
#endif // CONFIG_M5STICKC_PLUS_I2C_CLK

  i2c_config_t i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = M5DEVICE_PORTA_SDA,
    .scl_io_num = M5DEVICE_PORTA_SCL,
    .sda_pullup_en = pullup,
    .scl_pullup_en = pullup,
    .master.clk_speed = clk
  };

  if (semTake(sem, (TickType_t) 10) == pdTRUE) {
    ESP_ERROR_CHECK(i2c_param_config(i2c_port, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_port, mode, 0, 0, 0));
    _m5device_i2c_is_initialized = true;
    semGive(sem);
  } else {
    ESP_LOGI(TAG, "In stickcplus_i2c_init, semaphoreTake failed.");
    return ESP_ERR_TIMEOUT;
  }

  return ESP_OK;
}


esp_err_t m5device_i2c_deinit(void)
{
  int i2c_port = I2C_NUM_0;
#ifdef CONFIG_M5STICKC_PLUS_I2C_NUM_1
  i2c_port = I2C_NUM_1;
#endif // CONFIG_M5STICKC_PLUS_I2C_NUM_1

  return i2c_driver_delete(i2c_port);
}


bool m5device_i2c_is_initialized(void)
{
  return _m5device_i2c_is_initialized;
}

