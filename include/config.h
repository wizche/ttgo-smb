#ifndef __CONFIG_H
#define __CONFIG_H

#define LILYGO_WATCH_2020_V1 // To use T-Watch2020 , please uncomment this line
#define LILYGO_WATCH_LVGL    //To use LVGL, you need to enable the macro LVGL

#define WATCH_FLAG_SLEEP_MODE _BV(1)
#define WATCH_FLAG_SLEEP_MODE _BV(1)
#define WATCH_FLAG_SLEEP_EXIT _BV(2)
#define WATCH_FLAG_BMA_IRQ _BV(3)
#define WATCH_FLAG_AXP_IRQ _BV(4)

enum ModTimeUnit
{
    set = 0,
    hour,
    minute
};
enum Gestures
{
    none = 0,
    left,
    right,
    up,
    down
};

#include <LilyGoWatch.h>

#endif //__CONFIG_H
