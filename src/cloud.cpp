#include "cloud.h"

Cloud::Cloud(lv_obj_t *mparent, int px, int py)
{
    parent = mparent;
    x = px;
    y = py;
}

void Cloud::render(const char *datestring)
{  
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);

    cloudContainer = lv_cont_create(parent, NULL);
    lv_obj_add_style(cloudContainer, LV_OBJ_PART_MAIN, &style);
    lv_obj_set_pos(cloudContainer, x, y);
    lv_obj_set_width(cloudContainer, 42);
    lv_obj_set_height(cloudContainer, 32);

    lv_obj_t *cloudImg = lv_img_create(cloudContainer, NULL);
    lv_img_set_src(cloudImg, &cloud);
    lv_obj_align(cloudImg, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, cloudContainer);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&anim, 2500);
    lv_anim_set_playback_time(&anim, 2500);
    lv_anim_set_values(&anim, x, LV_HOR_RES-42);
    lv_anim_start(&anim);      
}