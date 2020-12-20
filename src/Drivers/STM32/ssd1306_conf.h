/**
 * Private configuration file for the SSD1306 library.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

#include "utils.h"

// Choose a microcontroller family
//#define STM32F0
//#define STM32F1
#define STM32F4
//#define STM32L0
//#define STM32L4
//#define STM32F3
//#define STM32H7
//#define STM32F7

// FreeRTOS support. To disable, simply comment out the following block:
//{
#define SSD1306_FreeRTOS
#define SSD1306_Delay(ms)           vTaskDelay(ms / portTICK_PERIOD_MS)

//}
// -- END FreeRTOS support block.

// Choose a bus
#define SSD1306_USE_I2C
//#define SSD1306_USE_SPI

// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1306_INVERSE_COLOR

#endif /* __SSD1306_CONF_H__ */
