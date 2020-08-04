#include "gui.h"

void Gui::setupGui()
{
  Serial.printf("Screensize: %dx%d\n", LV_HOR_RES, LV_VER_RES);
  lv_obj_t *scr = lv_scr_act();

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
  lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_style_set_border_width(&style, LV_STATE_DEFAULT, 0);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &emulogic_10);
  lv_obj_add_style(scr, LV_OBJ_PART_MAIN, &style);

  lv_obj_t *img_bin = lv_img_create(scr, NULL);
  lv_img_set_src(img_bin, &world);
  lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);

  RTC_Date curr = TTGOClass::getWatch()->rtc->getDateTime();
  // Characters
  marioHour = new Mario(scr, 27, 170);
  marioHour->render();

  marioMinutes = new Mario(scr, 77, 170);
  marioMinutes->render();

  marioSeconds = new Mario(scr, 127, 170);
  marioSeconds->render();

  // Boxes
  boxHour = new Box(scr, 26, 107);
  boxHour->render(marioHour->getJumpDurationMs(), curr.hour);

  boxMinutes = new Box(scr, 76, 107);
  boxMinutes->render(marioMinutes->getJumpDurationMs(), curr.minute);

  boxSeconds = new Box(scr, 126, 107);
  boxSeconds->render(marioSeconds->getJumpDurationMs(), curr.second);

  // header

  // step counter
  lv_obj_t *stepLabel = lv_label_create(scr, NULL);
  lv_obj_align(stepLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
  lv_label_set_align(stepLabel, LV_LABEL_ALIGN_LEFT);
  lv_label_set_text(stepLabel, "MARIO");

  stepLabelValue = lv_label_create(scr, NULL);
  lv_obj_align(stepLabelValue, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 22);
  lv_label_set_align(stepLabelValue, LV_LABEL_ALIGN_LEFT);
  lv_label_set_text(stepLabelValue, "000000");

  // wakeup counter (coins)
  lv_obj_t *coinImg = lv_img_create(scr, NULL);
  lv_img_set_src(coinImg, &coin);
  lv_obj_align(coinImg, NULL, LV_ALIGN_IN_TOP_LEFT, 90, 22);

  coinsLabel = lv_label_create(scr, NULL);
  lv_obj_align(coinsLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 102, 22);
  lv_label_set_align(coinsLabel, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(coinsLabel, "*00");

  // battery
  lv_obj_t *wtflabel = lv_label_create(scr, NULL);
  lv_obj_align(wtflabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  lv_label_set_align(wtflabel, LV_LABEL_ALIGN_RIGHT);
  lv_label_set_text(wtflabel, "TIME");

  batteryLabelValue = lv_label_create(scr, NULL);
  lv_obj_align(batteryLabelValue, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 22);
  lv_label_set_align(batteryLabelValue, LV_LABEL_ALIGN_RIGHT);

  // date
  dateCloud = new Clouds(scr, 150, 70);
  dateCloud->render();

  lv_task_create(Gui::lv_update_task, 1000, LV_TASK_PRIO_MID, this);

  updateTime();
  updateDate();
  updateBatteryLevel();

  lv_obj_set_user_data(scr, this);
  lv_obj_set_event_cb(scr, Gui::event_handler);
}

void Gui::updateDate()
{
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();
  dateCloud->update(curr.day, curr.month);
}

void Gui::updateTime()
{
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();

  marioSeconds->jump();
  boxSeconds->hit(curr.second);

  if (curr.minute != boxMinutes->getCurrentValue())
  {
    marioMinutes->jump();
    boxMinutes->hit(curr.minute);
  }

  if (curr.hour != boxHour->getCurrentValue())
  {
    marioHour->jump();
    boxHour->hit(curr.hour);
  }
}

void Gui::updateBatteryLevel()
{
  TTGOClass *ttgo = TTGOClass::getWatch();
  int p = ttgo->power->getBattPercentage();
  lv_label_set_text(batteryLabelValue, String(p).c_str());
  lv_label_set_align(batteryLabelValue, LV_LABEL_ALIGN_RIGHT);
  lv_obj_align(batteryLabelValue, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 22);
  Serial.printf("Battery is %2d %%\n", p);
}

void Gui::updateStepCounter(unsigned int steps)
{
  char buff[7];
  sprintf(buff, "%06u", steps);
  lv_label_set_text(stepLabelValue, buff);
  lv_obj_align(stepLabelValue, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 22);
  Serial.printf("Steps %06d\n", steps);
}

void Gui::updateWakeupCount()
{
  if (wakeUpCounter > 255)
  {
    wakeUpCounter = 0;
  }
  wakeUpCounter += 1;

  char buff[4];
  sprintf(buff, "*%02u", wakeUpCounter);
  lv_label_set_text(coinsLabel, buff);
  lv_obj_align(coinsLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 102, 22);
}

void Gui::event_handler(lv_obj_t *obj, lv_event_t event)
{
  // this should be improved once the following issue is fixed
  // https://github.com/lvgl/lvgl/issues/1650
}

void Gui::lv_update_task(struct _lv_task_t *task)
{
  Gui *gui = (Gui *)task->user_data;
  gui->updateTime();
}
