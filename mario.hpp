#include "config.h"

LV_IMG_DECLARE(mario);
LV_IMG_DECLARE(mario_jump);

class Mario {
    private:
        lv_obj_t *parent;
        lv_obj_t *marioContainer;
        lv_obj_t *marioImg;
        lv_anim_t marioAnim;

        int x, y = 0;
        const unsigned int JUMP_DURATION_MS = 300;

        static void resetMario(lv_task_t *task){
            lv_obj_t *marioImg = (lv_obj_t*)task->user_data;
            lv_img_set_src(marioImg, &mario);
        }

        static void jumpMario(lv_task_t *task){
            lv_obj_t *marioImg = (lv_obj_t*)task->user_data;
            lv_img_set_src(marioImg, &mario_jump);
        }

    public:
        Mario(lv_obj_t *mparent, int px, int py){
            parent = mparent;  
            x = px;
            y = py;          
        }

        int getJumpDurationMs(){
            return JUMP_DURATION_MS;
        }
        void render(){
            static lv_style_t mainStyle;
            lv_style_init(&mainStyle);
            lv_style_set_border_width(&mainStyle, LV_STATE_DEFAULT, 0);
            // TODO: fix transparency issue
            lv_style_set_bg_color(&mainStyle, LV_OBJ_PART_MAIN, lv_color_hex(0x6b8cff));
            marioContainer = lv_cont_create(parent, NULL);
            lv_obj_add_style(marioContainer, LV_OBJ_PART_MAIN, &mainStyle);
            
            lv_obj_set_pos(marioContainer, x, y);
            lv_obj_set_width(marioContainer, 23);
            lv_obj_set_height(marioContainer, 46);
            
            marioImg = lv_img_create(marioContainer, NULL);
            lv_img_set_src(marioImg, &mario);
            lv_obj_align(marioImg, NULL, LV_ALIGN_CENTER, 0, 0);

            lv_anim_init(&marioAnim);
            lv_anim_set_var(&marioAnim, marioContainer);
            lv_anim_set_exec_cb(&marioAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_values(&marioAnim, y, y-40);
            lv_anim_set_playback_time(&marioAnim, JUMP_DURATION_MS);
            lv_anim_set_time(&marioAnim, JUMP_DURATION_MS);
            lv_anim_path_t path;
            lv_anim_path_init(&path);
            lv_anim_path_set_cb(&path, lv_anim_path_ease_in);
            lv_anim_set_path(&marioAnim, &path);
        }

        void jump(){
            lv_anim_start(&marioAnim);
            lv_task_once(lv_task_create(&Mario::resetMario, JUMP_DURATION_MS * 2, LV_TASK_PRIO_MID, marioImg));
            lv_task_once(lv_task_create(&Mario::jumpMario, 100, LV_TASK_PRIO_MID, marioImg));
        }
};
 
