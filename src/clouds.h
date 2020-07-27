#ifndef __CLOUD_H
#define __CLOUD_H

#include "config.h"

LV_IMG_DECLARE(cloud);
LV_FONT_DECLARE(emulogic_10);

class Clouds {
    private:
        lv_obj_t *parent;
        lv_obj_t *cloudDayContainer;
        lv_obj_t *cloudMonthContainer;
        lv_anim_t animDay,animMonth;
        int x, y = 0;

    public:
        Clouds(lv_obj_t *mparent, int px, int py);
        void render(const char *datestring);
};

#endif //__CLOUD_H
