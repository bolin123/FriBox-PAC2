#ifndef HAL_LED_H
#define HAL_LED_H

#include "HalCtype.h"

void HalLEDColorSet(const uint8_t grb[3]);
void HalLEDInitialize(void);
void HalLEDPoll(void);

#endif
