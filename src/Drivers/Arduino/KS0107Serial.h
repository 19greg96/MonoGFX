#ifndef KS0107Serial_h
#define KS0107Serial_h

#include <stdint.h>

class KS0107Serial {
	public:
		KS0107Serial(uint8_t slaveSelectPin);
		void shiftOut(int8_t data, int8_t control);
		void write(uint8_t cs_s, uint8_t d_i, uint8_t g_data);
		void update(uint8_t* buff);
	private:
		uint8_t _slaveSelectPin;
};



#endif