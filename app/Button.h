#ifndef BUTTON_H
#define BUTTON_H

typedef enum
{
    BUTTON_EVENT_SPEED_PUSH = 0,
    BUTTON_EVENT_POWER_PUSH,
    BUTTON_EVENT_SPEED_LONG_PUSH,
    BUTTON_EVENT_POWER_LONG_PUSH,
}ButtonEvent_t;

typedef void (* ButtonEventHandle_t)(ButtonEvent_t event);

void ButtonEventRegiste(ButtonEventHandle_t handle);
void ButtonInitialize(void);
void ButtonPoll(void);
#endif

