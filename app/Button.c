#include "Sys.h"
#include "SysTimer.h"
#include "SysButton.h"
#include "Display.h"
#include "Motor.h"
#include "LEDBar.h"

#define BUTTON_POWER_PIN 0x2a
#define BUTTON_SPEED_PIN 0x2b

static SysButton_t g_powerButton;
static SysButton_t g_gearButton;

static bool g_poweron = false;
static MotorSpeedLevel_t g_speedlevel = 0;

static SysButtonState_t getButtonStatus(SysButton_t *button)
{
    if(button == &g_powerButton)
    {
        if(HalGPIOGetLevel(BUTTON_POWER_PIN))
        {
            return SYS_BUTTON_STATE_RELEASED;
        }
        else
        {
            return SYS_BUTTON_STATE_PRESSED;
        }
    }
    else if(button == &g_gearButton)
    {
        if(HalGPIOGetLevel(BUTTON_SPEED_PIN))
        {
            return SYS_BUTTON_STATE_RELEASED;
        }
        else
        {
            return SYS_BUTTON_STATE_PRESSED;
        }
    }
    return SYS_BUTTON_STATE_RELEASED;
}

static void netconfigTimeout(void *args)
{
    DisplayNetStatus(SYS_NET_STATUS_CONNECT);
    LEDBarModeSet(LED_MODE_BRIGHT, LED_COLOR_BLUE);
}

static uint8_t buttonHandle(SysButton_t *button, SysTime_t pressTime, SysButtonState_t state)
{
    static bool powerLongPress = false;
    static bool gearLongPress = false;
    
    if(state == SYS_BUTTON_STATE_RELEASED)
    {
        if(pressTime > 20)
        {
            if(button == &g_powerButton)
            {
                g_poweron = !g_poweron;
                if(g_poweron)
                {
                    DisplayOn();
                }
                else
                {
                    DisplayOff();
                }
            }
            else if(button == &g_gearButton)
            {
                g_speedlevel++;
                if(g_speedlevel > 3)
                {
                    g_speedlevel = 0;
                }
                MotorSpeedSet(1, g_speedlevel);
                MotorSpeedSet(2, g_speedlevel);
                MotorSpeedSet(3, g_speedlevel);
                MotorSpeedSet(4, g_speedlevel);
                DisplaySpeedLevel(g_speedlevel);
            }
            return 1;
        }
    }
    else
    {
        if(button == &g_powerButton)
        {
            if(pressTime > 5000)
            {
                powerLongPress = true;
            }
        }
        else if(button == &g_gearButton)
        {
            if(pressTime > 5000)
            {
                gearLongPress = true;
            }
        }

        if(powerLongPress && gearLongPress)
        {
            SysNetConfigStart();
            powerLongPress = false;
            gearLongPress = false;
            DisplayNetStatus(SYS_NET_STATUS_NETCONFIG);
            LEDBarModeSet(LED_MODE_BREATH, LED_COLOR_GREEN);
            SysTimerSet(netconfigTimeout, 60000, 0, NULL);
        }
    }
    return 0;
}

void ButtonInitialize(void)
{
    HalGPIOConfig(BUTTON_POWER_PIN, HAL_IO_INPUT);
    HalGPIOConfig(BUTTON_SPEED_PIN, HAL_IO_INPUT);

    SysButtonInitialize();
    
    SysButtonRegister(&g_powerButton, buttonHandle, getButtonStatus);
    SysButtonRegister(&g_gearButton, buttonHandle, getButtonStatus);
}

void ButtonPoll(void)
{
    SysButtonPoll();
}

