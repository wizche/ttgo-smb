#include "gui.h"

static void lv_update_task(struct _lv_task_t *);
static void event_handler(lv_obj_t *obj, lv_event_t event);
void updateTime();

static lv_style_t timeStyle;
static lv_obj_t *timeLabel = nullptr;
static lv_obj_t *timeView = nullptr;
static ModTimeUnit modunit = ModTimeUnit::set;

static int start_x, end_x, start_y, end_y = 0;


void setupGui() { 

  Serial.printf("Screensize: %dx%d\n", LV_HOR_RES, LV_VER_RES);
  lv_obj_t *scr = lv_scr_act();

  static lv_style_t mainStyle;
  lv_style_init(&mainStyle);
  lv_style_set_text_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
  lv_obj_add_style(scr, LV_OBJ_PART_MAIN, &mainStyle);
  

  lv_style_copy(&timeStyle, &mainStyle);
  lv_style_set_border_width(&timeStyle, LV_STATE_DEFAULT, 0);
  lv_style_set_text_font(&timeStyle, LV_STATE_DEFAULT, &emulogic_16);
  timeView = lv_cont_create(scr, NULL);
  
  lv_obj_add_style(timeView, LV_OBJ_PART_MAIN, &timeStyle);
  lv_obj_set_size(timeView,  LV_HOR_RES, LV_VER_RES);

  lv_obj_t *img_bin = lv_img_create(timeView, NULL);
  lv_img_set_src(img_bin, &stage);
  lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);

  timeLabel = lv_label_create(timeView, NULL);
  lv_label_set_recolor(timeLabel, true);

  lv_task_create(lv_update_task, 1000, LV_TASK_PRIO_LOWEST, NULL);

  updateTime();
  //lv_obj_set_hidden(timeView, true);

  //lv_obj_set_event_cb(scr, event_handler);
  //lv_obj_set_gesture_parent(scr, true);
  lv_obj_set_event_cb(timeView, event_handler);
}

void updateTime() {
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();
  char buff[128];
  if(modunit == ModTimeUnit::hour){
    sprintf(buff, "#ff0000 %02d# %02d %02d", curr.hour, curr.minute, curr.second);
  } else if (modunit == ModTimeUnit::minute){
    sprintf(buff, "%02d #ff0000 %02d# %02d", curr.hour, curr.minute, curr.second);
  } else {
    sprintf(buff, "%02d:%02d:%02d", curr.hour, curr.minute, curr.second);
  }
  lv_label_set_text(timeLabel, buff);
  lv_obj_align(timeLabel, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_style_set_border_color(&timeStyle, LV_STATE_DEFAULT, LV_COLOR_MAKE(rand() % 255, rand() % 255, rand() % 255));
  lv_obj_refresh_style(timeView, LV_STYLE_BORDER_COLOR);
}

static Gestures process_gesture(lv_event_t event) {
  lv_point_t pointAct;
  lv_indev_get_point(lv_indev_get_act(), &pointAct);
  if (event == LV_EVENT_PRESSED) {
    start_x = pointAct.x;
    start_y = pointAct.y;
  } else if (event == LV_EVENT_RELEASED) {
    end_x = pointAct.x;
    end_y = pointAct.y;

    lv_coord_t diffX = end_x - start_x;
    lv_coord_t diffY = end_y - start_y;

    //Serial.printf("X: %3d Y: %3d\n", pointAct.x, pointAct.y);
    Serial.printf("DiffX: %d, DiffY: %d\n", diffX, diffY);

    if (abs(diffX) > abs(diffY) && diffX > 150) {
      return Gestures::right;
    } else if (abs(diffX) > abs(diffY) && diffX < -150) {
      return Gestures::left;
    } else if (abs(diffX) < abs(diffY) && diffY > 50) {
      return Gestures::down;
    } else if (abs(diffX) < abs(diffY) && diffY < -50) {
      return Gestures::up;
    }
    return Gestures::none;
  }
}

void modifyTime(int increment) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  RTC_Date curr = ttgo->rtc->getDateTime();
  if(modunit == ModTimeUnit::hour){
    curr.hour = abs(curr.hour + increment) % 24;
    curr.second = 0;
    Serial.printf("New hour: %d\n", curr.hour);
  } else {
    curr.minute = abs(curr.minute + increment) % 60;
    curr.second = 0;
    Serial.printf("New minute: %d\n", curr.minute);
  }
  ttgo->rtc->setDateTime(curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second);
}

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
  switch (process_gesture(event)) {
    case Gestures::right:
      Serial.printf(" swipe right\n ");
      modunit = ModTimeUnit(abs(modunit - 1) % (ModTimeUnit::minute + 1));
      Serial.printf("Unit: %d\n", modunit);
      break;
    case Gestures::left:
      Serial.printf(" swipe left\n ");
      modunit = ModTimeUnit((modunit + 1) % (ModTimeUnit::minute + 1));
      Serial.printf("Unit: %d\n", modunit);
      break;
    case Gestures::up:
        Serial.printf(" swipe up\n ");
        modifyTime(1);
        break;
    case Gestures::down:
        Serial.printf(" swipe down\n ");
        modifyTime(-1);
        break;
    default:
      break;
      //Serial.printf("No gesture detected!");
  }
}

static void lv_update_task(struct _lv_task_t *data)
{
  updateTime();
}
