#ifndef ArduinoGraphics2DCanvas_H
#define ArduinoGraphics2DCanvas_H

#ifdef OSW_DISPLAY_GC9A01
#include <Arduino_GFX.h>
#endif
#ifdef OSW_DISPLAY_LS012B7DH02
#include <Adafruit_SharpMem.h>
#endif

#include <gfx_2d_print.h>

class Arduino_Canvas_Graphics2D : public Arduino_GFX {
 public:
#ifdef OSW_DISPLAY_GC9A01
  Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);
#endif
#ifdef OSW_DISPLAY_LS012B7DH02
  Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Adafruit_SharpMem *output, int16_t output_x = 0,
                            int16_t output_y = 0);
#endif
  void begin(int32_t speed = 0);
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color);
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void flush(void);

  Graphics2DPrint *getGraphics2D(void);

 protected:
  Graphics2DPrint *_gfx2d;
#ifdef OSW_DISPLAY_GC9A01
  Arduino_G *_output;
#endif
#ifdef OSW_DISPLAY_LS012B7DH02
  Adafruit_SharpMem *_output;
#endif

  int16_t _output_x, _output_y;

 private:
};

#endif
