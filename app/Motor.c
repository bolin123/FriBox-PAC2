#include "Sys.h"
#include "Motor.h"

#define MOTOR_MAX_NUM   4
#define MOTOR_LEVEL_MAX 5

static const uint8_t g_pluse[MOTOR_MAX_NUM][MOTOR_LEVEL_MAX] = {
    {0, 20, 40, 60, 90}, //motor 1
    {0, 20, 40, 60, 90}, //motor 2
    {0, 20, 40, 60, 90}, //motor 3
    {0, 20, 40, 60, 90}, //motor 4
};

void MotorSpeedSet(uint8_t level)
{
    uint8_t i;

    if(level < MOTOR_LEVEL_MAX)
    {
        for(i = 0; i < MOTOR_MAX_NUM; i++)
        {
            HalPwmPluseSet(HAL_PWM_NUM_1, i, g_pluse[i][level]);
        }
    } 
}

void MotorInitialize(void)
{
    HalPwmConfig(HAL_PWM_NUM_1, 0);
}

void MotorPoll(void)
{
}

