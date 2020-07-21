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
        Box(lv_obj_t *mparent, int px, int py){
          parent = mparent;  
          x = px;
          y = py;          
        }

        int getCurrentValue(){
          return currentValue;
        }
        
        void render(int animationDelayMs, int initialValue){
          this->animationDelayMs = animationDelayMs;
          static lv_style_t mainStyle;
          lv_style_init(&mainStyle);
          lv_style_set_border_width(&mainStyle, LV_STATE_DEFAULT, 0);
          lv_style_set_text_font(&mainStyle, LV_STATE_DEFAULT, &emulogic_11);
          lv_style_set_text_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
          // TODO: fix transparency issue
          lv_style_set_bg_color(&mainStyle, LV_OBJ_PART_MAIN, lv_color_hex(0x6b8cff));
          boxContainer = lv_cont_create(parent, NULL);
          lv_obj_add_style(boxContainer, LV_OBJ_PART_MAIN, &mainStyle);
          lv_obj_set_pos(boxContainer, x, y);
          lv_obj_set_width(boxContainer, 25);
          lv_obj_set_height(boxContainer, 25);
          
          lv_obj_t *boxImg = lv_img_create(boxContainer, NULL);
          lv_img_set_src(boxImg, &box);
          lv_obj_align(boxImg, NULL, LV_ALIGN_CENTER, 0, 0);

          timeLabel = lv_label_create(boxContainer, NULL);
          lv_obj_align(timeLabel, NULL, LV_ALIGN_CENTER, 11, -1);
          char buff[3];
          sprintf(buff, "%02d", initialValue);
          currentValue = initialValue;
          lv_label_set_text(timeLabel, buff);
          
          lv_anim_init(&boxAnim);
          lv_anim_set_var(&boxAnim, boxContainer);
          lv_anim_set_exec_cb(&boxAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
          lv_anim_set_values(&boxAnim, y, y-25);
          lv_anim_set_playback_time(&boxAnim, HIT_DURATION_MS);
          lv_anim_set_delay(&boxAnim, animationDelayMs);
          lv_anim_set_time(&boxAnim, HIT_DURATION_MS);
        }

        void hit(int newValue){
            currentValue = newValue;
            lv_anim_start(&boxAnim);
            delay(animationDelayMs);
            char buff[3];
            sprintf(buff, "%02d", newValue);
            lv_label_set_text(timeLabel, buff);
        }
};
