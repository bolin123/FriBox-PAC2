#include "Display.h"
#include "ili9341.h"

#define DISPLAY_HEIGH_OFFSET 0
/*
Ili9341LCDAssiiDisplay(86, 52, LCD_ASIIC_SIZE_48X24, "W", LCD_COLOR_GRAY);
Ili9341LCDAssiiDisplay(130, 67, LCD_ASIIC_SIZE_16X08, "1234567", LCD_COLOR_GRAY);
Ili9341LCDAssiiDisplay(50, 111, LCD_ASIIC_SIZE_97X48, "231", LCD_COLOR_GREEN);
Ili9341LCDAssiiDisplay(96, 210, LCD_ASIIC_SIZE_48X24, "D3", LCD_COLOR_GRAY);
Ili9341LCDAssiiDisplay(108, 262, LCD_ASIIC_SIZE_16X08, "86p", LCD_COLOR_GRAY);
*/
void DisplayNetStatus(SysNetStatus_t status)
{
    if(status == SYS_NET_STATUS_CONNECT)
    {
        Ili9341LCDAssiiDisplay(86, 52 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_48X24, "W", LCD_COLOR_GRAY);
    }
    else if(status == SYS_NET_STATUS_NETCONFIG)
    {
        Ili9341LCDAssiiDisplay(86, 52 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_48X24, "C", LCD_COLOR_GRAY);
    }
    else if(status == SYS_NET_STATUS_DISCONNECT)
    {
        Ili9341LCDAssiiDisplay(86, 52 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_48X24, "N", LCD_COLOR_GRAY);
    }
}

void DisplayDeviceUID(char *uid)
{
    Ili9341LCDAssiiDisplay(130, 67 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_16X08, uid, LCD_COLOR_GRAY);
}

void DisplayPM2p5Value(uint16_t value)
{
    char text[4] = "";

    sprintf(text, "%03d", value);
    Ili9341LCDAssiiDisplay(50, 111 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_97X48, text, LCD_COLOR_GREEN);
}

void DisplayDateTime(SysDateTime_t *time)
{
}

void DisplaySpeedLevel(uint8_t level)
{
    char text[3] = "";
    sprintf(text, "D%d", level);
    Ili9341LCDAssiiDisplay(96, 210 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_48X24, text, LCD_COLOR_GRAY);
}

void DisplayConsumables(uint8_t per)
{
    char text[4] = "";
    sprintf(text, "%02d@", per);
    Ili9341LCDAssiiDisplay(108, 262 + DISPLAY_HEIGH_OFFSET, LCD_ASIIC_SIZE_16X08, text, LCD_COLOR_GRAY);
}

void DisplayInitialize(void)
{
    ILI9341Initialize();
    DisplayNetStatus(SYS_NET_STATUS_CONNECT);
    DisplayDeviceUID("0001023");
    DisplayPM2p5Value(0);
    DisplaySpeedLevel(0);
    DisplayConsumables(96);
}

void DisplayOn(void)
{
    Ili9341LCDDisplayOn();
}

void DisplayOff(void)
{
    Ili9341LCDDisplayOff();
}

void DisplayPoll(void)
{
}

