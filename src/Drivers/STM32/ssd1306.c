#include "ssd1306.h"
#include <stdlib.h>

#if defined(SSD1306_USE_I2C)

void ssd1306_Reset(SSD1306_t* dev) {
	/* for I2C - do nothing */
}

/* when not customized by application define dummy one */
#ifndef ssd1306_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define ssd1306_GetI2cBus(dev) { xSemaphoreTake(dev->i2c->mutex, portMAX_DELAY); }
#endif

#ifndef ssd1306_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define ssd1306_PutI2cBus(dev) { xSemaphoreGive(dev->i2c->mutex); }
#endif

// Send a byte to the command register
void ssd1306_WriteCommand(SSD1306_t* dev, uint8_t byte) {
#ifdef SSD1306_FreeRTOS
	int status;
	status = HAL_I2C_Mem_Write_IT(dev->i2c->handle, dev->addr, 0x00, 1, &byte, 1);
	if (status == 0) {
    	xSemaphoreTake(dev->i2c->blockSem, portMAX_DELAY);
    	
    	status = dev->i2c->status;
    } else {
		// transmit error: reset i2c bus?
	}
#else
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
#endif
}

// Send data
void ssd1306_WriteData(SSD1306_t* dev, uint8_t* buffer, size_t buff_size) {
#ifdef SSD1306_FreeRTOS
	int status;
	status = HAL_I2C_Mem_Write_IT(dev->i2c->handle, dev->addr, 0x40, 1, buffer, buff_size);
	if (status == 0) {
    	xSemaphoreTake(dev->i2c->blockSem, portMAX_DELAY);
    	
    	status = dev->i2c->status;
    } else {
		// transmit error: reset i2c bus?
	}
#else
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
#endif
}

#elif defined(SSD1306_USE_SPI)

void ssd1306_Reset(SSD1306_t* dev) {
	// CS = High (not selected)
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);

	// Reset the OLED
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
	SSD1306_Delay(10);
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
	SSD1306_Delay(10);
}

// Send a byte to the command register
void ssd1306_WriteCommand(SSD1306_t* dev, uint8_t byte) {
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
	HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET); // command
	HAL_SPI_Transmit(&SSD1306_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

// Send data
void ssd1306_WriteData(SSD1306_t* dev, uint8_t* buffer, size_t buff_size) {
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
	HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET); // data
	HAL_SPI_Transmit(&SSD1306_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif


static void setDisplayOn(SSD1306_t* dev, const uint8_t on) {
	uint8_t value;
	if (on) {
		value = 0xAF;   // Display on
	} else {
		value = 0xAE;   // Display off
	}
	ssd1306_WriteCommand(dev, value);
}

static void setContrast(SSD1306_t* dev, const uint8_t value) {
	const uint8_t kSetContrastControlRegister = 0x81;
	ssd1306_WriteCommand(dev, kSetContrastControlRegister);
	ssd1306_WriteCommand(dev, value);
}

// Initialize the oled screen
void ssd1306_initialize(SSD1306_t* dev) {
	// Reset OLED
	ssd1306_Reset(dev);

	// Wait for the screen to boot
	SSD1306_Delay(100);

	ssd1306_GetI2cBus(dev);
	
	// Init OLED
	setDisplayOn(dev, 0); //display off

	ssd1306_WriteCommand(dev, 0x20); //Set Memory Addressing Mode
	ssd1306_WriteCommand(dev, 0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
								// 10b,Page Addressing Mode (RESET); 11b,Invalid

	ssd1306_WriteCommand(dev, 0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
	ssd1306_WriteCommand(dev, 0xC0); // Mirror vertically
#else
	ssd1306_WriteCommand(dev, 0xC8); //Set COM Output Scan Direction
#endif

	ssd1306_WriteCommand(dev, 0x00); //---set low column address
	ssd1306_WriteCommand(dev, 0x10); //---set high column address

	ssd1306_WriteCommand(dev, 0x40); //--set start line address - CHECK

	setContrast(dev, 0xFF);

#ifdef SSD1306_MIRROR_HORIZ
	ssd1306_WriteCommand(dev, 0xA0); // Mirror horizontally
#else
	ssd1306_WriteCommand(dev, 0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
	ssd1306_WriteCommand(dev, 0xA7); //--set inverse color
#else
	ssd1306_WriteCommand(dev, 0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
	// Found in the Luma Python lib for SH1106.
	ssd1306_WriteCommand(dev, 0xFF);
#else
	ssd1306_WriteCommand(dev, 0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
	ssd1306_WriteCommand(dev, 0x1F); //
#elif (SSD1306_HEIGHT == 64)
	ssd1306_WriteCommand(dev, 0x3F); //
#elif (SSD1306_HEIGHT == 128)
	ssd1306_WriteCommand(dev, 0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

	ssd1306_WriteCommand(dev, 0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

	ssd1306_WriteCommand(dev, 0xD3); //-set display offset - CHECK
	ssd1306_WriteCommand(dev, 0x00); //-not offset

	ssd1306_WriteCommand(dev, 0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_WriteCommand(dev, 0xF0); //--set divide ratio

	ssd1306_WriteCommand(dev, 0xD9); //--set pre-charge period
	ssd1306_WriteCommand(dev, 0x22); //

	ssd1306_WriteCommand(dev, 0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
	ssd1306_WriteCommand(dev, 0x02);
#elif (SSD1306_HEIGHT == 64)
	ssd1306_WriteCommand(dev, 0x12);
#elif (SSD1306_HEIGHT == 128)
	ssd1306_WriteCommand(dev, 0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

	ssd1306_WriteCommand(dev, 0xDB); //--set vcomh
	ssd1306_WriteCommand(dev, 0x20); //0x20,0.77xVcc

	ssd1306_WriteCommand(dev, 0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(dev, 0x14); //
	setDisplayOn(dev, 1); //--turn on SSD1306 panel
	
	ssd1306_PutI2cBus(dev);
}


// Write the screenbuffer with changed to the screen
void ssd1306_update(SSD1306_t* dev, uint8_t* buff) {
	// Write data to each page of RAM. Number of pages
	// depends on the screen height:
	//
	//  * 32px   ==  4 pages
	//  * 64px   ==  8 pages
	//  * 128px  ==  16 pages
	ssd1306_GetI2cBus(dev);
	for (uint8_t i = 0; i < SSD1306_HEIGHT / 8; i++) {
		ssd1306_WriteCommand(dev, 0xB0 + i); // Set the current RAM page address.
		ssd1306_WriteCommand(dev, 0x00);
		ssd1306_WriteCommand(dev, 0x10);
		ssd1306_WriteData(dev, &buff[SSD1306_WIDTH * i], SSD1306_WIDTH);
	}
	ssd1306_PutI2cBus(dev);
}

void ssd1306_SetContrast(SSD1306_t* dev, const uint8_t value) {
	ssd1306_GetI2cBus(dev);
	
	setContrast(dev, value);
	
	ssd1306_PutI2cBus(dev);
}


