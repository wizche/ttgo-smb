#ifndef __GUI_H
#define __GUI_H

#include "config.h"

#include "block.h"
#include "mario.h"
#include "clouds.h"
#include "abstractdevice.h"

LV_FONT_DECLARE(emulogic_10);
LV_FONT_DECLARE(emulogic_11);
LV_IMG_DECLARE(world);
LV_IMG_DECLARE(block);
LV_IMG_DECLARE(coin);

class Gui
{
private:
  AbstractDevice *abstractDevice;
  lv_style_t timeStyle;
  lv_obj_t *mainView = nullptr;
  lv_obj_t *batteryLabelValue = nullptr;
  lv_obj_t *stepLabelValue = nullptr;
  lv_obj_t *coinsLabel = nullptr;

  int start_x, end_x, start_y, end_y = 0;
  unsigned int wakeUpCounter = 0;

  //Mario *marioHour, *marioMinutes, *marioSeconds = nullptr;
  Mario *mario;
  
  Block *boxHour, *boxMinutes, *boxSeconds = nullptr;
  Clouds *dateCloud = nullptr;
  Gestures process_gesture(lv_event_t event);

  static void lv_update_task(struct _lv_task_t *);
  static void event_handler(lv_obj_t *obj, lv_event_t event);
  static void update(struct _lv_task_t *);
  void sleep_task(struct _lv_task_t *task);

public:
  Gui(AbstractDevice *ad);

  void setupGui();
  void updateTime();
  void updateDate();
  void updateBatteryLevel();
  void updateStepCounter(unsigned int steps);
  void updateWakeupCount();
  void updateFrame();
};

#endif /*__GUI_H */
