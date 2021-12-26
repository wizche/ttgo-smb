#include "config.h"
#include "gui.h"
#include <WiFi.h>
#include "secret.h"
#include "time.h"

#define DEFAULT_SCREEN_TIMEOUT 10 * 1000

enum
{
    Q_EVENT_BMA_INT,
    Q_EVENT_AXP_INT,
};

TTGOClass *ttgo = TTGOClass::getWatch();
QueueHandle_t g_event_queue_handle = NULL;
EventGroupHandle_t g_event_group = NULL;
EventGroupHandle_t isr_group = NULL;
bool lenergy = false;
bool tryNTPtime = true;

// Wifi variables
// The credetials are stored in src/secret.h file that doesnt need to be synched with the repo. The following format is used:
// #define SSID "MY_SSID"
// #define PASSWORD "MY_PASSWORD"
const char *ssid = SSID;
const char *ssid_passphrase = PASSWORD;

// NTP Settings : for more ionformations, https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
const char *ntpServer = "pool.ntp.org"; // (for worlwide NTP server)
// const char* ntpServer = "europe.pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

Gui *gui;

void initWakeupTriggers()
{
    AXP20X_Class *power = ttgo->power;
    BMA *sensor = ttgo->bma;
    Acfg cfg;
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    cfg.range = BMA4_ACCEL_RANGE_2G;
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);
    // Enable BMA423 accelerometer
    sensor->enableAccel();

    // Disable BMA423 isStepCounter feature
    sensor->enableFeature(BMA423_STEP_CNTR, false);
    // Enable BMA423 isTilt feature
    sensor->enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    sensor->enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    sensor->resetStepCounter();

    // Turn off feature interrupt
    // sensor->enableStepCountInterrupt();

    sensor->enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    sensor->enableWakeupInterrupt();
}

void low_energy()
{
    if (ttgo->bl->isOn())
    {
        xEventGroupSetBits(isr_group, WATCH_FLAG_SLEEP_MODE);
        ttgo->closeBL();
        ttgo->stopLvglTick();
        ttgo->bma->enableStepCountInterrupt(false);
        ttgo->displaySleep();
        lenergy = true;
        // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_2M);
        setCpuFrequencyMhz(20);
        Serial.println("ENTER IN LIGHT SLEEEP MODE");
        gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
        gpio_wakeup_enable((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        esp_light_sleep_start();
    }
    else
    {
        ttgo->startLvglTick();
        ttgo->displayWakeup();
        ttgo->rtc->syncToSystem();
        lv_disp_trig_activity(NULL);
        gui->updateTime();
        gui->updateBatteryLevel();
        gui->updateStepCounter(ttgo->bma->getCounter());
        gui->updateWakeupCount();
        gui->updateDate();
        ttgo->openBL();
        ttgo->bma->enableStepCountInterrupt();
    }
}

bool syncRtc2Ntp()
{
    //connect to WiFi
    custom_log("Connecting to %s\n", ssid);
    WiFi.begin(ssid, ssid_passphrase);
    // after 6 sec if WiFi is not found abort and avoid locking the setup
    int timeoutMs = 6000;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        timeoutMs -= 500;
        Serial.print(".");
        if (timeoutMs <= 0)
        {
            custom_log("\nWifi connection timed-out!\n");
            WiFi.mode(WIFI_OFF);
            return false;
        }
    }
    custom_log("\nConnected to %s \n", ssid);

    //init and get the time from NTP server
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return false;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    // RTC_Date updateRTC =(timeinfo.tm_year,timeinfo.tm_mon,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
    // ttgo->rtc->setDateTime(updateRTC);
    RTC_Date updateRTC;
    updateRTC.year = timeinfo.tm_year;
    updateRTC.month = timeinfo.tm_mon;
    updateRTC.day = timeinfo.tm_mday;
    updateRTC.hour = timeinfo.tm_hour;
    updateRTC.minute = timeinfo.tm_min;
    updateRTC.second = timeinfo.tm_sec;
    ttgo->rtc->setDateTime(updateRTC);
    custom_log("RTC time synched with NTP %2d.%2d.%4d\n", updateRTC.day, updateRTC.month, updateRTC.year);

    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Woked-up!");
    // ttgo = TTGOClass::getWatch();

    //initWakeupTriggers();

    //Create a program that allows the required message objects and group flags
    g_event_queue_handle = xQueueCreate(20, sizeof(uint8_t));
    g_event_group = xEventGroupCreate();
    isr_group = xEventGroupCreate();

    //Initialize TWatch
    ttgo->begin();
    ttgo->openBL();
    
    // Turn on the IRQ used
    ttgo->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    ttgo->power->enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ, AXP202_ON);
    ttgo->power->clearIRQ();

    
    // Turn off unused power
    // according to https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/docs/watch_2020_v2.md
    // TFT/TOUCHT
    // ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    // GPS Module
    ttgo->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);
    

    // Enable BMA423 interrupt ，
    // The default interrupt configuration,
    // you need to set the acceleration parameters, please refer to the BMA423_Accel example
    ttgo->bma->attachInterrupt();

    //Connection interrupted to the specified pin
    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(
        BMA423_INT1, [] {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            EventBits_t bits = xEventGroupGetBitsFromISR(isr_group);
            if (bits & WATCH_FLAG_SLEEP_MODE)
            {
                //! For quick wake up, use the group flag
                xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_BMA_IRQ, &xHigherPriorityTaskWoken);
            }
            else
            {
                uint8_t data = Q_EVENT_BMA_INT;
                xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
            }

            if (xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR();
            }
        },
        RISING);

    // Connection interrupted to the specified pin
    pinMode(AXP202_INT, INPUT);
    attachInterrupt(
        AXP202_INT, [] {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            EventBits_t bits = xEventGroupGetBitsFromISR(isr_group);
            if (bits & WATCH_FLAG_SLEEP_MODE)
            {
                //! For quick wake up, use the group flag
                xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_AXP_IRQ, &xHigherPriorityTaskWoken);
            }
            else
            {
                uint8_t data = Q_EVENT_AXP_INT;
                xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
            }
            if (xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR();
            }
        },
        FALLING);
    ttgo->lvgl_begin();

    //Check if the RTC clock matches, if not, use compile time
    ttgo->rtc->check();
    custom_log("RTC time: %s\n", ttgo->rtc->formatDateTime());

    if (tryNTPtime)
    {
        //Synchronize time to NTP
        syncRtc2Ntp();
    }

    ttgo->rtc->syncToSystem();
    custom_log("RTC time: %s\n", ttgo->rtc->formatDateTime());

    gui = new Gui(new AbstractDevice());
    gui->setupGui();
}

