#ifndef __SIGNAL_ANALYSIS_H
#define __SIGNAL_ANALYSIS_H

#include <stdint.h>
#include "Config.h"
#include "Serial.h"

#define EEG_APP_MODE_COLLECT        1
#define EEG_APP_MODE_INFER          2
#define EEG_APP_MODE_COLLECT_CSP    3

#define DIR_TEXT_PORT               SERIAL_PORT_WIFI

void Signal_Analysis_Start(void);

#endif
