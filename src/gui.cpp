#include "gui.h"

Gui::Gui(AbstractDevice *ad) : abstractDevice(ad) {}

void Gui::setupGui()
{
  lv_obj_t *scr = lv_scr_act();
  custom_log("Screensize: %dx%d | %p\n", LV_HOR_RES, LV_VER_RES, scr);

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

  // Characters
  mario = new Mario(scr, 0, 170, 23, 46);
  mario->render();

  // Blocks

  boxHour = new Block(abstractDevice, scr, 56, 107, 25, 25, BlockType::Hour, mario);
  boxHour->render();

  Block(abstractDevice, scr, 81, 107, 25, 25, BlockType::Empty, nullptr).render();

  boxMinutes = new Block(abstractDevice, scr, 106, 107, 25, 25, BlockType::Minute, mario);
  boxMinutes->render();

  Block(abstractDevice, scr, 131, 107, 25, 25, BlockType::Empty, nullptr).render();

  boxSeconds = new Block(abstractDevice, scr, 156, 107, 25, 25, BlockType::Seconds, mario);
  boxSeconds->render();

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
  dateCloud = new Clouds(abstractDevice, scr, 135, 50);
  dateCloud->render();

  lv_task_create(Gui::lv_update_task, 1000, LV_TASK_PRIO_MID, this);
  lv_task_create(Gui::update, 1000 / FPS, LV_TASK_PRIO_HIGH, this);

  updateTime();
  updateDate();
  updateBatteryLevel();
  mario->run();

  lv_obj_set_user_data(scr, this);
  lv_obj_set_event_cb(scr, Gui::event_handler);
}

void Gui::updateDate()
{
  dateCloud->update();
}

void Gui::updateTime()
{
  boxHour->updateTime();
  boxMinutes->updateTime();
  boxSeconds->updateTime();
}

void Gui::updateBatteryLevel()
{
  int p = abstractDevice->getBatteryLevel();
  std::stringstream ss;
  ss << p;
  lv_label_set_text(batteryLabelValue, ss.str().c_str());
  lv_label_set_align(batteryLabelValue, LV_LABEL_ALIGN_RIGHT);
  lv_obj_align(batteryLabelValue, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 22);
  custom_log("Battery is %2d %%\n", p);
}

void Gui::updateStepCounter(unsigned int steps)
{
  char buff[7];
  sprintf(buff, "%06u", steps);
  lv_label_set_text(stepLabelValue, buff);
  lv_obj_align(stepLabelValue, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 22);
  custom_log("Steps %06d\n", steps);
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

void Gui::update(struct _lv_task_t *task)
{
  Gui *gui = (Gui *)task->user_data;
  gui->updateFrame();
}

void Gui::updateFrame()
{
  if (mario->isCollidingOnce(boxHour))
  {
    custom_log("**** Mario colliding with hour block *********\n");
    boxHour->hit();
    mario->stopJump();
  }
  if (mario->isCollidingOnce(boxMinutes))
  {
    custom_log("**** Mario colliding with minutes block *********\n");
    boxMinutes->hit();
    mario->stopJump();
  }
  if (mario->isCollidingOnce(boxSeconds))
  {
    custom_log("**** Mario colliding with seconds block *********\n");
    boxSeconds->hit();
    mario->stopJump();
  }
  mario->update();
}
