#ifndef __ABSTRACTDEVICE_H
#define __ABSTRACTDEVICE_H

#include "config.h"
#include "datetime.h"

class AbstractDevice {
    public:
        int getBatteryLevel();
        DateTime getDateTime();
        void setDateTime(DateTime newDateTime);
};

#endif /*__ABSTRACTDEVICE_H */
