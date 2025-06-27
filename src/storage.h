#ifndef storage_h
#define storage_h

#include <ctime>
#include <Preferences.h>
#include "co2ValuesArray.h"

extern Preferences preferencesCO2sensorValues;
extern Preferences preferencesSettings;

extern size_t nvsStatsUsedEntries;
extern size_t nvsStatsTotalEntries;

void storage_init(void);
void getNvsEntriesAndSaveToCO2Values(Co2valuesArray& co2values);

void getNVSStatistics();

bool getWifiIsDisabledSetting();
void setWifiIsDisabledSetting(bool aWiFiIsDisabledSetting);

#endif /* storage_h */
