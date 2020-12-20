#include "abstractdevice.h"

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

int AbstractDevice::getBatteryLevel(){
    return TTGOClass::getWatch()->power->getBattPercentage();
}