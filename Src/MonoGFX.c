
#include "MonoGFX.h"
#include <math.h>
#include <stdlib.h>


MonoGFX_BufferTypedef* activeBuffer = NULL;

void MonoGFX_flood_fill_util(int32_t x, int32_t y, uint8_t prevC, uint8_t newC);
void MonoGFX_draw_circle_util(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint8_t color);
void MonoGFX_fill_circle_util(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t centerLength, uint8_t color);


void MonoGFX_init(MonoGFX_BufferTypedef* buff) {
	MonoGFX_setActiveBuffer(buff);
	
	activeBuffer->bufferSize = activeBuffer->width * activeBuffer->height / 8;
	activeBuffer->buffer = (uint8_t*)calloc(sizeof(uint8_t) * activeBuffer->bufferSize, sizeof(uint8_t)); // data size is 8
}
void MonoGFX_deinit(MonoGFX_BufferTypedef* buff) {
	free(buff->buffer);
}

void MonoGFX_setActiveBuffer(MonoGFX_BufferTypedef* buff) {
	activeBuffer = buff;
}
MonoGFX_BufferTypedef* MonoGFX_getActiveBuffer() {
	return activeBuffer;
}

void MonoGFX_clear() {
	for (uint32_t i = 0; i < activeBuffer->bufferSize; i ++) {
		activeBuffer->buffer[i] = 0;
	}
}

uint8_t MonoGFX_set_pixel(int32_t x, int32_t y, uint8_t v) {
	if (activeBuffer == NULL) {
		return -1;
	}
	if (x < 0 || x >= activeBuffer->width || y < 0 || y >= activeBuffer->height) {
		return -1;
	}
	// TODO: display width and height are hardcoded here:
	// MonoGFX_DISPLAY_MODE_HORIZONTAL: 240*128
	// MonoGFX_DISPLAY_MODE_VERTICAL: 128*64
	if (v == MonoGFX_COLOR_ON) {
		if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_HORIZONTAL) {
			activeBuffer->buffer[30 * y + (x >> 3)] |= 0x80 >> (x & 7); // bits arranged in rows
		} else if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_VERTICAL) {
			activeBuffer->buffer[x + ((y >> 3) << 7)] |= 0x01 << (y & 7); // bits arranged in columns
		}
	} else if (v == MonoGFX_COLOR_INVERT) {
		if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_HORIZONTAL) {
			activeBuffer->buffer[30 * y + (x >> 3)] ^= 0x80 >> (x & 7);
		} else if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_VERTICAL) {
			activeBuffer->buffer[x + ((y >> 3) << 7)] ^= 0x01 << (y & 7);
		}
	} else if (v == MonoGFX_COLOR_OFF) {
		if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_HORIZONTAL) {
			activeBuffer->buffer[30 * y + (x >> 3)] &= ~(0x80 >> (x & 7));
		} else if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_VERTICAL) {
			activeBuffer->buffer[x + ((y >> 3) << 7)] &= ~(0x01 << (y & 7));
		}
	}
	return v;
}
uint8_t MonoGFX_get_pixel(int32_t x, int32_t y) {
	if (activeBuffer == NULL) {
		return -1;
	}
	if (x < 0 || x >= activeBuffer->width || y < 0 || y >= activeBuffer->height) {
		return -1;
	}
	if (activeBuffer->mode == MonoGFX_DISPLAY_MODE_HORIZONTAL) {
		return (activeBuffer->buffer[30 * y + (x >> 3)] >> ((~x) & 7)) & 1;
	} else {
		return (activeBuffer->buffer[x + ((y >> 3) << 7)] >> (y & 7)) & 1;
	}
}

void MonoGFX_flood_fill_util(int32_t x, int32_t y, uint8_t prevC, uint8_t newC) {
	if (MonoGFX_get_pixel(x, y) != prevC) {
		return;
	}
	
	MonoGFX_set_pixel(x, y, newC);
	
	MonoGFX_flood_fill_util(x + 1, y, prevC, newC);
	MonoGFX_flood_fill_util(x - 1, y, prevC, newC);
	MonoGFX_flood_fill_util(x,	y + 1, prevC, newC);
	MonoGFX_flood_fill_util(x,	y - 1, prevC, newC);
}
void MonoGFX_flood_fill(int32_t x, int32_t y, uint8_t newC) {
	MonoGFX_flood_fill_util(x, y, !newC, newC);
}


