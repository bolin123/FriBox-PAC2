#include "Sys.h"
#include "SysTimer.h"
#include "SysButton.h"

#define BUTTON_POWER_PIN 0x2a
#define BUTTON_SPEED_PIN 0x2b

#define BUTTON_LONG_PRESS_TIME  5000

static SysButton_t g_powerButton;
static SysButton_t g_gearButton;
static ButtonEventHandle_t g_buttonHandle = NULL;

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

static uint8_t buttonHandle(SysButton_t *button, SysTime_t pressTime, SysButtonState_t state)
{
    
    if(state == SYS_BUTTON_STATE_RELEASED)
    {
        if(pressTime > 20 && pressTime < 2000)
        {
            if(button == &g_powerButton)
            {
                g_buttonHandle(BUTTON_EVENT_POWER_PUSH);
            }
            else if(button == &g_gearButton)
            {
                g_buttonHandle(BUTTON_EVENT_SPEED_PUSH);
            }
            return 1;
        }
    }
    else
    {
        if(button == &g_powerButton)
        {
            if(pressTime > BUTTON_LONG_PRESS_TIME)
            {
                g_buttonHandle(BUTTON_EVENT_POWER_LONG_PUSH);
                return 1;
            }
        }
        else if(button == &g_gearButton)
        {
            if(pressTime > BUTTON_LONG_PRESS_TIME)
            {
                g_buttonHandle(BUTTON_EVENT_SPEED_LONG_PUSH);
                return 1;
            }
        }

    }
    return 0;
}

void ButtonEventRegiste(ButtonEventHandle_t handle)
{
    g_buttonHandle = handle;
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

