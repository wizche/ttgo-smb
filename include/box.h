#ifndef __BOX_H
#define __BOX_H

#include "config.h"
#include "shape.h"

LV_IMG_DECLARE(box);
LV_FONT_DECLARE(emulogic_11);

class Box: public HitShape {
    private:
        lv_obj_t *parent;
        lv_obj_t *boxContainer;
        lv_anim_t boxAnim;
        lv_obj_t *timeLabel;
        int currentValue;
        int animationDelayMs = 0;
        int x, y = 0;
        const unsigned int HIT_DURATION_MS = 150;

        void updateTime();
    public:
        Box(lv_obj_t *mparent, int px, int py);

        int getCurrentValue();
        void render();
        void hit(int newValue);

        static void updateTimeCallback(struct _lv_anim_t* animstruct);
};

#endif //__BOX_H
