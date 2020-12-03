#ifndef __BOX_H
#define __BOX_H

#include "config.h"

LV_IMG_DECLARE(box);
LV_FONT_DECLARE(emulogic_11);

class Box {
    private:
        lv_obj_t *parent;
        lv_obj_t *boxContainer;
        lv_anim_t boxAnim;
        lv_obj_t *timeLabel;
        int currentValue;
        int animationDelayMs = 0;
        int x, y = 0;
        const unsigned int HIT_DURATION_MS = 150;

    public:
        Box(lv_obj_t *mparent, int px, int py);

        int getCurrentValue();
        void render(int animationDelayMs, int initialValue);
        void hit(int newValue);
        static void updateTime(struct _lv_anim_t* animstruct);
};

#endif //__BOX_H
