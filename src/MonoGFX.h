#ifndef __MonoGFX_H
#define __MonoGFX_H

#include "MonoGFX_config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MonoGFX_COLOR_OFF		0
#define MonoGFX_COLOR_ON		1
#define MonoGFX_COLOR_INVERT	2

// 2^3 bits in a byte
#define MonoGFX_BYTE_SIZE		3


typedef enum {
	MonoGFX_DISPLAY_MODE_HORIZONTAL,
	MonoGFX_DISPLAY_MODE_VERTICAL
} MonoGFX_DisplayModeTypedef;

typedef struct {
	uint32_t width;
	uint32_t height;
	
	uint32_t bufferSize;
	uint8_t* buffer;
	
	MonoGFX_DisplayModeTypedef mode;
} MonoGFX_DisplayTypedef;

typedef enum {
	MonoGFX_LINE_STYLE_NONE,
	MonoGFX_LINE_STYLE_SOLID,
	MonoGFX_LINE_STYLE_DOTS,
	MonoGFX_LINE_STYLE_DASHES,
	MonoGFX_LINE_STYLE_SPARSE_DOTS
} MonoGFX_LineStyleTypedef;

void MonoGFX_init(MonoGFX_DisplayTypedef* disp);
void MonoGFX_deinit(MonoGFX_DisplayTypedef* disp);

void MonoGFX_set_active_display(MonoGFX_DisplayTypedef* disp);
MonoGFX_DisplayTypedef* MonoGFX_get_active_display();

void MonoGFX_update();
void MonoGFX_clear();

uint8_t MonoGFX_set_pixel(int32_t x, int32_t y, uint8_t v);
uint8_t MonoGFX_get_pixel(int32_t x, int32_t y);

void MonoGFX_flood_fill(int32_t x, int32_t y, uint8_t newC);

void MonoGFX_draw_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color);
void MonoGFX_fill_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color);

void MonoGFX_draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);
void MonoGFX_draw_line_style(int32_t x0, int32_t y0, int32_t x1, int32_t y1, MonoGFX_LineStyleTypedef style, uint8_t color);

void MonoGFX_draw_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color);
void MonoGFX_fill_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color);

void MonoGFX_draw_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color);
void MonoGFX_fill_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color);

void MonoGFX_draw_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color);
void MonoGFX_fill_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color);

#ifdef __cplusplus
}
#endif

#endif
