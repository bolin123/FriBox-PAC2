#include "HalLED.h"
#include "HalWait.h"
#include "HalGPIO.h"

#define HAL_LED_CTRL_PIN 0x18
#define HAL_LED_DECT_PIN 0x19

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

static void ledBit(bool set)
{
    if(set)
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
        delay600ns();//0.6us
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
        delay600ns();//0.6us
    }
    else
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
        delay300ns();//0.3u
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
        delay900ns();//0.9u
    }
}

static void ledDataSend(uint8_t rgb[3])
{
    uint8_t i, j;
    uint8_t data;

    for(i = 0; i < 3; i++)
    {
        data = rgb[i];
        for(j = 0; j < 8; j++)
        {
            ledBit((data << j) & 0x80);
        }
    }
}

void HalLEDTest(void)
{
    uint8_t rgb[3] = {230, 0, 18};
    ledDataSend(rgb);
    ledDataSend(rgb);
    ledDataSend(rgb);
    ledDataSend(rgb);
}

void HalLEDInitialize(void)
{
    HalGPIOConfig(HAL_LED_CTRL_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_LED_DECT_PIN, HAL_IO_INPUT);

    HalGPIOSetLevel(HAL_LED_CTRL_PIN, HAL_IO_LOW_LEVEL);
    HalWaitUs(100);//reset
}

void HalLEDPoll(void)
{
}

