#ifndef LED_BAR_H
#define LED_BAR_H

#include "Sys.h"

typedef enum
{
    LED_MODE_SHUTDOWN = 0, //关闭
    LED_MODE_BRIGHT,       //常亮
    LED_MODE_BREATH,       //呼吸
    LED_MODE_LOOPL,        //流水灯
}LEDMode_t;

typedef enum
{
    LED_COLOR_BLUE,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
}LEDColor_t;

void LEDBarModeSet(LEDMode_t mode, LEDColor_t color);
void LEDBarInitialize(void);
void LEDBarPoll(void);

#endif

