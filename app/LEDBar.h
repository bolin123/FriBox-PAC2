#ifndef LED_BAR_H
#define LED_BAR_H

#include "Sys.h"

typedef enum
{
    LED_MODE_SHUTDOWN = 0, //�ر�
    LED_MODE_BRIGHT,       //����
    LED_MODE_BREATH,       //����
    LED_MODE_LOOPL,        //��ˮ��
}LEDMode_t;

typedef enum
{
    LED_COLOR_GREEN = 0,
    LED_COLOR_RED,
    LED_COLOR_BLUE,
}LEDColor_t;

void LEDBarModeSet(LEDMode_t mode, LEDColor_t color);
void LEDBarInitialize(void);
void LEDBarPoll(void);

#endif

