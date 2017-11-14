#include "LEDBar.h"
#include "math.h"

static LEDMode_t g_mode = LED_MODE_SHUTDOWN;
static LEDColor_t g_color;
static SysTime_t g_modeTime;

static bool g_breathOut = false;
static int8_t g_cycleCount = 0;

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
    uint8_t data[3] = {0, 0, 0};
    
    if(g_mode == LED_MODE_LOOPL)
    {
        if(SysTimeHasPast(g_modeTime, 200))
        {
            for(i = 0; i < g_cycleCount; i++)
            {
                HalLEDColorSet(HAL_LED_COLOR_NONE);
            }
            HalLEDColorSet(getColorData(g_color));
            
            g_cycleCount++;
            if(g_cycleCount > SYS_LED_BAR_NUM)
            {
                g_cycleCount = 0;
            }
            g_modeTime = SysTime();
        }
    }
    else if(g_mode == LED_MODE_BREATH)
    {
        if(SysTimeHasPast(g_modeTime, 200))
        {
            data[g_color] = pow(2, g_cycleCount) - 1;
            allLEDColorSet(data);
            
            if(g_breathOut)//½¥ÁÁ
            {
                g_cycleCount++;
                if(g_cycleCount > 8)
                {
                    g_cycleCount = 8;
                    g_breathOut = false;
                }
            }
            else
            {
                g_cycleCount--;
                if(g_cycleCount < 0)
                {
                    g_cycleCount = 0;
                    g_breathOut = true;
                }
            }
            
            g_modeTime = SysTime();
        }
    }
}

void LEDBarModeSet(LEDMode_t mode, LEDColor_t color)
{
    if(mode == LED_MODE_SHUTDOWN)
    {
        allLEDColorSet((uint8_t *)HAL_LED_COLOR_NONE);
    }
    else
    {
        allLEDColorSet(getColorData(color));
        g_color = color;
        g_modeTime = SysTime();
        g_cycleCount = 0;
        g_breathOut = true;
    }
    
    g_mode = mode;
    
}

void LEDBarInitialize(void)
{
    LEDBarModeSet(LED_MODE_BRIGHT, LED_COLOR_BLUE);
}

void LEDBarPoll(void)
{

    ledCycleHandle();
}

