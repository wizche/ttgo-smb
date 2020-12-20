#include "mario.h"

Mario::Mario(lv_obj_t *mparent, int px, int py, int mwidth, int mheight)
    : BasicObject(mparent, px, py, mwidth, mheight), initialPosition{px, py}
{
}

int Mario::getJumpDurationMs()
{
    return abs((int)(dt * ((jumpVel / jumpAcc)) * 1000.0)) - 10;
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

    lv_obj_set_pos(marioContainer, x, y);
    custom_log("Mario rendered at X: %d Y: %d, W: %d, H: %d\n", x, y, width, height);
    lv_obj_set_width(marioContainer, width);
    lv_obj_set_height(marioContainer, height);

    marioImg = lv_img_create(marioContainer, NULL);
    lv_img_set_src(marioImg, &mario_player);
    lv_obj_set_pos(marioImg, 0, 0);
    lv_obj_set_width(marioImg, width);
    lv_obj_set_height(marioImg, height);
    lv_obj_align(marioImg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    int mx = frames.at(2).first;
    int my = frames.at(2).second;
    lv_img_set_offset_x(marioImg, mx);
    lv_img_set_offset_y(marioImg, my);
    custom_log("%d %d\n", mx, my);
}

void Mario::update()
{
    // if vertical velocity and position match ground, reset
    if (vel[1] > 0.0 && y >= initialPosition[1])
    {
        custom_log("Jump is over, reset!\n");
        vel[1] = 0.0;
        acc[1] = 0.0;
        y = initialPosition[1];
        jumping = false;
    }

    if (abs(vel[0]) > maxSpeed)
    {
        //custom_log("Max speed!\n");
        vel[0] -= 0.05 * vel[0] / abs(vel[0]);
        acc[0] = 0.0f;
    }

    if (x >= LV_HOR_RES && (vel[0] > 0.0f))
    {
        custom_log("We went out of screen!\n");
        acc[0] = 0.07f;
        x = initialPosition[0] - width;
        frameIndex = 0.0f;
        maxSpeed = rand() % 3 + 4;
        vel[0] = maxSpeed;
    }

    vel[0] += acc[0];
    vel[1] += acc[1];

    x += vel[0];
    y += vel[1];

    lv_obj_set_pos(marioContainer, x, y);
    //custom_log("pos x %d;%d, jump %d, vel %.2f;%.2f, acc: %.2f;%.2f | jumps %lu vel %.2f acc %.2f\n", x, y, jumping, vel[0], vel[1], acc[0], acc[1], jumpTargets.size(), jumpVel, jumpAcc);

    // SPRITE SELECTION
    // when running
    if (vel[0] > 0.0f && !jumping)
    {
        double framesSpeed = vel[0] * 4;
        frameIndex += (framesSpeed * dt);
        int frame = ((int)floor(frameIndex) % enabledFrames.size());
        lv_img_set_offset_x(marioImg, frames[enabledFrames[frame]].first);
        lv_img_set_offset_y(marioImg, frames[enabledFrames[frame]].second);
    }
    else if (jumping)
    {
        //jumping!
        lv_img_set_offset_x(marioImg, frames[5].first);
        lv_img_set_offset_y(marioImg, frames[5].second);
    }
    else
    {
        //standing
        lv_img_set_offset_x(marioImg, frames[0].first);
        lv_img_set_offset_y(marioImg, frames[0].second);
    }

    // compute jump only if not already jumping!
    if (!jumping)
    {
        // lets iterate over the scheduled jump to see if there is a match!
        std::vector<int>::iterator it = jumpTargets.begin();
        while (it != jumpTargets.end())
        {
            int targetX = *it;
            // we compute half of the jump since we want to target the highest point, not when mario is back to the ground!
            int steps = abs((int)floor(jumpVel / jumpAcc / 2.0));
            float futurePos = x + (steps * vel[0]);
            float futurePosNext = x + ((steps + 1) * vel[0]);

            //custom_log("JUMP target: %d, steps %d, f1 %3.0f, f2 %3.0f, vel %.2f;%.2f, acc: %.2f;%.2f\n",
            //              targetX, steps, futurePos, futurePosNext,
            //              vel[0], vel[1], acc[0], acc[1]);

            if (targetX >= futurePos && targetX <= futurePosNext)
            {
                if (jumping)
                {
                    custom_log("Skipping jump to %d, already jumping\n", targetX);
                    ++it;
                }
                else
                {
                    custom_log("Time to jump to hit %d!\n", targetX);
                    it = jumpTargets.erase(it);
                    vel[1] = jumpVel;
                    acc[1] = jumpAcc;
                    jumping = true;
                }
            }
            else
                ++it;
        }
    }
}

void Mario::stopJump()
{
    custom_log("Mario head hit something, better stop jumping!\n");
    vel[1] = 0.0;
}

void Mario::jump(int targetX)
{
    if (std::any_of(jumpTargets.begin(), jumpTargets.end(),
                    [&targetX](const int &p) { return p == targetX; }))
    {
        custom_log("Jump target %d already in the list!\n", targetX);
    }
    else
    {
        custom_log("Added jump target %d to list!\n", targetX);
        jumpTargets.push_back(targetX);
    }
}

void Mario::schedule(int position)
{
    jump(position);
}

void Mario::jumpMario(lv_task_t *task)
{
    lv_obj_t *marioImg = (lv_obj_t *)task->user_data;
    lv_img_set_src(marioImg, &mario_jump);
}

void Mario::resetMario(lv_task_t *task)
{
    lv_obj_t *marioImg = (lv_obj_t *)task->user_data;
    lv_img_set_src(marioImg, &mario_player);
}

void Mario::run()
{
    custom_log("Running!");
    enabledFrames = {1, 2, 3};
    running = true;
}
