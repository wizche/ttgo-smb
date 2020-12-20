#include "datetime.h"

DateTime::DateTime(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second)
    : day(day), month(month), year(year), hour(hour), minute(minute), second(second)
{
    assert(month <= 11);
}

void DateTime::update(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second)
{
    assert(month <= 11);
    this->day = day;
    this->month = month;
    this->year = year;
    this->hour = hour;
    this->minute = minute;
    this->second = second;
}