// Draw a circle outline
void MonoGFX_draw_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color) {
	MonoGFX_set_pixel(x0, y0+r, color);
	MonoGFX_set_pixel(x0, y0-r, color);
	MonoGFX_set_pixel(x0+r, y0, color);
	MonoGFX_set_pixel(x0-r, y0, color);
	
	MonoGFX_draw_circle_util(x0, y0, r, 15, color);
}

void MonoGFX_draw_circle_util(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint8_t color) {
	int32_t f		 = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -2 * r;
	int32_t x		 = 0;
	int32_t y		 = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			MonoGFX_set_pixel(x0 + x, y0 + y, color);
			MonoGFX_set_pixel(x0 + y, y0 + x, color);
		} 
		if (cornername & 0x2) {
			MonoGFX_set_pixel(x0 + x, y0 - y, color);
			MonoGFX_set_pixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			MonoGFX_set_pixel(x0 - y, y0 + x, color);
			MonoGFX_set_pixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			MonoGFX_set_pixel(x0 - y, y0 - x, color);
			MonoGFX_set_pixel(x0 - x, y0 - y, color);
		}
	}
}

void MonoGFX_fill_circle(int32_t x0, int32_t y0, int32_t r, uint8_t color) {
	MonoGFX_draw_line(x0, y0-r, x0, 2*r, color);
	MonoGFX_fill_circle_util(x0, y0, r, 3, 0, color);
}

// Used to draw circles and roundrects
void MonoGFX_fill_circle_util(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t centerLength, uint8_t color) {
	int32_t f		= 1 - r;
	int32_t ddF_x	= 1;
	int32_t ddF_y	= -2 * r;
	int32_t x		= 0;
	int32_t y		= r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y	+= 2;
			f		+= ddF_y;
		}
		x++;
		ddF_x	+= 2;
		f		+= ddF_x;

		if (cornername & 0x1) { // right half
			MonoGFX_draw_line(x0+x, y0-y, x0+x, y0 + y+centerLength, color);
			MonoGFX_draw_line(x0+y, y0-x, x0+y, y0 + x+centerLength, color);
		}
		if (cornername & 0x2) { // left half
			MonoGFX_draw_line(x0-x, y0-y, x0-x, y0 + y+centerLength, color);
			MonoGFX_draw_line(x0-y, y0-x, x0-y, y0 + x+centerLength, color);
		}
	}
}

