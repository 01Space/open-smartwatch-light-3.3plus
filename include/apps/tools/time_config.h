#ifndef OSW_APP_TIME_FROM_WEB_H
#define OSW_APP_TIME_FROM_WEB_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppTimeConfig : public OswApp {
 public:
  OswAppTimeConfig(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppTimeConfig(){};

 private:
  void enterManualMode(OswHal* hal);
  void handleIncrementButton(OswHal* hal);
  void handleDecrementButton(OswHal* hal);
  void handleNextButton(OswHal* hal);
  short timeZone;
  float daylightOffset;
  bool manualSettingScreen = false;
  int8_t manualSettingStep = 0;
  int16_t manualSettingTimestamp[11];
};

#endif
