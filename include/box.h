#ifndef __BOX_H
#define __BOX_H

#include "config.h"
#include "shape.h"
#include <cmath>

LV_IMG_DECLARE(box);
LV_FONT_DECLARE(emulogic_11);

enum BoxType {
    Seconds, Minute, Hour
};

class Box: public HitShape, public BasicObject {
    private:
        lv_obj_t *boxContainer;
        lv_anim_t boxAnim;
        lv_obj_t *timeLabel;
        BoxType type;
        uint8_t currentValue;
        const unsigned int HIT_DURATION_MS = 125;

        void updateTime();
    public:
        Box(lv_obj_t *mparent, int px, int py, int width, int height, BoxType type);

        uint8_t getCurrentValue();
        void render();
        void hit(int newValue);
    
        static void updateTimeCallback(struct _lv_anim_t* animstruct);
};

#endif //__BOX_H
