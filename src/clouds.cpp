#include "clouds.h"

Clouds::Clouds(AbstractDevice *ad, lv_obj_t *mparent, int px, int py)
{
    abstractDevice = ad;
    parent = mparent;
    x = px;
    y = py;
}

void Clouds::update(){
    auto curr = abstractDevice->getDateTime();
    int mi = curr.month;
    int day = curr.day;
    const char *m = months[mi];
    custom_log("New date %02d.%02d | %s\n", day, mi, m);

    char buff[3];
    sprintf(buff, "%02d", day);
    lv_label_set_text_fmt(dayLabel, buff);
    lv_obj_align(dayLabel, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_align(dayLabel, LV_LABEL_ALIGN_CENTER);

    lv_label_set_text_fmt(monthLabel, m);
    lv_obj_align(monthLabel, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_label_set_align(monthLabel, LV_LABEL_ALIGN_RIGHT);
}

void Clouds::render()
{  
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, LV_STATE_DEFAULT, lv_color_hex(0xca4d00));
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
    lv_style_set_text_font(&style, LV_STATE_DEFAULT, &emulogic_10);
    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);

    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, lv_anim_path_ease_in_out);

    cloudDayContainer = lv_cont_create(parent, NULL);
    lv_obj_add_style(cloudDayContainer, LV_OBJ_PART_MAIN, &style);
    lv_obj_set_pos(cloudDayContainer, x-84, y-10);
    lv_obj_set_size(cloudDayContainer, 50, 32);    

    cloudMonthContainer = lv_cont_create(parent, NULL);
    lv_obj_add_style(cloudMonthContainer, LV_OBJ_PART_MAIN, &style);
    lv_obj_set_pos(cloudMonthContainer, x, y);
    lv_obj_set_size(cloudMonthContainer, 50, 32);    

    lv_obj_t *cloudDayImg = lv_img_create(cloudDayContainer, NULL);
    lv_img_set_src(cloudDayImg, &cloud);
    lv_obj_align(cloudDayImg, NULL, LV_ALIGN_CENTER, 0, 0);

    dayLabel = lv_label_create(cloudDayContainer, NULL);

    lv_obj_t *cloudMonthImg = lv_img_create(cloudMonthContainer, NULL);
    lv_img_set_src(cloudMonthImg, &cloud);
    lv_obj_align(cloudMonthImg, NULL, LV_ALIGN_CENTER, 5, 0);

    monthLabel = lv_label_create(cloudMonthContainer, NULL);

    lv_anim_init(&animDay);
    lv_anim_set_var(&animDay, cloudDayContainer);
    lv_anim_set_exec_cb(&animDay, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_repeat_count(&animDay, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&animDay, 2000);
    lv_anim_set_playback_time(&animDay, 2000);
    lv_anim_set_delay(&animDay, 500);
    lv_anim_set_values(&animDay, x-84, LV_HOR_RES-130);
    lv_anim_start(&animDay);   
    lv_anim_set_path(&animDay, &path);

    lv_anim_init(&animMonth);
    lv_anim_set_var(&animMonth, cloudMonthContainer);
    lv_anim_set_exec_cb(&animMonth, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_repeat_count(&animMonth, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&animMonth, 2500);
    lv_anim_set_playback_time(&animMonth, 2500);
    lv_anim_set_values(&animMonth, x, LV_HOR_RES-50);
    lv_anim_start(&animMonth);        
    lv_anim_set_path(&animMonth, &path);

}