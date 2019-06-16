#ifndef __MonoGFX_H
#define __MonoGFX_H


#include <stdint.h>



#define MonoGFX_COLOR_OFF		0
#define MonoGFX_COLOR_ON		1
#define MonoGFX_COLOR_INVERT	2


typedef enum {
	MonoGFX_DISPLAY_MODE_HORIZONTAL,
	MonoGFX_DISPLAY_MODE_VERTICAL
} MonoGFX_BufferModeTypedef;

typedef struct {
	uint32_t width;
	uint32_t height;
	
	uint32_t bufferSize;
	uint8_t* buffer;
	
	MonoGFX_BufferModeTypedef mode;
} MonoGFX_BufferTypedef;

typedef enum {
	MonoGFX_LINE_STYLE_NONE,
	MonoGFX_LINE_STYLE_SOLID,
	MonoGFX_LINE_STYLE_DOTS,
	MonoGFX_LINE_STYLE_DASHES,
	MonoGFX_LINE_STYLE_SPARSE_DOTS
} MonoGFX_LineStyleTypedef;


void MonoGFX_init(MonoGFX_BufferTypedef* buff);
void MonoGFX_deinit(MonoGFX_BufferTypedef* buff);

void MonoGFX_setActiveBuffer(MonoGFX_BufferTypedef* buff);
MonoGFX_BufferTypedef* MonoGFX_getActiveBuffer();

void MonoGFX_clear();

uint8_t MonoGFX_set_pixel(int32_t x, int32_t y, uint8_t v);
uint8_t MonoGFX_get_pixel(int32_t x, int32_t y);

void MonoGFX_flood_fill(int32_t x, int32_t y, uint8_t newC);

void MonoGFX_draw_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color);
void MonoGFX_fill_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color);

void MonoGFX_draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);
void MonoGFX_draw_line_style(int32_t x0, int32_t y0, int32_t x1, int32_t y1, MonoGFX_LineStyle style, uint8_t color);

void MonoGFX_draw_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color);
void MonoGFX_fill_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color);

void MonoGFX_draw_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color);
void MonoGFX_fill_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color);

void MonoGFX_draw_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color);
void MonoGFX_fill_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color);




#endif