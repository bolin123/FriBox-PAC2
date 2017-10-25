#include "HalWait.h"
#include "stm32f10x_tim.h"

static void timerConfig0P3Us(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_Cmd(TIM2, DISABLE);
    TIM_TimeBaseStructure.TIM_Period = (24 - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = (1 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// Clear TIM14 update pending flag[??TIM14??????] 
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	// TIM14 enable counter 
	TIM_Cmd(TIM2, ENABLE);
}


void HalWait0P3Us(uint8_t count)
{
    uint8_t num = 0;
    timerConfig0P3Us();
    while(num < count)
	{
    	if((TIM2->SR)&TIM_IT_Update)
    	{
    		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    		num++;
    	}
	}
    TIM_Cmd(TIM2, DISABLE);
}

static void timerConfigUs(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_Cmd(TIM2, DISABLE);
    TIM_TimeBaseStructure.TIM_Period = (72 - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = (1 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// Clear TIM14 update pending flag[??TIM14??????] 
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	// TIM14 enable counter 
	TIM_Cmd(TIM2, ENABLE);
}
static void timerConfigMs(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    TIM_Cmd(TIM2, DISABLE);
	TIM_TimeBaseStructure.TIM_Period = (72 - 1);
	TIM_TimeBaseStructure.TIM_Prescaler = (1000 - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	// TIM2 enable counter 
	TIM_Cmd(TIM2, ENABLE);
}


void HalWaitMs(uint16_t ms)
{
    uint16_t count = 0;
    
    timerConfigMs();
	while(count < ms)
	{
    	if((TIM2->SR)&TIM_IT_Update)
    	{
    		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    		count++;
    	}
	}
    TIM_Cmd(TIM2, DISABLE);
}

void HalWaitUs(uint16_t us)
{
    uint16_t count = 0;
    
    timerConfigUs();
	while(count < us)
	{
    	if((TIM2->SR)&TIM_IT_Update)
    	{
    		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    		count++;
    	}
	}
    TIM_Cmd(TIM2, DISABLE);
}

void HalWaitInitialize(void)
{
    
}

void HalWaitPoll(void)
{
}