void loop()
{
    bool rlst;
    uint8_t data;

    //! Fast response wake-up interrupt
    EventBits_t bits = xEventGroupGetBits(isr_group);
    if (bits & WATCH_FLAG_SLEEP_EXIT)
    {
        if (lenergy)
        {
            lenergy = false;
            // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_160M);
            setCpuFrequencyMhz(160);
        }

        low_energy();

        if (bits & WATCH_FLAG_BMA_IRQ)
        {
            do
            {
                rlst = ttgo->bma->readInterrupt();
            } while (!rlst);
            xEventGroupClearBits(isr_group, WATCH_FLAG_BMA_IRQ);
        }
        if (bits & WATCH_FLAG_AXP_IRQ)
        {
            ttgo->power->readIRQ();
            ttgo->power->clearIRQ();
            //TODO: Only accept axp power pek key short press
            xEventGroupClearBits(isr_group, WATCH_FLAG_AXP_IRQ);
        }
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_EXIT);
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_MODE);
    }
    if ((bits & WATCH_FLAG_SLEEP_MODE))
    {
        //! No event processing after entering the information screen
        return;
    }

    //! Normal polling
    if (xQueueReceive(g_event_queue_handle, &data, 5 / portTICK_RATE_MS) == pdPASS)
    {
        switch (data)
        {
        case Q_EVENT_BMA_INT:
            do
            {
                rlst = ttgo->bma->readInterrupt();
            } while (!rlst);

            //! setp counter
            if (ttgo->bma->isStepCounter())
            {
                custom_log("Stepcounter: %d\n", ttgo->bma->getCounter());
                gui->updateStepCounter(ttgo->bma->getCounter());
            }
            break;
        case Q_EVENT_AXP_INT:
            ttgo->power->readIRQ();
            if (ttgo->power->isVbusPlugInIRQ())
            {
                Serial.println("Charging");
            }
            if (ttgo->power->isVbusRemoveIRQ())
            {
                //updateBatteryIcon(LV_ICON_CALCULATION);
                Serial.println("Finished charging");
            }
            if (ttgo->power->isChargingDoneIRQ())
            {
                Serial.println("Full charged");
            }
            if (ttgo->power->isPEKShortPressIRQ())
            {
                Serial.println("PEK Short press");
                custom_log("Current time: %s\n", ttgo->rtc->formatDateTime());
                ttgo->power->clearIRQ();
                low_energy();
                return;
            }
            ttgo->power->clearIRQ();
            break;
        default:
            break;
        }
    }

    if (ttgo->power->isVBUSPlug() || lv_disp_get_inactive_time(NULL) < DEFAULT_SCREEN_TIMEOUT)
    {
        lv_task_handler();
    }
    else
    {
        low_energy();
    }
}
