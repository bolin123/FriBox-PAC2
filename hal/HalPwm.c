#include "HalPwm.h"
#include "stm32f10x_tim.h"

void HalPwmPluseSet(uint8_t pwmNo, uint8_t chn, uint16_t pluse)
{
    if(pwmNo == HAL_PWM_NUM_1)
    {
        switch(chn)
        {
            case 0:
                TIM_SetCompare1(TIM3, pluse);
                break;
            case 1:
                TIM_SetCompare2(TIM3, pluse);
                break;
            case 2:
                TIM_SetCompare3(TIM3, pluse);
                break;
            case 3:
                TIM_SetCompare4(TIM3, pluse);
                break;
            default:
                break;
        }
    }
}

void HalPwmConfig(uint8_t pwmNo, uint16_t defPluse)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  

    if(pwmNo == HAL_PWM_NUM_1)
    {
        /* PWM信号电平跳变值 */  
        /*PCLK1经过2倍频后作为TIM3的时钟源等于72MHz*/  
        /* Time base configuration */           
        //TIM_DeInit(TIM3);
        TIM_TimeBaseStructure.TIM_Period = 100 - 1;  
        TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;                                    //设置预分频：预分频=2，即为72/3=24MHz  
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                                //设置时钟分频系数：不分频  
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //向上计数溢出模式  
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
        
        /* PWM1 Mode configuration: Channel1 */  
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //配置为PWM模式1  
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
        TIM_OCInitStructure.TIM_Pulse = defPluse;                                       //设置跳变值，当计数器计数到这个值时，电平发生跳变  
        TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_Low;                    //当定时器计数值小于CCR1时为高电平  
        TIM_OC1Init(TIM3, &TIM_OCInitStructure);                                    //使能通道1      
        //TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
        
        /* PWM1 Mode configuration: Channel2 */  
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
        TIM_OCInitStructure.TIM_Pulse = defPluse;                                       //设置通道2的电平跳变值，输出另外一个占空比的PWM  
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //当定时器计数值小于CCR2时为低电平 
        TIM_OC2Init(TIM3, &TIM_OCInitStructure);                                    //使能通道2  

        /* PWM1 Mode configuration: Channel3 */  
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
        TIM_OCInitStructure.TIM_Pulse = defPluse;                                       //设置通道2的电平跳变值，输出另外一个占空比的PWM  
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //当定时器计数值小于CCR2时为低电平 
        TIM_OC3Init(TIM3, &TIM_OCInitStructure); 

        /* PWM1 Mode configuration: Channel3 */  
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
        TIM_OCInitStructure.TIM_Pulse = defPluse;                                       //设置通道2的电平跳变值，输出另外一个占空比的PWM  
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //当定时器计数值小于CCR2时为低电平 
        TIM_OC4Init(TIM3, &TIM_OCInitStructure); 
        
        TIM_Cmd(TIM3, ENABLE); 
        TIM_CtrlPWMOutputs(TIM3, ENABLE);
    }
    
}

void HalPwmInitialize(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
    /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */ 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // 复用推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);     

    //ledPwmInit();
}

void HalPwmPoll(void)
{
}

