#include "config.h"
#include "gui.h"

TTGOClass *ttgo;

void setup()
{
    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    
    ttgo->begin();
    ttgo->lvgl_begin();

    //Check if the RTC clock matches, if not, use compile time
    ttgo->rtc->check();

    //Synchronize time to system time
    ttgo->rtc->syncToSystem();
    
    Serial.printf("%s", ttgo->rtc->formatDateTime());
    setupGui();


    ttgo->openBL();
}

void loop()
{
    lv_task_handler();
    delay(5);
}
