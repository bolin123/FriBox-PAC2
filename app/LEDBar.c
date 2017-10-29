#include "LEDBar.h"

static LEDMode_t g_mode = LED_MODE_SHUTDOWN;
static LEDColor_t g_color;
static SysTime_t g_modeTime;

static bool g_startLight = false;
static uint8_t g_breathData[3] = {0x00, 0x00, 0x00};

const uint8_t HAL_LED_COLOR_NONE[3] = {0x00, 0x00, 0x00};
const uint8_t HAL_LED_COLOR_RED[3]   = {0x00, 0xff, 0x00};
const uint8_t HAL_LED_COLOR_GREEN[3] = {0xff, 0x00, 0x00};
const uint8_t HAL_LED_COLOR_BLUE[3]  = {0x00, 0x00, 0xff};

static uint8_t *getColorData(LEDColor_t color)
{
    uint8_t *data = (uint8_t *)HAL_LED_COLOR_NONE;
    switch(color)
    {
        case LED_COLOR_BLUE:
            data = (uint8_t *)HAL_LED_COLOR_BLUE;
            break;
        case LED_COLOR_RED:
            data = (uint8_t *)HAL_LED_COLOR_RED;
            break;
        case LED_COLOR_GREEN:
            data = (uint8_t *)HAL_LED_COLOR_GREEN;
            break;
        default:
            break;
    }
    return data;
}

static void allLEDColorSet(uint8_t *grb)
{
    uint8_t i;

    for(i = 0; i < SYS_LED_BAR_NUM; i++)
    {
        HalLEDColorSet((const uint8_t *)grb);
    }
    //GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
}

static void ledCycleHandle(void)
{
    uint8_t i;
    uint8_t *data;
    static uint8_t cycleCount = 0;
    
    if(g_mode == LED_MODE_LOOPL)
    {
        if(SysTimeHasPast(g_modeTime, 200))
        {
            for(i = 0; i < cycleCount; i++)
            {
                HalLEDColorSet(HAL_LED_COLOR_NONE);
            }
            HalLEDColorSet(getColorData(g_color));
            cycleCount++;
            if(cycleCount > SYS_LED_BAR_NUM)
            {
                cycleCount = 0;
            }
            g_modeTime = SysTime();
        }
    }
    else if(g_mode == LED_MODE_BREATH)
    {
    #if 0
        if(SysTimeHasPast(g_modeTime, 200))
        {
            if(g_startLight)
            {
                g_breathData[0] = g_breathData[0] * 2;
                g_breathData[1] = g_breathData[1] * 2;
                g_breathData[2] = g_breathData[2] * 2;
            }
            else
            {
                g_breathData[0] = g_breathData[0] / 2;
                g_breathData[1] = g_breathData[1] / 2;
                g_breathData[2] = g_breathData[2] / 2;
            }
            
            for(i = 0; i < SYS_LED_BAR_NUM; i++)
            {
                HalLEDColorSet(data);
            }
            g_modeTime = SysTime();
        }
    #endif
    }
}

void LEDBarModeSet(LEDMode_t mode, LEDColor_t color)
{
    allLEDColorSet(getColorData(color));
    g_mode = mode;
    g_color = color;
    g_modeTime = SysTime();
    
    if(LED_MODE_BREATH == mode)
    {
        g_startLight = false;
        memcpy(g_breathData, getColorData(color), sizeof(g_breathData));
    }
}

void LEDBarInitialize(void)
{
}

void LEDBarPoll(void)
{

    ledCycleHandle();
}

