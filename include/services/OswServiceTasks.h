#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
class OswServiceTaskWiFi;
class OswServiceTaskRawScreenServer;

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
    //extern OswServiceTaskExample example;
    extern OswServiceTaskWiFi wifi;
#ifdef DEBUG
    extern OswServiceTaskMemMonitor memory;
#endif
#if SERVICE_RAW_SCREEN == 1
    extern OswServiceTaskRawScreenServer screenServer;
#endif
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];
