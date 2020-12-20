#ifndef __MARIO_H
#define __MARIO_H

#include "config.h"
#include "shape.h"

#include <map>
#include <cmath>

#define OFFSET -72

LV_IMG_DECLARE(mario_player);
LV_IMG_DECLARE(mario_jump);
LV_IMG_DECLARE(block);

class Mario : public UpdateSubscribe, public BasicObject
{
private:

    // Somehow the offset doesnt reflect what a balanced
    // person may think of it, its reversed WTF
    std::map<int, std::pair<int, int>> frames = {
        { 0, { 144, 0 }}, // still
        { 1, { 120, 0 }}, // move right
        { 2, { 96, 0 } }, // move right
        { 3, { 72, 0 } }, // move right
        { 4, { 48, 0 } }, // switchback
        { 5, { 24, 0 } }, // jump
    };
    lv_obj_t *marioContainer;
    lv_obj_t *marioImg;
    lv_anim_t marioAnim;
    static void resetMario(lv_task_t *task);
    static void jumpMario(lv_task_t *task);

    float maxSpeed = 4.0;
    float vel[2] = { 0.0, 0.0 };
    float acc[2] = { 0.07, 0.25 };
    int initialPosition[2] = {0, 0};
    // timesteps
    double dt = FPS / 1000.0;
    double frameIndex = 0.0f;
    float jumpVel = -7.0;
    float jumpAcc = 0.5;
    bool left, running, jumping = false;
    std::vector<int> enabledFrames = {0};;
    std::vector<int> jumpTargets;

    // methods
    void jump(int targetX);
    int getJumpDurationMs();

public:
    Mario(lv_obj_t *mparent, int px, int py, int width, int height);
    void render();
    void update();
    void run();
    void schedule(int position);
    void stopJump();
};

#endif // __MARIO_H
