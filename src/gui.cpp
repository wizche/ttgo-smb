#include "gui.h"

void Gui::setupGui()
{
  Serial.printf("Screensize: %dx%d\n", LV_HOR_RES, LV_VER_RES);
  lv_obj_t *scr = lv_scr_act();

  static lv_style_t mainStyle;
  lv_style_init(&mainStyle);
  lv_style_set_text_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
  lv_style_set_border_width(&mainStyle, LV_STATE_DEFAULT, 0);
  lv_obj_add_style(scr, LV_OBJ_PART_MAIN, &mainStyle);

  lv_style_copy(&timeStyle, &mainStyle);
  lv_style_set_text_font(&timeStyle, LV_STATE_DEFAULT, &emulogic_11);
  mainView = lv_cont_create(scr, NULL);

  lv_obj_add_style(mainView, LV_OBJ_PART_MAIN, &timeStyle);
  lv_obj_set_size(mainView, LV_HOR_RES, LV_VER_RES);

  lv_obj_t *img_bin = lv_img_create(mainView, NULL);
  lv_img_set_src(img_bin, &world);
  lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);

  timeLabel = lv_label_create(mainView, NULL);
  lv_label_set_recolor(timeLabel, true);
  lv_obj_align(timeLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_set_hidden(timeLabel, true);

  RTC_Date curr = TTGOClass::getWatch()->rtc->getDateTime();
  // Characters
  marioHour = new Mario(mainView, 27, 170);
  marioHour->render();

  marioMinutes = new Mario(mainView, 77, 170);
  marioMinutes->render();

  marioSeconds = new Mario(mainView, 127, 170);
  marioSeconds->render();

  // Boxes
  boxHour = new Box(mainView, 26, 107);
  boxHour->render(marioHour->getJumpDurationMs(), curr.hour);

  boxMinutes = new Box(mainView, 76, 107);
  boxMinutes->render(marioMinutes->getJumpDurationMs(), curr.minute);

  boxSeconds = new Box(mainView, 126, 107);
  boxSeconds->render(marioSeconds->getJumpDurationMs(), curr.second);

  // header
  static lv_style_t headerStyle;
  lv_style_copy(&headerStyle, &mainStyle);
  lv_style_set_text_font(&headerStyle, LV_STATE_DEFAULT, &emulogic_10);

  // step counter
  lv_obj_t *stepLabel = lv_label_create(mainView, NULL);
  lv_obj_align(stepLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
  lv_obj_add_style(stepLabel, LV_OBJ_PART_MAIN, &headerStyle);
  lv_label_set_align(stepLabel, LV_LABEL_ALIGN_LEFT);
  lv_label_set_text(stepLabel, "MARIO");

  stepLabelValue = lv_label_create(mainView, NULL);
  lv_obj_align(stepLabelValue, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 22);
  lv_obj_add_style(stepLabelValue, LV_OBJ_PART_MAIN, &headerStyle);
  lv_label_set_align(stepLabelValue, LV_LABEL_ALIGN_LEFT);
  lv_label_set_text(stepLabelValue, "000000");

  // wakeup counter (coins)
  lv_obj_t *coinImg = lv_img_create(mainView, NULL);
  lv_img_set_src(coinImg, &coin);
  lv_obj_align(coinImg, NULL, LV_ALIGN_IN_TOP_LEFT, 90, 22);

  coinsLabel = lv_label_create(mainView, NULL);
  lv_obj_align(coinsLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 102, 22);
  lv_obj_add_style(coinsLabel, LV_OBJ_PART_MAIN, &headerStyle);
  lv_label_set_align(coinsLabel, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(coinsLabel, "*00");

  // battery
  lv_obj_t *wtflabel = lv_label_create(mainView, NULL);
  lv_obj_add_style(wtflabel, LV_OBJ_PART_MAIN, &headerStyle);
  lv_obj_align(wtflabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  lv_label_set_align(wtflabel, LV_LABEL_ALIGN_RIGHT);
  lv_label_set_text(wtflabel, "TIME");

  batteryLabelValue = lv_label_create(mainView, NULL);
  lv_obj_align(batteryLabelValue, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 22);
  lv_obj_add_style(batteryLabelValue, LV_OBJ_PART_MAIN, &headerStyle);
  lv_label_set_align(batteryLabelValue, LV_LABEL_ALIGN_RIGHT);

  // date
  Cloud *dateCloud = new Cloud(mainView, 140, 50);
  dateCloud->render("23.07");


  lv_task_create(Gui::lv_update_task, 1000, LV_TASK_PRIO_MID, this);

  updateTime();
  updateBatteryLevel();

  lv_obj_set_user_data(mainView, this);
  lv_obj_set_event_cb(mainView, Gui::event_handler);
}

void Gui::updateTime()
{
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();
  if (modunit != ModTimeUnit::set)
  {
    lv_obj_set_hidden(timeLabel, false);
    char buff[128];
    if (modunit == ModTimeUnit::hour)
      sprintf(buff, "#ff0000 %02d#:%02d:%02d", curr.hour, curr.minute, curr.second);
    else
      sprintf(buff, "%02d:#ff0000 %02d#:%02d", curr.hour, curr.minute, curr.second);

    Serial.printf("Clock set mode...%s\n", buff);
    lv_label_set_text(timeLabel, buff);
    lv_obj_align(timeLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  }
  else
  {
    lv_obj_set_hidden(timeLabel, true);
  }

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

void Gui::modifyTime(int increment)
{
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();
  if (modunit == ModTimeUnit::hour)
  {
    curr.hour = abs(curr.hour + increment) % 24;
    curr.second = 0;
    Serial.printf("New hour: %d\n", curr.hour);
  }
  else
  {
    curr.minute = abs(curr.minute + increment) % 60;
    curr.second = 0;
    Serial.printf("New minute: %d\n", curr.minute);
  }
  ttgo->rtc->setDateTime(curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second);
}

Gestures Gui::process_gesture(lv_event_t event)
{
  lv_point_t pointAct;
  lv_indev_get_point(lv_indev_get_act(), &pointAct);
  if (event == LV_EVENT_PRESSED)
  {
    start_x = pointAct.x;
    start_y = pointAct.y;
  }
  else if (event == LV_EVENT_RELEASED)
  {
    end_x = pointAct.x;
    end_y = pointAct.y;

    lv_coord_t diffX = end_x - start_x;
    lv_coord_t diffY = end_y - start_y;

    //Serial.printf("X: %3d Y: %3d\n", pointAct.x, pointAct.y);
    Serial.printf("DiffX: %d, DiffY: %d\n", diffX, diffY);

    if (abs(diffX) > abs(diffY) && diffX > 50)
    {
      return Gestures::right;
    }
    else if (abs(diffX) > abs(diffY) && diffX < -50)
    {
      return Gestures::left;
    }
    else if (abs(diffX) < abs(diffY) && diffY > 50)
    {
      return Gestures::down;
    }
    else if (abs(diffX) < abs(diffY) && diffY < -50)
    {
      return Gestures::up;
    }
    return Gestures::none;
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
  Gui *gui = (Gui *)lv_obj_get_user_data_ptr(obj);
  switch (gui->process_gesture(event))
  {
  case Gestures::right:
    Serial.printf("swipe right\n ");
    gui->modunit = ModTimeUnit(abs(gui->modunit - 1) % (ModTimeUnit::minute + 1));
    Serial.printf("Unit: %d\n", gui->modunit);
    break;
  case Gestures::left:
    Serial.printf("swipe left\n ");
    gui->modunit = ModTimeUnit((gui->modunit + 1) % (ModTimeUnit::minute + 1));
    Serial.printf("Unit: %d\n", gui->modunit);
    break;
  case Gestures::up:
    Serial.printf("swipe up\n ");
    gui->modifyTime(1);
    break;
  case Gestures::down:
    Serial.printf("swipe down\n ");
    gui->modifyTime(-1);
    break;
  default:
    break;
    //Serial.printf("No gesture detected!");
  }
}

void Gui::lv_update_task(struct _lv_task_t *task)
{
  Gui *gui = (Gui *)task->user_data;
  gui->updateTime();
}
