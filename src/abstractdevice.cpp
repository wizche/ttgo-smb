#include "abstractdevice.h"

#ifdef SIMULATOR

DateTime AbstractDevice::getDateTime()
{
    return currentDateTime;
}

void AbstractDevice::setDateTime(DateTime newDateTime)
{
    this->currentDateTime = newDateTime;
}

int AbstractDevice::getBatteryLevel()
{
    return 100;
}

#else

DateTime AbstractDevice::getDateTime()
{
    RTC_Date curr = TTGOClass::getWatch()->rtc->getDateTime();
    return DateTime(curr.day, curr.month, curr.year, curr.hour, curr.minute, curr.second);
}

void AbstractDevice::setDateTime(DateTime newDateTime)
{
    RTC_Date newRtcDate = RTC_Date(newDateTime.year, newDateTime.month, newDateTime.day, newDateTime.hour, newDateTime.minute, newDateTime.second);
    TTGOClass::getWatch()->rtc->setDateTime(newRtcDate);
}

int AbstractDevice::getBatteryLevel()
{
    return TTGOClass::getWatch()->power->getBattPercentage();
}
#endif