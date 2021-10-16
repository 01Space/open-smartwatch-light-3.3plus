#include "Arduino_Canvas_Graphics2D.h"

#ifdef OSW_DISPLAY_GC9A01
#include <Arduino_DataBus.h>
#include <Arduino_GFX.h>
#endif

#ifdef OSW_DISPLAY_LS012B7DH02
#include <Adafruit_SharpMem.h>
#endif

#include <gfx_2d_print.h>

#include "config_defaults.h"

#ifdef OSW_DISPLAY_GC9A01
Arduino_Canvas_Graphics2D::Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G* output, int16_t output_x,
                                                     int16_t output_y)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y) {}
#endif

#ifdef OSW_DISPLAY_LS012B7DH02
Arduino_Canvas_Graphics2D::Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Adafruit_SharpMem* output, int16_t output_x,
                                                     int16_t output_y)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y) {}
#endif

void Arduino_Canvas_Graphics2D::begin(int32_t speed) {
  _gfx2d = new Graphics2DPrint(_width, _height, DISP_CHUNK_H, true);
  _output->begin();
  //   _output->fillScreen(BLACK);
}
void Arduino_Canvas_Graphics2D::writePixelPreclipped(int16_t x, int16_t y, uint16_t color) {
  _gfx2d->drawPixel(x, y, color);
}
void Arduino_Canvas_Graphics2D::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  _gfx2d->drawVLine(x, y, h, color);
}
void Arduino_Canvas_Graphics2D::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  _gfx2d->drawHLine(x, y, w, color);
}
void Arduino_Canvas_Graphics2D::flush(void) {
  // only flush if there is a buffer
  if (_gfx2d->hasBuffer()) {
    uint8_t chunkHeight = _gfx2d->getChunkHeight();
    for (uint8_t chunk = 0; chunk < _gfx2d->numChunks(); chunk++) {
      uint16_t offsetX = _gfx2d->getChunkOffset(chunk);
      uint16_t offsetY = chunk * chunkHeight;

      for (uint16_t y = 0; y < chunkHeight; y++) {
        for (uint16_t x = 0; x < _gfx2d->getChunkWidth(chunk); x++) {
          uint16_t screenX = offsetX + x;
          uint16_t screenY = offsetY + y;
          _output->drawPixel(screenX, screenY, _gfx2d->getPixel(screenX, screenY));
        }
      }
    }
    _output->refresh();
  }
}

Graphics2DPrint* Arduino_Canvas_Graphics2D::getGraphics2D(void) { return _gfx2d; }
