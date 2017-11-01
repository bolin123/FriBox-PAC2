#ifndef DISPLAY_H
#define DISPLAY_H

#include "Sys.h"

void DisplayNetStatus(SysNetStatus_t status);
void DisplayDeviceUID(char *uid);
void DisplayPM2p5Value(uint16_t value);
void DisplayDateTime(SysDateTime_t *time);
void DisplaySpeedLevel(uint8_t level);
void DisplayConsumables(uint8_t per);

void DisplayOn(void);
void DisplayOff(void);
void DisplayInitialize(void);
void DisplayPoll(void);

#endif

