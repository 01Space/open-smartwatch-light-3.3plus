
#include "osw_app_runtime_test.h"

#include <config.h>
#include <gfx_util.h>
#include <mini-iot-client.h>
#include <osw_app.h>
#include <osw_hal.h>

MiniIotClient miniIot(MINI_IOT_DEVICENAME, MINI_IOT_SERVER, WIFI_SSID, WIFI_PASS);

void OswAppRuntimeTest::loop(OswHal* hal) {
  static long startup = millis();
  static float batteryVoltage = 0;
  static uint32_t batteryRaw = 0;
  static boolean readBattery = true;

  if (readBattery) {
    readBattery = false;

    uint8_t numSamples = 20;

    // measure X times
    for (uint8_t i = 0; i < numSamples; i++) {
      batteryVoltage = batteryVoltage + hal->getBatteryVoltage();
      batteryRaw = batteryRaw + hal->getBatteryRaw();
    }

    // return average
    batteryVoltage = batteryVoltage / numSamples;
    batteryRaw = batteryRaw / numSamples;

    Serial.print("Raw: ");
    Serial.print(batteryRaw);
    Serial.print(" Voltage: ");
    Serial.println(batteryVoltage);
  }

  uint32_t runtimeSeconds = (millis() - startup) / 1000;
  Serial.println(runtimeSeconds);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(116, 20);
  hal->getCanvas()->setTextSize(2);
  hal->getCanvas()->print(runtimeSeconds);
  
  if (runtimeSeconds >= 8) {
    miniIot.setDebugStream(&Serial);
    // upload data
    miniIot.connectToWifi();
    miniIot.appendWithTimestamp("battery.csv", String(String(batteryRaw) + "," + String(batteryVoltage)));
    delay(500);

    hal->deepSleep(15 * 60 * 1000);  // 15 minutes
  }
}
