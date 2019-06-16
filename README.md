# MonoGFX
Graphics library for monochromatic GLCDs.

Methods for drawing to a buffer:
 - circle
 - line
 - rect
 - round rect
 - triangle

Every method supports outline or fill. Every method can enable, disable or invert the pixels of the shape.


## TODO
 - Support for multiple display buffers
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
