/*

Driver code for KS0107/S6B0107/SBN6400 compatible LCD drivers that are connected to an Arduino via serial interface.

*/

#include "Drivers/Arduino/KS0107Serial.h"
#include "Arduino.h"
#include <stdint.h>
#include <SPI.h>


#define KS0107_DD_DI_COMMAND	0
#define KS0107_DD_DI_DATA		1

#define KS0107_DD_CS_1		1
#define KS0107_DD_CS_2		2
#define KS0107_DD_CS_12		(KS0107_DD_CS_1 | KS0107_DD_CS_2)

#define KS0107_DD_REG_DISPLAY_STATE		0x3E
#define KS0107_DD_REG_START_LINE		0xC0
#define KS0107_DD_REG_COLUMN_ADDR		0x40
#define KS0107_DD_REG_PAGE_ADDR			0xB8

#define KS0107_DD_DISPLAY_STATE_ON		0x01
#define KS0107_DD_DISPLAY_STATE_OFF		0x01

#define KS0107_DD_START_LINE_MASK		0x3F
#define KS0107_DD_COLUMN_ADDR_MASK		0x3F
#define KS0107_DD_PAGE_ADDR_MASK		0x07




KS0107Serial::KS0107Serial(uint8_t slaveSelectPin) {
	this->_slaveSelectPin = slaveSelectPin;
	
	pinMode(this->_slaveSelectPin, OUTPUT);
	SPI.begin();
	
	this->shiftOut(0, 0); // GLCD_E = 0
	this->write(KS0107_DD_CS_12, KS0107_DD_DI_COMMAND, KS0107_DD_REG_DISPLAY_STATE | KS0107_DD_DISPLAY_STATE_ON);	// GLCD on
	this->write(KS0107_DD_CS_12, KS0107_DD_DI_COMMAND, KS0107_DD_REG_START_LINE | (0 & KS0107_DD_START_LINE_MASK));	// GLCD Start Line
}

void KS0107Serial::update(uint8_t* buff) {
	uint8_t x, y;
	uint32_t pos = 0;
	
	for (y = 0; y < 8; y ++) {
		this->write(KS0107_DD_CS_1, KS0107_DD_DI_COMMAND, KS0107_DD_REG_COLUMN_ADDR); // set column to 0
		this->write(KS0107_DD_CS_1, KS0107_DD_DI_COMMAND, (KS0107_DD_REG_PAGE_ADDR | y)); // send row number
		for (x = 0; x < 64; x ++) {
			this->write(KS0107_DD_CS_1, KS0107_DD_DI_DATA, buff[pos]);
			pos++;
		}
		this->write(KS0107_DD_CS_2, KS0107_DD_DI_COMMAND, KS0107_DD_REG_COLUMN_ADDR);
		this->write(KS0107_DD_CS_2, KS0107_DD_DI_COMMAND, (KS0107_DD_REG_PAGE_ADDR | y));
		for (x = 0; x < 64; x ++) {
			this->write(KS0107_DD_CS_2, KS0107_DD_DI_DATA, buff[pos]);
			pos++;
		}
	}
}

void KS0107Serial::shiftOut(int8_t data, int8_t control) {
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(this->_slaveSelectPin, LOW);
	SPI.transfer(control);
	SPI.transfer(data);
	digitalWrite(this->_slaveSelectPin, HIGH);
	
	SPI.endTransaction();
}
void KS0107Serial::write(uint8_t cs_s, uint8_t d_i, uint8_t g_data) {
	uint8_t E	= 0b00010000;
	uint8_t DI	= 0b00001000;
	uint8_t CS1	= 0b00000010;
	uint8_t CS2	= 0b00000100;
	
// 0..7 = DB0-DB7
// 8 = NC
// 9, 10 = /CS1, /CS2
// 11 = D/I
// 12 = E
// 13..15 = NC
	uint8_t control = 0x00;
	switch (cs_s) {
		case KS0107_DD_CS_1:
			control |= CS1;
			break;
		case KS0107_DD_CS_2:
			control |= CS2;
			break;
		case KS0107_DD_CS_12:
			control |= CS1;
			control |= CS2;
			break;
	}
	switch (d_i) {
		case KS0107_DD_DI_COMMAND:
			control &= ~DI;
			break;
		case KS0107_DD_DI_DATA:
			control |= DI;
			break;
	}
	
	this->shiftOut(g_data, control);
	
	// delayMicroseconds(1);
	control |= E;
	this->shiftOut(g_data, control);
	
	// delayMicroseconds(2);
	control &= (~E);
	this->shiftOut(g_data, control);
	
	// delayMicroseconds(4);
	control &= (~CS1);
	control &= (~CS2);
	this->shiftOut(g_data, control);
}

