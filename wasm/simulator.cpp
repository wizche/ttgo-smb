
#include "../src/gui.h"
#include "../src/abstractdevice.h"

static AbstractDevice *abstractDevice = new AbstractDevice();
static Gui *gui = new Gui(abstractDevice);

extern "C" { 
    void init_simulator(void);
    void update_date_time(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
}

void update_date_time(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second){
    DateTime newDateTime(day, month, year, hour, minute, second);
    abstractDevice->setDateTime(newDateTime);
}

void init_simulator(void)
{
    printf("Initializing GUI!");
    gui->setupGui();
    gui->updateTime();
    gui->updateBatteryLevel();
    gui->updateStepCounter(1337);
    gui->updateWakeupCount();
    gui->updateDate();
    printf("Gui initialized!");
}