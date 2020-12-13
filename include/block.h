#ifndef __BOX_H
#define __BOX_H

#include "config.h"
#include "shape.h"
#include <cmath>

LV_IMG_DECLARE(block);
LV_FONT_DECLARE(emulogic_11);

enum BoxType {
    Seconds=0, Minute, Hour
};

class Block: public BasicObject {
    private:
        lv_obj_t *boxContainer;
        lv_anim_t boxAnim;
        lv_obj_t *timeLabel;
        BoxType type;
        uint8_t currentValue = 0;
        UpdateSubscribe *updateSubscribe;
        const unsigned int HIT_DURATION_MS = 150;
        bool animating = false;

        // this callback is called once the hit animation is finished
        static void animation_finished(struct _lv_task_t *);

    public:
        Block(lv_obj_t *mparent, int px, int py, int width, int height, BoxType mtype, UpdateSubscribe *updatableShape);

        void updateTime();
        uint8_t getCurrentValue();
        void render();
        void hit();
    
        static void updateTimeCallback(struct _lv_anim_t* animstruct);
};

#endif //__BOX_H
