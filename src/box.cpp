#include "box.h"

Box::Box(lv_obj_t *mparent, int px, int py)
{
  parent = mparent;
  x = px;
  y = py;
}

int Box::getCurrentValue()
{
  return currentValue;
}

void Box::render(int animationDelayMs, int initialValue)
{
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &emulogic_11);
  lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);

  this->animationDelayMs = animationDelayMs;
  boxContainer = lv_cont_create(parent, NULL);
  lv_obj_add_style(boxContainer, LV_OBJ_PART_MAIN, &style);
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
  lv_anim_set_values(&boxAnim, y, y - 25);
  lv_anim_set_playback_time(&boxAnim, HIT_DURATION_MS);
  lv_anim_set_delay(&boxAnim, animationDelayMs);
  lv_anim_set_time(&boxAnim, HIT_DURATION_MS);

  // TODO: Is this the best way to deal with static class methods? 
  // set_user_data actually create a copy (perf?)
  lv_obj_set_user_data(boxContainer, this);
  lv_anim_set_start_cb(&boxAnim, &Box::updateTime);
}

void Box::updateTime(struct _lv_anim_t* animstruct){
  Box *box = (Box *)lv_obj_get_user_data((lv_obj_t *)animstruct->var);
  char buff[3];
  sprintf(buff, "%02d", box->currentValue);
  lv_label_set_text(box->timeLabel, buff);
}

void Box::hit(int newValue)
{
  Serial.printf("Current time: %d\n", newValue);
  currentValue = newValue;
  lv_anim_start(&boxAnim);
}
