#ifndef OSW_APP_FADEIN_DISPLAY_H
#define OSW_APP_FADEIN_DISPLAY_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppFadeInDisplay : public OswApp {
 public:
  OswAppFadeInDisplay(uint16_t fadeDuration) { _fadeDuration = fadeDuration; };
  void run(OswHal* hal);
  ~OswAppFadeInDisplay(){};

 private:
  uint16_t _fadeDuration;
};

#endif