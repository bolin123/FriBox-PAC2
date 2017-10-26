#include "HalCommon.h"
#include "Sys.h"

static uint32_t g_sysTimeCount = 0;
static uint8_t g_blinkCount = 0;

void HardFault_Handler(void)
{
    uint8_t i;

    for(i = 0; i < 10; i++)
    {
        SysLog("");
    }
    while(1);
}

static void statusLedBlink(void)
{
    static uint8_t blinkCount = 0;
    static SysTime_t lastBlinkTime = 0;

    if(blinkCount < g_blinkCount)
    {
        if(SysTimeHasPast(lastBlinkTime, 200))
        {
            HalGPIOSetLevel(HAL_STATUS_LED_PIN, !HalGPIOGetLevel(HAL_STATUS_LED_PIN));
            lastBlinkTime = SysTime();
            blinkCount++;
        }
    }
    else
    {
        HalGPIOSetLevel(HAL_STATUS_LED_PIN, HAL_STATUS_LED_DISABLE_LEVEL);
        if(SysTimeHasPast(lastBlinkTime, 1500))
        {
            //lastBlinkTime = SysTime();
            blinkCount = 0;
        }
    }
}

static void statusLedInit(void)
{
    HalGPIOConfig(HAL_STATUS_LED_PIN, HAL_IO_OUTPUT);
    HalGPIOSetLevel(HAL_STATUS_LED_PIN, HAL_STATUS_LED_ENABLE_LEVEL);

    HalGPIOConfig(0x18, HAL_IO_OUTPUT);
    HalGPIOSetLevel(0x18, 0);
}

void HalCommonStatusLedSet(uint8_t blink)
{
    g_blinkCount = blink * 2;
}

void HalTimerPast1ms(void)
{
    g_sysTimeCount++;
}

uint32_t HalGetSysTimeCount(void)
{
    return g_sysTimeCount;
}

void HalInterruptSet(bool enable)
{
    if(enable)
    {
        __enable_irq();
    }
    else
    {
        __disable_irq();
    }
}

static void periphClockInit(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //wait
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
}

static void delay300ns(void)
{
    __ASM("NOP");
}

static void delay600ns(void)
{
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
}

static void delay900ns(void)
{
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    
}


static void ledBarTestPoll(void)
{
    uint8_t i;
		uint8_t level[8] = {0,1,0,1,0,1,0,1};
    static SysTime_t lastTime;

    if(SysTimeHasPast(lastTime, 1000))
    {
        for(i = 0; i < 8; i++)
        {
            //HalWait0P3Us(1);
            delay900ns();

            GPIO_WriteBit(GPIOB, GPIO_Pin_8, level[i]);
        }
        
        lastTime = SysTime();
    }
}

void HalCommonInitialize(void)
{
    SystemInit();
    periphClockInit();
    HalGPIOInitialize();
    HalFlashInitialize();
    HalUartInitialize();
    HalTimerInitialize();
    HalPwmInitialize();
    HalSPIInitialize();
    //HalLEDInitialize();
    statusLedInit();
    //HalLEDTest();
}

void HalCommonPoll(void)
{
    HalUartPoll();
    HalGPIOPoll();
    HalTimerPoll();
    HalSPIPoll();
    HalFlashPoll();
    //HalLEDPoll();
    statusLedBlink();
    ledBarTestPoll();
}

