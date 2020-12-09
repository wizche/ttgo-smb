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

void Box::render()
{
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &emulogic_11);
  lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);

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
  updateTime();

  lv_anim_init(&boxAnim);
  lv_anim_set_var(&boxAnim, boxContainer);
  lv_anim_set_exec_cb(&boxAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
  lv_anim_set_values(&boxAnim, y, y - 25);
  lv_anim_set_playback_time(&boxAnim, HIT_DURATION_MS);
  lv_anim_set_time(&boxAnim, HIT_DURATION_MS);
}

void Box::updateTime()
{
  char buff[3];
  RTC_Date curr = TTGOClass::getWatch()->rtc->getDateTime();
  sprintf(buff, "%02d", curr.second);
  lv_label_set_text(timeLabel, buff);
}

void Box::updateTimeCallback(struct _lv_anim_t *animstruct)
{
  Box *box = (Box *)lv_obj_get_user_data((lv_obj_t *)animstruct->var);
  box->updateTime();
}

void Box::hit(int delayMs)
{
  Serial.printf("Scheduling box hit in %d ms\n", delayMs);
  lv_anim_set_delay(&boxAnim, delayMs);

  // TODO: Is this the best way to deal with static class methods?
  // set_user_data actually create a copy (perf?)
  lv_obj_set_user_data(boxContainer, this);
  lv_anim_set_start_cb(&boxAnim, &Box::updateTimeCallback);
  lv_anim_start(&boxAnim);
}
