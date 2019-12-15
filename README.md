# MonoGFX
Lightweight graphics library for monochromatic GLCDs.

Methods for drawing to a buffer:
 - circle
 - line
 - rect
 - round rect
 - triangle

Every method supports outline or fill. Every method can enable, disable or invert the pixels of the shape.

## Installation

### Arduino
Clone this repo to your arduino library folder.

### Other
Clone this repo, add `src/` to include directories and source directories.

## Usage

 - First create an instance of `MonoGFX_BufferTypedef` structure.

        MonoGFX_BufferTypedef MonoGFXDisplay;
        MonoGFXDisplay.width = 128;
        MonoGFXDisplay.height = 64;
        MonoGFXDisplay.mode = MonoGFX_DISPLAY_MODE_VERTICAL; // orientation of pixels in one byte
        MonoGFX_init(&MonoGFXDisplay);

    This zero allocates a buffer of size `w * h / 8` for use as a drawing surface. Each byte represents 8 pixels. Use `void MonoGFX_deinit(MonoGFX_BufferTypedef* buff)` to free this buffer.


 - Then use the following methods to draw on the buffer:

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

    See `MonoGFX.h`
 - You can change the active buffer by calling `void MonoGFX_setActiveBuffer(MonoGFX_BufferTypedef* buff)` in case you have multiple displays.
 - Finally send the contents of the buffer to your GLCD module. This should look something like this:

       void GLCD_update() {
        // MonoGFXDisplay is previously created `MonoGFX_BufferTypedef` structure.
        // To retrieve a pointer to this structure, you can call MonoGFX_BufferTypedef* MonoGFX_getActiveBuffer()
        // That being said, you are always the owner of this structure.
        for (uint32_t i = 0; i < MonoGFXDisplay.bufferSize; i ++) {
         GLCD_Write(i, MonoGFXDisplay.buffer[i]); // write the byte to GLCD memory
        }
       }



## TODO
 - Support for bufferless operation for low memory controllers (with and without GLCD memory read operation)
 - Support for double buffered operation for low bandwidth displays
 - Remove use of `malloc` for buffer allocation
 - Documentation
 - Examples
 - Tests
 - Implement text
 - Implement bitmaps
 - Implement animations
 - Implement clip rect
 - Implement better line style algorithm (thickness, custom pattern)
 - Implement bezier curves
