#include "box.h"

Box::Box(lv_obj_t *mparent, int px, int py, int width, int height, BoxType mtype, UpdateSubscribe *mupdatableShape)
    : BasicObject(mparent, px, py, width, height)
{
  type = mtype;
  updateSubscribe = mupdatableShape;
}

uint8_t Box::getCurrentValue()
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
  lv_obj_set_width(boxContainer, width);
  lv_obj_set_height(boxContainer, height);

  lv_obj_t *boxImg = lv_img_create(boxContainer, NULL);
  lv_img_set_src(boxImg, &box);
  lv_obj_align(boxImg, NULL, LV_ALIGN_CENTER, 0, 0);

  timeLabel = lv_label_create(boxContainer, NULL);
  lv_obj_align(timeLabel, NULL, LV_ALIGN_CENTER, (int)floor(width / 2.0) - 1, -1);
  lv_label_set_text(timeLabel, "00");

  lv_anim_init(&boxAnim);
  lv_anim_set_var(&boxAnim, boxContainer);
  lv_anim_set_exec_cb(&boxAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
  lv_anim_set_values(&boxAnim, y, y - height);
  lv_anim_set_playback_time(&boxAnim, HIT_DURATION_MS);
  lv_anim_set_time(&boxAnim, HIT_DURATION_MS);
}

void Box::updateTime()
{
  RTC_Date curr = TTGOClass::getWatch()->rtc->getDateTime();
  uint8_t value = 0;
  switch (type)
  {
  case BoxType::Seconds:
    value = curr.second;
    break;
  case BoxType::Minute:
    value = curr.minute;
    break;
  case BoxType::Hour:
    value = curr.hour;
    break;
  }

  Serial.printf("Updating box %d, previous %d: %d\n", type, currentValue, value);

  if(currentValue != value){
    currentValue = value;
    updateSubscribe->schedule(getCenter(), this);
  }
}

void Box::updateTimeCallback(struct _lv_anim_t *animstruct)
{
  Box *box = (Box *)lv_obj_get_user_data((lv_obj_t *)animstruct->var);
  char buff[3];
  sprintf(buff, "%02d", box->currentValue);
  lv_label_set_text(box->timeLabel, buff);
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
