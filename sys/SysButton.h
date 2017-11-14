#ifndef SYSBUTTON_H
#define SYSBUTTON_H
#include "Sys.h"

void SysButtonInitialize(void);
void SysButtonPoll(void);

//����״̬
typedef enum
{
    SYS_BUTTON_STATE_RELEASED, //δ����
    SYS_BUTTON_STATE_PRESSED,  //����
}SysButtonState_t;

struct SysButton_st;
typedef struct SysButton_st SysButton_t;

//����1 ��ʾ��ס�ظ���������
typedef uint8_t (*SysButtonHandler_t)(SysButton_t *button, SysTime_t pressTime, SysButtonState_t state);
typedef SysButtonState_t (*SysButtonGetState_t)(SysButton_t *button);

struct SysButton_st
{
    SysButtonState_t lastState;
    uint8_t handled;
    bool longPress;
    SysTime_t pressedTime;
    SysButtonHandler_t handler;
    SysButtonGetState_t getState;
};

//ע�ᰴ��
uint8_t SysButtonRegister(SysButton_t *button, SysButtonHandler_t handler, SysButtonGetState_t getState);


#endif // SYSBUTTON_H
