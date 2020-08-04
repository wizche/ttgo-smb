#include "config.h"
#include "src/gui.h"

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
void setup()
{
    Serial.begin(115200);
    Serial.println("Woked-up!");
    ttgo = TTGOClass::getWatch();

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
    ttgo->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
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

    //Synchronize time to system time
    ttgo->rtc->syncToSystem();

    Serial.printf("%s\n", ttgo->rtc->formatDateTime());
    gui = new Gui();
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
                Serial.printf("Stepcounter: %d\n", ttgo->bma->getCounter());
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