void MonoGFX_draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color) {
	MonoGFX_draw_line_style(x0, y0, x1, y1, MonoGFX_LINE_STYLE_SOLID, color);
}
// Bresenham's algorithm - thx wikpedia
void MonoGFX_draw_line_style(int32_t x0, int32_t y0, int32_t x1, int32_t y1, MonoGFX_LineStyle style, uint8_t color) {
	if (style == MonoGFX_LINE_STYLE_NONE) {
		return;
	}
	int32_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		// std::swap(x0, y0);
		int32_t tmp = x0;
		x0 = y0;
		y0 = tmp;
		
		// std::swap(x1, y1);
		tmp = x1;
		x1 = y1;
		y1 = tmp;
	}

	if (x0 > x1) {
		// std::swap(x0, x1);
		int32_t tmp = x0;
		x0 = x1;
		x1 = tmp;
		
		// std::swap(y0, y1);
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	int32_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int32_t err = dx / 2;
	int32_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	uint8_t enable = 1;
	for (; x0 <= x1; x0 ++) {
		if (style == MonoGFX_LINE_STYLE_DOTS) {
			enable = (x0 & 1);
		} else if (style == MonoGFX_LINE_STYLE_DASHES) {
			enable = (x0 & 2);
		} else if (style == MonoGFX_LINE_STYLE_SPARSE_DOTS) {
			enable = ((x0 & 3) == 3);
		}
		if (enable) {
			if (steep) {
				MonoGFX_set_pixel(y0, x0, color);
			} else {
				MonoGFX_set_pixel(x0, y0, color);
			}
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// Draw a rectangle
void MonoGFX_draw_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color) {
	w--;
	h--;
	MonoGFX_draw_line(x, y, x + w, y, color); // top
	MonoGFX_draw_line(x, y + h, x + w, y + h, color); // bottom
	MonoGFX_draw_line(x, y, x, y + h, color); // left
	MonoGFX_draw_line(x + w, y, x + w, y + h, color); // right
}

void MonoGFX_fill_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t color) {
	h --;
	if (w == 0 || h == 0) {
		return;
	}
	for (int32_t i = x; i < x + w; i ++) {
		MonoGFX_draw_line(i, y, i, y + h, color);
	}
}

// Draw a rounded rectangle
void MonoGFX_draw_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color) {
	// smarter version
	MonoGFX_draw_line(x + r,		y,			x + w - 2 * r,	y,				color); // top
	MonoGFX_draw_line(x + r,		y + h - 1,	x + w - 2 * r,	y + h - 1,		color); // bottom
	MonoGFX_draw_line(x,			y + r,		x,				y + h - 2 * r,	color); // left
	MonoGFX_draw_line(x + w - 1,	y + r,		x + w - 1,		y + h - 2 * r,	color); // right
	// draw four corners
	MonoGFX_draw_circle_util(x+r,		y+r,		r, 1, color);
	MonoGFX_draw_circle_util(x+w-r-1,	y+r,		r, 2, color);
	MonoGFX_draw_circle_util(x+w-r-1,	y+h-r-1,	r, 4, color);
	MonoGFX_draw_circle_util(x+r,		y+h-r-1,	r, 8, color);
}

// Fill a rounded rectangle
void MonoGFX_fill_round_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t r, uint8_t color) {
	// smarter version
	MonoGFX_fill_rect(x + r, y, w - 2 * r, h, color); // draw center horizontal fill of rectangle (from x+r to end-r)

	// draw two sides
	MonoGFX_fill_circle_util(x + w - r - 1,	y + r, r, 1, h - 2 * r - 1, color); // right side
	MonoGFX_fill_circle_util(x + r,			y + r, r, 2, h - 2 * r - 1, color); // left side
}

// Draw a triangle
void MonoGFX_draw_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color) {
	MonoGFX_draw_line(x0, y0, x1, y1, color);
	MonoGFX_draw_line(x1, y1, x2, y2, color);
	MonoGFX_draw_line(x2, y2, x0, y0, color);
}

// Fill a triangle
void MonoGFX_fill_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color) {
	int32_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	int32_t tmp;
	if (y0 > y1) {
		// std::swap(y0, y1);
		tmp = y0;
		y0 = y1;
		y1 = tmp;
		
		// std::swap(x0, x1);
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y1 > y2) {
		// std::swap(y2, y1);
		tmp = y2;
		y2 = y1;
		y1 = tmp;
		
		// std::swap(x2, x1);
		tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		// std::swap(y0, y1);
		tmp = y0;
		y0 = y1;
		y1 = tmp;
		
		// std::swap(x0, x1);
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)			a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)			a = x2;
		else if(x2 > b) b = x2;
		// drawFastHLine(a, y0, b-a+1, color);
		MonoGFX_draw_line(a, y0, b + 1, y0, color);
		return;
	}

	int32_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1;
	int32_t
		sa	 = 0,
		sb	 = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.	If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2) last = y1;	 // Include y1 scanline
	else				 last = y1-1; // Skip it

	for(y=y0; y<=last; y++) {
		a	 = x0 + sa / dy01;
		b	 = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) {
			// std::swap(a,b);
			tmp = a;
			a = b;
			b = tmp;
		}
		// drawFastHLine(a, y, b-a+1, color);
		MonoGFX_draw_line(a, y, b + 1, y, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.	This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a	 = x1 + sa / dy12;
		b	 = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) {
			// std::swap(a,b);
			tmp = a;
			a = b;
			b = tmp;
		}
		// drawFastHLine(a, y, b-a+1, color);
		MonoGFX_draw_line(a, y, b + 1, y, color);
	}
}

