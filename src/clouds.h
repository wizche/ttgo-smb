#ifndef __CLOUD_H
#define __CLOUD_H

#include "config.h"
#include "abstractdevice.h"

LV_IMG_DECLARE(cloud);
LV_FONT_DECLARE(emulogic_10);

class Clouds {
    private:
        const char *months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

        AbstractDevice *abstractDevice;
        lv_obj_t *parent;
        lv_obj_t *cloudDayContainer;
        lv_obj_t *cloudMonthContainer;
        lv_anim_t animDay,animMonth;
        lv_obj_t *monthLabel, *dayLabel;
        int x, y = 0;

    public:
        Clouds(AbstractDevice *ad, lv_obj_t *mparent, int px, int py);
        void render();
        void update();
};

#endif //__CLOUD_H
