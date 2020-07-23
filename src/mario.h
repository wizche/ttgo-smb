#ifndef __MARIO_H
#define __MARIO_H

#include "config.h"

LV_IMG_DECLARE(mario);
LV_IMG_DECLARE(mario_jump);

class Mario
{
private:
    lv_obj_t *parent;
    lv_obj_t *marioContainer;
    lv_obj_t *marioImg;
    lv_anim_t marioAnim;
    int x, y = 0;
    const unsigned int JUMP_DURATION_MS = 300;
    static void resetMario(lv_task_t *task);
    static void jumpMario(lv_task_t *task);

public:
    Mario(lv_obj_t *mparent, int px, int py);
    int getJumpDurationMs();
    void render();
    void jump();
};

#endif // __MARIO_H
