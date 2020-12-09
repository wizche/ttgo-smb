#ifndef __MARIO_H
#define __MARIO_H

#include "config.h"
#include "shape.h"

#include <map>
#include <cmath>

#define OFFSET -72

LV_IMG_DECLARE(mario);
LV_IMG_DECLARE(mario_jump);

class Mario
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

    lv_obj_t *parent;
    lv_obj_t *marioContainer;
    lv_obj_t *marioImg;
    lv_anim_t marioAnim;
    static void resetMario(lv_task_t *task);
    static void jumpMario(lv_task_t *task);

    int width=23;
    int height=46;
    int x, y = 0;
    float maxSpeed = 4.0;
    float vel[2] = { 0.0, 0.0 };
    float acc[2] = { 0.07, 0.25 };
    int pos[2] = {0, 0};
    // timesteps
    double dt = FPS / 1000.0;
    double frameIndex = 0.0f;
    float jumpVel = -6.5;
    float jumpAcc = 0.5;
    bool left, running, jumping = false;
    std::vector<int> enabledFrames;
    std::vector<std::pair<int, HitShape*>> jumpTargets;

    // methods
    int getJumpDurationMs();

public:
    Mario(lv_obj_t *mparent, int px, int py);
    void render();
    void jump(int targetX, HitShape *hittableShape);
    void update();
    void run();
};

#endif // __MARIO_H
