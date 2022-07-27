#pragma once

#include <stdbool.h>

#include "sdkconfig.h"

#include "esp_err.h"

#if defined CONFIG_M5DEVICE_STICKC_PLUS
#define M5DEVICE_PORTA_SDA      21
#define M5DEVICE_PORTA_SCL      22
#elif defined CONFIG_M5DEVICE_STACK_CORE2
//...
#endif // M5DEVICE_TYPE

extern bool m5device_is_initialized;

esp_err_t m5device_init(void);
esp_err_t m5device_deinit(void);

esp_err_t m5device_i2c_init(void);
esp_err_t m5device_i2c_deinit(void);

bool m5device_i2c_is_initialized(void);
