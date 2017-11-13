#include "PowerManager.h"

void PMPowerOn(void)
{
    LEDBarModeSet(LED_MODE_BRIGHT, LED_COLOR_BLUE);
    MotorSpeedSet((uint8_t)PropertyGetValue(PROPERTY_ID_GEAR));
    DisplayOn();
}

void PMPowerOff(void)
{
    LEDBarModeSet(LED_MODE_SHUTDOWN, LED_COLOR_GREEN);
    MotorSpeedSet(0);
    DisplayOff();
}

void PMInitialize(void)
{
}

void PMPoll(void)
{
}

