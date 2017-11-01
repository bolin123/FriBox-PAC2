#ifndef ILI9341_H
#define ILI9341_H

#include "HalCtype.h"

#define LCD_COLOR_WHITE         	 0xFFFF
#define LCD_COLOR_BLACK         	 0x0000	  
#define LCD_COLOR_BLUE         	 0x001F  
#define LCD_COLOR_BRED             0XF81F
#define LCD_COLOR_GRED 			 0XFFE0
#define LCD_COLOR_GBLUE			 0X07FF
#define LCD_COLOR_RED           	 0xF800
#define LCD_COLOR_MAGENTA       	 0xF81F
#define LCD_COLOR_GREEN         	 0x07E0
#define LCD_COLOR_CYAN          	 0x7FFF
#define LCD_COLOR_YELLOW        	 0xFFE0
#define LCD_COLOR_BROWN 			 0XBC40 //?��?��|?
#define LCD_COLOR_BRRED 			 0XFC07 //?��?o��?��|?
#define LCD_COLOR_GRAY  			 0X8430 //?���㡧|?

typedef enum
{
    LCD_ASIIC_SIZE_16X08,
    LCD_ASIIC_SIZE_48X24,
    LCD_ASIIC_SIZE_97X48,
}LCDAsiicSize_t;

typedef enum
{
    CHAR_SIZE_TYPE_1206 = 0,
    CHAR_SIZE_TYPE_1608,
    CHAR_SIZE_TYPE_4824,
}IliCharSizeType_t;

void Ili9341LCDDisplayOff(void);
void Ili9341LCDDisplayOn(void);
void Ili9341LCDAssiiDisplay(uint16_t x, uint16_t y, LCDAsiicSize_t size,  char *string, uint16_t color);
//void Ili9341LCDDisplayString(uint16_t x, uint16_t y, uint16_t cwidth, uint16_t cheigh, char *string, uint16_t color);
void ILI9341Initialize(void);
void ILI9341Poll(void);

#endif

