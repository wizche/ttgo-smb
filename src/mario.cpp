#include "mario.h"

Mario::Mario(lv_obj_t *mparent, int px, int py) : pos{px, py}
{
    parent = mparent;
    x = px;
    y = py;
    jump();
}

int Mario::getJumpDurationMs()
{
    return JUMP_DURATION_MS;
}

void Mario::render()
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);
    marioContainer = lv_cont_create(parent, NULL);
    lv_obj_add_style(marioContainer, LV_OBJ_PART_MAIN, &style);

    lv_obj_set_pos(marioContainer, pos[0], pos[1]);
    Serial.printf("X: %d Y: %d\n", pos[0], pos[1]);
    lv_obj_set_width(marioContainer, width);
    lv_obj_set_height(marioContainer, height);

    marioImg = lv_img_create(marioContainer, NULL);
    lv_img_set_src(marioImg, &mario);
    lv_obj_set_pos(marioImg, 0, 0);
    lv_obj_set_width(marioImg, width);
    lv_obj_set_height(marioImg, height);
    lv_obj_align(marioImg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    //lv_obj_align(marioImg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    int mx = frames.at(2).first;
    int my = frames.at(2).second;
    lv_img_set_offset_x(marioImg, mx);
    lv_img_set_offset_y(marioImg, my);
    Serial.printf("%d %d\n", mx, my);
}

void Mario::update()
{
    if (jumping)
    {
        jumping -= 1;
    }

    // if vertical velocity and position match ground, reset
    if (vel[1] > 0.0 && pos[1] >= y)
    {
        Serial.printf("Jump is over, reset!\n");
        vel[1] = 0.0;
        acc[1] = 0.0;
        jumping = 0;
        pos[1] = y;
    }

    if (abs(vel[0]) > maxSpeed)
    {
        Serial.printf("Max speed!\n");
        vel[0] -= 0.05 * vel[0] / abs(vel[0]);
        acc[0] = 0.0f;
    }

    if (pos[0] >= LV_HOR_RES && (vel[0] > 0.0f))
    {
        Serial.printf("We went out of screen!\n");
        acc[0] = 0.07f;
        pos[0] = x-width;
        frameIndex = 0.0f;
    }

    vel[0] += acc[0];
    vel[1] += acc[1];

    pos[0] += vel[0];
    pos[1] += vel[1];

    lv_obj_set_pos(marioContainer, pos[0], pos[1]);
    Serial.printf("pos x %d;%d, jump %d, vel %.2f;%.2f, acc: %.2f;%.2f\n", pos[0], pos[1], jumping, vel[0], vel[1], acc[0], acc[1]);

    if (jumping)
    {
        lv_img_set_offset_x(marioImg, frames[5].first);
        lv_img_set_offset_y(marioImg, frames[5].second);
    }
    else if (vel[0] > 0.0f)
    {
        // we need to update the frame proportionally to the speed of mario
        double dt = FPS / 1000.0;
        double framesSpeed = vel[0] * 4;
        frameIndex += (framesSpeed * dt);
        int frame = ((int) floor(frameIndex) % enabledFrames.size());
        Serial.printf("idx %d, selframe %d, frameindex %f, speed %3.3f\n", frame, enabledFrames[frame], frameIndex, framesSpeed);
        lv_img_set_offset_x(marioImg, frames[enabledFrames[frame]].first);
        lv_img_set_offset_y(marioImg, frames[enabledFrames[frame]].second);
    }
    else
    {
        lv_img_set_offset_x(marioImg, frames[0].first);
        lv_img_set_offset_y(marioImg, frames[0].second);
    }


    if(pos[0] > 85 && pos[0] < 95){
        jump();
    }
}

void Mario::jump()
{
    if (vel[1] > 0.0)
    {
        return;
    }
    Serial.printf("JUMP!\n");
    vel[1] = -5.5;
    acc[1] = 0.55;
    jumping = 40;
}

void Mario::jumpMario(lv_task_t *task)
{
    lv_obj_t *marioImg = (lv_obj_t *)task->user_data;
    lv_img_set_src(marioImg, &mario_jump);
}

void Mario::resetMario(lv_task_t *task)
{
    lv_obj_t *marioImg = (lv_obj_t *)task->user_data;
    lv_img_set_src(marioImg, &mario);
}

void Mario::run()
{
    Serial.printf("Running!");
    enabledFrames = {1, 2, 3};
    running = true;
}
