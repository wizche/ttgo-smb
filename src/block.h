#ifndef __BLOCK_H
#define __BLOCK_H

#include "config.h"
#include "shape.h"
#include "abstractdevice.h"

#include <cmath>

LV_IMG_DECLARE(block);
LV_IMG_DECLARE(emptyblock);
LV_FONT_DECLARE(emulogic_11);

enum BlockType {
    Seconds=0, Minute, Hour, Empty
};

class Block: public BasicObject {
    private:
        AbstractDevice *abstractDevice;
        lv_obj_t *boxContainer;
        lv_anim_t boxAnim;
        lv_obj_t *timeLabel;
        BlockType type;
        uint8_t currentValue = 0;
        UpdateSubscribe *updateSubscribe;
        const unsigned int HIT_DURATION_MS = 150;
        bool animating = false;

        // this callback is called once the hit animation is finished
        static void animation_finished(struct _lv_task_t *);

    public:
        Block(AbstractDevice *ad, lv_obj_t *mparent, int px, int py, int width, int height, BlockType mtype, UpdateSubscribe *updatableShape);

        void updateTime();
        uint8_t getCurrentValue();
        void render();
        void hit();
    
        static void updateTimeCallback(struct _lv_anim_t* animstruct);
};

#endif //__BLOCK_H
