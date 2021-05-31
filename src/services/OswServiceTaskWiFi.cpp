#include <time.h>
#include "./services/OswServiceTaskWiFi.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#else
#error only esp8266 and esp32 are supported
#endif

void OswServiceTaskWiFi::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
  this->enableWiFi();
  if(OswConfigAllKeys::wifiBootEnabled.get())
    this->connectWiFi();
}

/**
 * This provides the "Auto-AP"-Feature (create AP while wifi is unavailable)
 */
void OswServiceTaskWiFi::loop(OswHal* hal) {
  //TODO reconnect if client active and it should be connected -> m_enableClient
  if(this->m_enableClient) {
    if(this->m_autoAPTimeout and WiFi.status() == WL_CONNECTED) {
      //Nice - reset timeout
      this->m_autoAPTimeout = 0;
    } else if(!this->m_autoAPTimeout and WiFi.status() != WL_CONNECTED) {
      //Wifi is either disconnected or unavailable -> start timeout until we start our own ap
      this->m_autoAPTimeout = time(nullptr);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": [AutoAP] Timeout activated: 10 seconds");
#endif
    }

    if(!this->m_enableStation and this->m_autoAPTimeout and this->m_autoAPTimeout < time(nullptr) - 10) { //10 seconds no network -> auto ap!
      this->enableStation();
      this->m_enabledStationByAutoAP = true;
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": [AutoAP] Active (password is " + this->m_stationPass + ").");
#endif
    }

    if(this->m_queuedNTPUpdate and WiFi.status() == WL_CONNECTED) {
      configTime(OswConfigAllKeys::timeZone.get() * 3600 + 3600, OswConfigAllKeys::daylightOffset.get() * 3600, "pool.ntp.org", "time.nist.gov");
      this->m_queuedNTPUpdate = false;
      this->m_waitingForNTPUpdate = true;
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": [TimeViaNTP] Started update...");
#endif
    }

    // sometimes time(nullptr) returns seconds since boot
    // so check the request was resolved
    if (this->m_waitingForNTPUpdate and time(nullptr) > 1600000000) {
      this->m_waitingForNTPUpdate = false;
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": [TimeViaNTP] Update finished (time of " + time(nullptr) + ")!");
#endif
      hal->setUTCTime(time(nullptr));
    }
  }

  if(this->m_enabledStationByAutoAP and (WiFi.status() == WL_CONNECTED or !this->m_enableClient)) {
    this->disableStation();
    this->m_enabledStationByAutoAP = false;
#ifdef DEBUG
    Serial.println(String(__FILE__) + ": [AutoAP] Inactive.");
#endif
  }
}

void OswServiceTaskWiFi::stop(OswHal* hal) {
  OswServiceTask::stop(hal);
  this->disableWiFi();
}

/**
 * Enables wifi with the configured properties (caches the SSID/Pwd NOW)
 */
void OswServiceTaskWiFi::enableWiFi() {
  this->m_enableWiFi = true;
  this->updateWiFiConfig();
}

/**
 * Disables wifi, this will not modify isStationEnabled or isWiFiEnabled as these are properties
 */
void OswServiceTaskWiFi::disableWiFi() {
  this->m_enableWiFi = false;
  this->updateWiFiConfig();
}

WiFiClass* getNativeHandler() {
  return &WiFi;
}

/**
 * Is either the station active or are we currently connected to an ssid?
 */
bool OswServiceTaskWiFi::isConnected() {
  return this->m_enableStation or WiFi.status() == WL_CONNECTED;
}

/**
 * Returns either the station ip or the client ip (if connected), otherwise reports IPAddress()
 */
IPAddress OswServiceTaskWiFi::getIP() {
  if(WiFi.status() == WL_CONNECTED)
    return WiFi.localIP();
  else if(this->m_enableStation)
    return WiFi.softAPIP();
  return IPAddress();
}

