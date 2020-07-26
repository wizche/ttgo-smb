#ifndef __CLOUD_H
#define __CLOUD_H

#include "config.h"

LV_IMG_DECLARE(cloud);
LV_FONT_DECLARE(emulogic_11);

class Cloud {
    private:
        lv_obj_t *parent;
        lv_obj_t *cloudContainer;
        lv_anim_t anim;
        int x, y = 0;

    public:
        Cloud(lv_obj_t *mparent, int px, int py);
        void render(const char *datestring);
};

#endif //__CLOUD_H
