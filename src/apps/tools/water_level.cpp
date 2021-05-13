
#include "./apps/tools/water_level.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

const int middleX = 120;
const int middleY = 120;
const int screenWidth = 240;

void OswAppWaterLevel::setup(OswHal* hal) {}

void OswAppWaterLevel::debug(OswHal* hal) {
  const int defaultXHint = 40;
  const int defaultYHint = 40;
  const int fontHeight = 1;
  const int lineHeight = fontHeight * 10 + 2;

  hal->gfx()->setTextSize(fontHeight);

  hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());
  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 0 * lineHeight);
  hal->gfx()->print("Accelerometer Data:");

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 1 * lineHeight);
  hal->gfx()->print("X: ");
  hal->gfx()->print(hal->getAccelerationX());

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 2 * lineHeight);
  hal->gfx()->print("Y: ");
  hal->gfx()->print(hal->getAccelerationY());

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 3 * lineHeight);
  hal->gfx()->print("Z: ");
  hal->gfx()->print(hal->getAccelerationZ());
}

void OswAppWaterLevel::circlesDisplay(OswHal* hal) {
  Graphics2D* gfx = hal->gfx();
  const float xValue = hal->getAccelerationX();
  const float yValue = hal->getAccelerationY();
  const float zValue = hal->getAccelerationZ();

  const bool isXYAccelerationInMiddle = abs(yValue) < 0.25 && abs(xValue) < 0.25;

  uint16_t color = isXYAccelerationInMiddle ? ui->getSuccessColor() : ui->getInfoColor();

  hal->getCanvas()->drawFastHLine(0, middleY, screenWidth, color);
  hal->getCanvas()->drawFastVLine(middleX, 0, screenWidth, color);

  const int x0 = middleX + xValue * 64;
  const int y0 = middleY - yValue * 64;
  const int defaultRadius = 32;
  const int bigCircleMargin = 4;

  gfx->fillCircle(x0, y0, defaultRadius + bigCircleMargin, ui->getForegroundColor());
  gfx->fillCircle(x0, y0, defaultRadius, color);
  gfx->drawCircle(x0, y0, defaultRadius, ui->getBackgroundColor());
}

void OswAppWaterLevel::drawBar(OswHal* hal, const float value, char text, const int x) {
  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();

  const int fontHeight = 1;
  hal->getCanvas()->setTextSize(fontHeight);

  const int maxVal = 2;
  const int redComponent = min(255, max(0, (int)((2 + value) / (2 * maxVal) * 255)));
  const int greenComponent = 255 - min(255, max(0, (int)((2 + value) / (2 * maxVal) * 255)));

  const int width = 10;
  const int height = 10;

  const int barHeight = min(100, (int)(32 * abs(value) / maxVal * 2));
  const int yOffset = value <= 0 ? barHeight : 0;

  gfx->fillRFrame(x, 120 - 5 - yOffset, width, height + barHeight, 5, rgb565(redComponent, greenComponent, 0));

  bool isMiddleValue = value > -0.25 && value < 0.25;

  const int backgroundColor = isMiddleValue ? ui->getSuccessColor() : ui->getBackgroundColor();
  const int foregroundColor = isMiddleValue ? ui->getBackgroundColor() : ui->getForegroundColor();

  gfx->fillCircle(x + 4, 120 + 2, width / 2 + 3, backgroundColor);

  gfx->drawCircle(x + 4, 120 + 2, width / 2 + 3, ui->getForegroundColor());

  hal->getCanvas()->setCursor(x + 2, 120 - 2);

  hal->getCanvas()->setTextColor(foregroundColor, backgroundColor);
  hal->getCanvas()->print(text);
}

void OswAppWaterLevel::barsDisplay(OswHal* hal) {
  const float xValue = hal->getAccelerationX();
  const float yValue = hal->getAccelerationY();
  const float zValue = hal->getAccelerationZ();

  try {
    drawBar(hal, xValue, 'X', 80);
    drawBar(hal, yValue, 'Y', 120);
    drawBar(hal, zValue, 'Z', 160);

  } catch (const std::exception& e) {
    hal->getCanvas()->print(e.what());
  }
}
int displayMode = 1;
void OswAppWaterLevel::loop(OswHal* hal) {
  hal->getCanvas()->fillScreen(ui->getBackgroundColor());

  // to better understand the accelerometer values use the debug function
  // debug(hal);

  if (hal->btnHasGoneDown(BUTTON_2)) {
    displayMode = (displayMode + 1) % 2;
  }
  switch (displayMode) {
    case 0:
      barsDisplay(hal);
      break;

    default:
      circlesDisplay(hal);
      break;
  }

  hal->requestFlush();
}

void OswAppWaterLevel::stop(OswHal* hal) {}
