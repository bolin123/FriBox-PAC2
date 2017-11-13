#include "APP.h"
#include "Display.h"
#include "Button.h"
#include "Property.h"

static void buttonEventHandle(ButtonEvent_t event)
{
    bool power;
    uint16_t gear;
    switch(event)
    {
        case BUTTON_EVENT_SPEED_PUSH:
            gear = PropertyGetValue(PROPERTY_ID_GEAR);
            if(gear >= 3)
            {
                PropertySetValue(PROPERTY_ID_GEAR, 0);
            }
            else
            {
                PropertySetValue(PROPERTY_ID_GEAR, gear + 1);
            }
            DisplaySpeedLevel((uint8_t)PropertyGetValue(PROPERTY_ID_GEAR));
            MotorSpeedSet((uint8_t)PropertyGetValue(PROPERTY_ID_GEAR));
            break;
        case BUTTON_EVENT_POWER_PUSH:
            power = PropertyGetValue(PROPERTY_ID_POWER);
            PropertySetValue(PROPERTY_ID_POWER, !power);
            if(!power)
            {
                PMPowerOn();
            }
            break;
        case BUTTON_EVENT_SPEED_LONG_PUSH:
            gear = PropertyGetValue(PROPERTY_ID_GEAR);
            if(gear == 0)
            {
                WifiNetConfigStart();
                LEDBarModeSet(LED_MODE_BREATH, LED_COLOR_GREEN);
                DisplayNetStatus(SYS_NET_STATUS_NETCONFIG);
            }
            else if(gear == 3)
            {
                PropertySetValue(PROPERTY_ID_GEAR, gear + 1);
                DisplaySpeedLevel((uint8_t)PropertyGetValue(PROPERTY_ID_GEAR));
            }
            break;
        case BUTTON_EVENT_POWER_LONG_PUSH:
            power = PropertyGetValue(PROPERTY_ID_POWER);
            if(power)
            {
                //PropertySetValue(PROPERTY_ID_POWER, 0);
                PMPowerOff();
            }
            break;
        default:
            break;
    }
}

static void lcdDisplay(void)
{
    DisplayNetStatus(SYS_NET_STATUS_CONNECT);
    DisplayDeviceUID("0001023");
    DisplayPM2p5Value(0);
    DisplaySpeedLevel(0);
    DisplayConsumables(96);
}

void APPInitialize(void)
{
    lcdDisplay();
}

void APPPoll(void)
{
}