void OswServiceTaskWiFi::queueTimeUpdateViaNTP() {
  this->m_queuedNTPUpdate = true;
}

bool OswServiceTaskWiFi::isWiFiEnabled() {
  return this->m_enableClient;
}

/**
 * Connect to the wifi, using the provided credentials from the config...
 */
void OswServiceTaskWiFi::connectWiFi() {
  this->m_enableClient = true;
  this->m_clientSSID = std::move(OswConfigAllKeys::wifiSsid.get());
  this->m_clientPass = std::move(OswConfigAllKeys::wifiPass.get());
  this->updateWiFiConfig();
  WiFi.begin(this->m_clientSSID.c_str(), this->m_clientPass.c_str());
  this->m_autoAPTimeout = 0;
  if(!this->m_queuedNTPUpdate)
    this->m_queuedNTPUpdate = OswConfigAllKeys::wifiAlwaysNTPEnabled.get();
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Connecting to SSID " + OswConfigAllKeys::wifiSsid.get() + "...");
#endif
}

void OswServiceTaskWiFi::disconnectWiFi() {
  this->m_enableClient = false;
  WiFi.disconnect(false);
  this->updateWiFiConfig();
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Disconnected wifi client...");
#endif
}

IPAddress OswServiceTaskWiFi::getClientIP() {
  return this->m_enableClient ? WiFi.localIP() : IPAddress();
}

bool OswServiceTaskWiFi::isStationEnabled() {
  return this->m_enableStation;
}

/**
 * This enables the wifi station mode
 * 
 * @param password Set the wifi password to this (at least 8 chars!), otherwise a random password will be choosen.
 */
void OswServiceTaskWiFi::enableStation(const String& password) {
  if(password.isEmpty())
    //Generate password
    this->m_stationPass = String((int)(rand() % 90000000 + 10000000)); //Generate random 8 chars long numeric password
  else
    this->m_stationPass = password;
  this->m_enableStation = true;
  this->m_enabledStationByAutoAP = false; //Revoke AutoAP station control
  this->updateWiFiConfig(); //This enables ap support
  WiFi.softAP(this->m_hostname.c_str(), this->m_stationPass.c_str());
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Enabled own station with SSID " + this->getStationSSID() + "...");
#endif
}

void OswServiceTaskWiFi::disableStation() {
  this->m_enableStation = false;
  WiFi.softAPdisconnect(false);
  this->updateWiFiConfig();
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Disabled station mode...");
#endif
}

IPAddress OswServiceTaskWiFi::getStationIP() {
  return this->m_enableStation ? WiFi.softAPIP() : IPAddress();
}

const String& OswServiceTaskWiFi::getStationSSID() const {
  return this->m_hostname;
}

const String& OswServiceTaskWiFi::getStationPassword() const {
  return this->m_stationPass;
}

/**
 * This updates the wifi modem state; including mode & hostname
 */
void OswServiceTaskWiFi::updateWiFiConfig() {
  this->m_hostname = std::move(OswConfigAllKeys::hostname.get());

#if defined(ESP8266)
  WiFi.hostname(this->m_hostname.c_str());
#elif defined(ESP32)
  WiFi.setHostname(this->m_hostname.c_str());
#endif

  if(!this->onlyOneModeSimultaneously and this->m_enableWiFi and this->m_enableClient and this->m_enableStation) {
    WiFi.mode(WIFI_MODE_APSTA);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Station & client");
#endif
  } else if(this->m_enableWiFi and this->m_enableStation) {
    //Check this BEFORE the client, so in case of onlyOneModeSimultaneously we prefer the station, when enabled!
    WiFi.mode(WIFI_MODE_AP);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Station");
#endif
  } else if(this->m_enableWiFi and this->m_enableClient) {
    WiFi.mode(WIFI_MODE_STA);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Client");
#endif
  } else {
    WiFi.mode(WIFI_MODE_NULL);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Off");
#endif
  }
}