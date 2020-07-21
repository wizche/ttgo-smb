#ifndef __GUI_H
#define __GUI_H

#include "config.h"

#include "mario.hpp"
#include "box.hpp"

LV_FONT_DECLARE(emulogic_12);
//LV_FONT_DECLARE(emulogic_14);
LV_FONT_DECLARE(emulogic_16);
LV_IMG_DECLARE(world);
LV_IMG_DECLARE(box);

class Gui
{
  private:
    lv_style_t timeStyle;
    lv_obj_t *timeLabel = nullptr;
    lv_obj_t *mainView = nullptr;
    lv_obj_t *batteryLabel = nullptr;
    
    int start_x, end_x, start_y, end_y = 0;
    
    Mario *marioHour, *marioMinutes, *marioSeconds = nullptr;
    Box *boxHour, *boxMinutes, *boxSeconds = nullptr;

    ModTimeUnit modunit;

    void modifyTime(int increment);
    Gestures process_gesture(lv_event_t event);
    
    static void lv_update_task(struct _lv_task_t *);
    static void event_handler(lv_obj_t *obj, lv_event_t event);
    void sleep_task(struct _lv_task_t *task);
    
  public:    
    void setupGui();
    void updateTime();
    void updateBatteryLevel();
  
};

#endif /*__GUI_H */
