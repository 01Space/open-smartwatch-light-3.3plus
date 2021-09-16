#include "./apps/watchfaces/watchface_digital.h"
#include "./apps/watchfaces/watchface.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#define COLOR_BLACK rgb565(0, 0, 0)

void drawDate(const bool& useMMDDYYYY) {
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  OswHal* hal = OswHal::getInstance();
  const char* weekday = hal->getWeekday();

  hal->getDate(&dayInt, &monthInt, &yearInt);

  // we want to output a value like "Wed, 05/02/2021"

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(6.9), 80);

  hal->gfx()->print(weekday[0]);
  hal->gfx()->print(weekday[1]);
  hal->gfx()->print(weekday[2]);
  hal->gfx()->print(", ");

  // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do right
  // now is simply two if statements covering the 2 common conditions.
  if (useMMDDYYYY) {
    hal->gfx()->printDecimal(monthInt, 2);
    hal->gfx()->print("/");
    hal->gfx()->printDecimal(dayInt, 2);
    hal->gfx()->print("/");
  } else {
    hal->gfx()->printDecimal(dayInt, 2);
    hal->gfx()->print(".");
    hal->gfx()->printDecimal(monthInt, 2);
    hal->gfx()->print(".");
  }

  hal->gfx()->print(yearInt);
}

void timeOutput(uint32_t hour, uint32_t minute, uint32_t second) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(second, 2);
}

void drawTime() {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(5.5), 120);

  hal->getLocalTime(&hour, &minute, &second, &afterNoon);
  timeOutput(hour, minute, second);
  hal->gfx()->print(" ");
  if (afterNoon) {
    hal->gfx()->print(pm);
  } else {
    hal->gfx()->print(am);
  }
}

void drawTime24Hour() {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);

  hal->getLocalTime(&hour, &minute, &second);
  timeOutput(hour, minute, second);
}

void drawSteps() {
#ifdef OSW_FEATURE_STATS_STEPS
  uint8_t w = 8;
  OswAppWatchface::drawStepHistory(OswUI::getInstance(), (DISP_W / 2) - w * 3.5, 180, w, w * 4, OswConfigAllKeys::stepsPerDay.get());
#else
  OswHal* hal = OswHal::getInstance();
  uint32_t steps = hal->getStepsToday();
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(120, 210 - hal->gfx()->getTextOfsetRows(1) / 2);

  hal->gfx()->print(steps);
#endif
}

void OswAppWatchfaceDigital::setup() { useMMDDYYYY = OswConfigAllKeys::dateFormat.get() == "mm/dd/yyyy"; }

void OswAppWatchfaceDigital::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  drawDate(this->useMMDDYYYY);

  if (!OswConfigAllKeys::timeFormat.get()) {
    drawTime();
  } else {
    drawTime24Hour();
  }

  drawSteps();

  hal->requestFlush();
}

void OswAppWatchfaceDigital::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
