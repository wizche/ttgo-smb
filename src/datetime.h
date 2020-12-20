#ifndef __DATETIME_H
#define __DATETIME_H

#include <stdint.h>
#include <assert.h>

// Abstract away RTC_Date
class DateTime {
    public:
        uint8_t day;
        uint8_t month;
        uint16_t year;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;

        DateTime(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
        void update(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
};

#endif /*__DATETIME_H */
