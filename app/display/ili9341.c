#include "ili9341.h"
#include "HalGPIO.h"
#include "HalSPI.h"
#include "HalWait.h"
#include "Sys.h"
#include "zimo.h"

#define LCD_SIZE_WIDTH  240
#define LCD_SIZE_HEIGHT 320

#define LCD_LED_PIN    0x15 //pb5
#define LCD_RESET_PIN  0x32 //pd2
#define LCD_DC_PIN     0x2c //pc12
#define LCD_CS_PIN     0x1c //pb12

#define LCD_CHAR_WIDTH_1206  6
#define LCD_CHAR_HEIGHT_1206 12

#define LCD_CHAR_WIDTH_1608  8
#define LCD_CHAR_HEIGHT_1608 16

typedef struct
{
    uint16_t width;
    uint16_t heigh;
    uint16_t pixelNum;
    uint8_t *data;
}LCDDisplayInfo_t;

static void getDisplayInfo(LCDAsiicSize_t size, LCDDisplayInfo_t *info)
{
    if(info)
    {
        if(size == LCD_ASIIC_SIZE_16X08)
        {
            info->width = 8;
            info->heigh = 16;
            info->pixelNum = 16;
            info->data = (uint8_t *)asc2_1608;
        }
        else if(size == LCD_ASIIC_SIZE_48X24)
        {
            info->width = 24;
            info->heigh = 48;
            info->pixelNum = 144;
            info->data = (uint8_t *)asc2_4824;
        }
        else if(size == LCD_ASIIC_SIZE_97X48)
        {
            info->width = 48;
            info->heigh = 97;
            info->pixelNum = 582;
            info->data = (uint8_t *)asc2_9748;
        }
    }
}

//写寄存器
static void ili9341WriteReg(uint8_t regval)
{ 
    HalGPIOSetLevel(LCD_CS_PIN, 0);  //LCD_CS=0
    HalGPIOSetLevel(LCD_DC_PIN, 0);
    HalSPIReadWrite(HAL_SPI_PORT_2, regval);
    HalGPIOSetLevel(LCD_CS_PIN, 1);  //LCD_CS=1	   		 
}

//写数据16B
static void ili9341WriteData(uint16_t data)
{
 	HalGPIOSetLevel(LCD_CS_PIN, 0);  //LCD_CS=0
	HalGPIOSetLevel(LCD_DC_PIN, 1);	
	HalSPIReadWrite(HAL_SPI_PORT_2, (uint8_t)(data >> 8));
	HalSPIReadWrite(HAL_SPI_PORT_2, (uint8_t)data);
	//HalSPIDmaWrite((uint8_t *)&data, sizeof(uint16_t));
	HalGPIOSetLevel(LCD_CS_PIN, 1);  //LCD_CS=1		
}
#if 0
static void ili9341DmaWriteData(uint8_t *data, uint16_t len)
{
 	HalGPIOSetLevel(LCD_CS_PIN, 0);  //LCD_CS=0
	HalGPIOSetLevel(LCD_DC_PIN, 1);	

	HalSPIDmaWrite(data, len);
	HalGPIOSetLevel(LCD_CS_PIN, 1);  //LCD_CS=1		
}
#endif

//写数据8B
static void ili9341WriteDataByte(uint8_t data)
{
	HalGPIOSetLevel(LCD_CS_PIN, 0); //LCD_CS=0
	HalGPIOSetLevel(LCD_DC_PIN, 1);				    	   
	HalSPIReadWrite(HAL_SPI_PORT_2, data);	
	HalGPIOSetLevel(LCD_CS_PIN, 1);  //LCD_CS=1   			 
}	

//开始写GRAM
static void ili9341WriteRamPrepare(void)
{
    ili9341WriteReg(0X2C);  //写入GRAM
}

//LCD开启显示        ILI9341
void Ili9341LCDDisplayOn(void)
{                                           
    ili9341WriteReg(0X29); //26万色显示开启
}         
//LCD关闭显示        ILI9341
void Ili9341LCDDisplayOff(void)
{           
    ili9341WriteReg(0X28);//关闭显示 
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
static uint16_t bgr2rgb(uint16_t c)
{
        uint16_t  r,g,b,rgb;   
        b=(c>>0)&0x1f;
        g=(c>>5)&0x3f;
        r=(c>>11)&0x1f;         
        rgb=(b<<11)+(g<<5)+(r<<0);                 
        return(rgb);
}   

/*
lcddev.width=240;
lcddev.height=320;
lcddev.wramcmd=0X2C;
lcddev.setxcmd=0X2A;
lcddev.setycmd=0X2B; 

*/
//设置光标位置
static void ili9341SetCursor(uint16_t xpos, uint16_t ypos)
{
    ili9341WriteReg(0X2A);  //set x
	ili9341WriteDataByte(xpos>>8); 
	ili9341WriteDataByte(xpos&0XFF);
    //ili9341WriteDataByte((LCD_SIZE_WIDTH - 1) >> 8);
    //ili9341WriteDataByte(LCD_SIZE_WIDTH - 1);
	ili9341WriteReg(0X2B);  //set y
	ili9341WriteDataByte(ypos>>8); 
	ili9341WriteDataByte(ypos&0XFF);
    //ili9341WriteDataByte((LCD_SIZE_HEIGHT - 1) >> 8);
    //ili9341WriteDataByte((uint8_t)(LCD_SIZE_HEIGHT - 1));
} 	  

static void ili9341LCDOpenWindow( uint16_t x , uint16_t y , uint16_t width , uint16_t height )
{   
    //列地址控制设置，  
    ili9341WriteReg(0x2A); 
    //列设置指令     
    ili9341WriteDataByte(x>>8);
    //先高8位后低8位    
    ili9341WriteDataByte(x & 0xff);    
    ili9341WriteDataByte((x+width-1)>>8);  
    ili9341WriteDataByte((x+width-1) & 0xff);      
    //行地址控制设置，  
    ili9341WriteReg(0x2B); 
    //行设置指令 
    ili9341WriteDataByte(y>>8);
    //先高8位后低8位    
    ili9341WriteDataByte(y & 0xff);    
    ili9341WriteDataByte((y+height-1)>>8); 
    ili9341WriteDataByte((y+height-1) & 0xff);
}

#if 0
static uint8_t getCharFront(uint8_t id, uint8_t pos, uint8_t width)
{    
    if(width == 24)
    {
        return asc2_4824[id][pos];
    }
    else if(width == 8)
    {
        return asc2_1608[id][pos];
    }
    else
    {
        return 0;
    }
}


/****************************************************************************************
	*@brief		字符显示函数
	*@param		x,y,ASILL,color
	*@retvel	无
*****************************************************************************************/
static void ili9341LCDDisplaychar(uint16_t x, uint16_t y, 
                                    uint16_t ASILL, uint16_t chrwth, uint16_t chrhgt,
                                    uint16_t color)
{
	int8_t page,column,i,temp;
    int8_t n, cycle = 1;
    uint8_t realWth = chrwth;
	i = ASILL - ' ' ; 

    if(chrwth > 8)
    {
        cycle = (chrwth + 7) / 8;
        realWth = 8;
    }
	ili9341LCDOpenWindow( x , y , chrwth , chrhgt); 
	ili9341WriteRamPrepare(); //存储器写指令
	
	for(page = 0; page < chrhgt; page++)
	{
	    
        for(n = 0; n < cycle; n++)
        {
            temp = getCharFront(i, n + page * cycle, chrwth);//取字模   
            
            for(column = 0; column < realWth; column++)
    		{
    			if(temp & 0x80)
    				ili9341WriteData(color);
    			else
    				ili9341WriteData(LCD_COLOR_BLACK); //背景
    			temp<<=1 ; 
    		}//写完一行
        }
		
	}//写完一个字符	
}

/****************************************************************************************
	*@brief		字符串显示函数
	*@param		x,y,ASILL,color
	*@retvel	无
*****************************************************************************************/
void Ili9341LCDDisplayString(uint16_t x, uint16_t y, uint16_t cwidth, uint16_t cheigh, char *string, uint16_t color)
{

	while(*string != '\0')
	{
		if(x>(240 - cwidth))//检测是否到了一行的最末尾，若是切换到第二行显示
		{
			x = 0 ; 
			y += cheigh ; 
		}
		else if(y>(320 - cheigh))//检测是否到了最后一行，若是切换到始地址开始显示
		{
			x=0 ; 
			y=0; 
		}
		ili9341LCDDisplaychar(x, y, *string, cwidth, cheigh, color) ; 
		x += cwidth ;//显示完一个字符，列地址自动增加
		string++ ; //显示的字符自增
	}
}
#endif 

static void ili9341LCDDisplayByte(uint16_t x, uint16_t y, LCDDisplayInfo_t *info, char assiic, uint16_t color)
{
    uint16_t id = assiic - '0';
    uint16_t i, j;
    uint8_t *dataStart = &info->data[id * info->pixelNum];
    uint8_t tmp;
    
    ili9341LCDOpenWindow( x , y , info->width, info->heigh); 
	ili9341WriteRamPrepare(); //存储器写指令

    for(i = 0; i < info->pixelNum; i++)
    {
        tmp = dataStart[i];
        for(j = 0; j < 8; j++)
		{
			if(tmp & 0x80)
				ili9341WriteData(color);
			else
				ili9341WriteData(LCD_COLOR_BLACK); //背景
			tmp <<= 1 ; 
		}//写完一行
    }
}
#if 0
static void testDisplay(void)
{
    uint8_t n;
    uint16_t i, j;
    uint8_t tmp;

    for(n = 0; n < 3; n++)
    {
        ili9341LCDOpenWindow( 48*n, 0, 48, 97); 
    	ili9341WriteRamPrepare(); //存储器写指令

        for(i = 0; i < 582; i++)
        {
            tmp = testAsiic[n][i];
            for(j = 0; j < 8; j++)
    		{
    		    if(tmp & 0x80)
    				ili9341WriteData(LCD_COLOR_GREEN);
    			else
    				ili9341WriteData(LCD_COLOR_BLACK); //背景
    			tmp <<= 1 ; 
    		}
        }
    }
    
}
#endif

void Ili9341LCDAssiiDisplay(uint16_t x, uint16_t y, LCDAsiicSize_t size,  char *string, uint16_t color)
{
    LCDDisplayInfo_t info;
    
    getDisplayInfo(size, &info);
    
    while(*string != '\0')
	{
		if(x>(240 - info.width))//检测是否到了一行的最末尾，若是切换到第二行显示
		{
			x = 0 ; 
			y += info.heigh; 
		}
		else if(y>(320 - info.heigh))//检测是否到了最后一行，若是切换到始地址开始显示
		{
			x=0 ; 
			y=0; 
		}
		ili9341LCDDisplayByte(x, y, &info, *string, color); 
		x += info.width ;//显示完一个字符，列地址自动增加
		string++ ; //显示的字符自增
	}
}

#if 0
//画点
static void ili9341DrawPoint(uint16_t x, uint16_t y)
{
	ili9341SetCursor(x,y);		
	ili9341WriteReg(0X2C);  //写入GRAM
	ili9341WriteData(0x0000); //画笔颜色
} 

static void ili9341FastDrawPoint(uint16_t x, uint16_t y, uint16_t color)
{                    
        ili9341WriteReg(0x2A); 
        ili9341WriteData(x>>8); 
        ili9341WriteData(x&0XFF);         
        ili9341WriteReg(0x2B); 
        ili9341WriteData(y>>8); 
        ili9341WriteData(y&0XFF);
                 
        ili9341WriteReg(0X2C);//开始写入GRAM
        ili9341WriteData(color); 
}  


static void ili9341LCDFill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{          
        uint16_t i,j;
        uint16_t xlen = 0;        

        xlen = xend - xsta + 1;           
        for(i = ysta; i <= yend; i++)
        {
            ili9341SetCursor(xsta,i);      //设置光标位置 
            ili9341WriteRamPrepare();     //开始写入GRAM          
            for(j=0;j<xlen;j++)
                ili9341WriteData(color);        
        }
                                              
}  
#endif
//清屏
static void ili9341DisplayClear(uint16_t color)
{
	uint32_t i;     
    //static uint8_t disColor[2];
    //memset(disColor, 0xf8, sizeof(disColor));
    //disColor[0] = 0xf8;
    //disColor[1] = 0xf8;
    /*
    for(i = 0; i < 320; i++)
    {
        disColor[i] = 0x00f8;
    }
    */
    
	ili9341SetCursor(0x0000, 0x0000);	//éè??1a±ê???? 
	ili9341WriteReg(0X2C); 
    //ili9341DmaWriteData((uint8_t *)qqimage, sizeof(qqimage));
#if 0   

    for(i = 0; i < 240; i++)
    {
        ili9341DmaWriteData(disColor, sizeof(disColor));
    }
#endif

	for(i = 0; i < LCD_SIZE_WIDTH * LCD_SIZE_HEIGHT; i++)
	{
		//ili9341WriteData(color);
		ili9341WriteData(color);
	}
}
#if 0
void showimage(uint16_t x,uint16_t y) //??ê?40*40í???
{  
	uint16_t i,j,k;
	uint16_t da;
	k=0;
	for(i=0;i<40;i++)
	{	
		ili9341SetCursor(x,y+i);
        ili9341WriteReg(0X2C);  
		//LCD_WriteRAM_Prepare();     			//?aê?D′è?GRAM	
		for(j=0;j<40;j++)
		{
			da=qqimage[k*2+1];
			da<<=8;
			da|=qqimage[k*2]; 
			ili9341WriteData(da);					
			k++;  			
		}
	}
}


void showqq()
{ 
	uint16_t x,y; 
	x=0;
	y=75;
	while(y<LCD_SIZE_HEIGHT)
	{
		x=0;
		while(x<LCD_SIZE_WIDTH)
		{
			showimage(x,y);	
			x+=40;
		}
		y+=40;
	 }	  
}
#endif

//初始化寄存器
static void configILI9341(void)
{
#if 1
    ili9341WriteReg(0xCB);  
    ili9341WriteDataByte(0x39); 
    ili9341WriteDataByte(0x2C); 
    ili9341WriteDataByte(0x00); 
    ili9341WriteDataByte(0x34); 
    ili9341WriteDataByte(0x02); 

    ili9341WriteReg(0xCF);  
    ili9341WriteDataByte(0x00); 
    ili9341WriteDataByte(0XC1); 
    ili9341WriteDataByte(0X30); 

    ili9341WriteReg(0xE8);  
    ili9341WriteDataByte(0x85); 
    ili9341WriteDataByte(0x00); 
    ili9341WriteDataByte(0x78); 

    ili9341WriteReg(0xEA);  
    ili9341WriteDataByte(0x00); 
    ili9341WriteDataByte(0x00); 

    ili9341WriteReg(0xED);  
    ili9341WriteDataByte(0x64); 
    ili9341WriteDataByte(0x03); 
    ili9341WriteDataByte(0X12); 
    ili9341WriteDataByte(0X81); 

    ili9341WriteReg(0xF7);  
    ili9341WriteDataByte(0x20); 
//
    ili9341WriteReg(0xC0);    //Power control 
    ili9341WriteDataByte(0x23);   //VRH[5:0] 

    ili9341WriteReg(0xC1);    //Power control 
    ili9341WriteDataByte(0x10);   //SAP[2:0];BT[3:0] 

    ili9341WriteReg(0xC5);    //VCM control 
    ili9341WriteDataByte(0x3e); //??±è?èμ÷?ú
    ili9341WriteDataByte(0x28); 

    ili9341WriteReg(0xC7);    //VCM control2 
    ili9341WriteDataByte(0x86);  //--

    ili9341WriteReg(0x36);    // Memory Access Control 
    ili9341WriteDataByte(0x48); //C8	   //48 68êú?á//28 E8 oá?á

    ili9341WriteReg(0x3A);    
    ili9341WriteDataByte(0x55); 

    ili9341WriteReg(0xB1);    
    ili9341WriteDataByte(0x00);  
    ili9341WriteDataByte(0x18); 

    ili9341WriteReg(0xB6);    // Display Function Control 
    ili9341WriteDataByte(0x08); 
    ili9341WriteDataByte(0x82);
    ili9341WriteDataByte(0x27);  

    ili9341WriteReg(0xF2);    // 3Gamma Function Disable 
    ili9341WriteDataByte(0x00); 

    ili9341WriteReg(0x26);    //Gamma curve selected 
    ili9341WriteDataByte(0x01); 

    ili9341WriteReg(0xE0);    //Set Gamma 
    ili9341WriteDataByte(0x0F); 
    ili9341WriteDataByte(0x31); 
    ili9341WriteDataByte(0x2B); 
    ili9341WriteDataByte(0x0C); 
    ili9341WriteDataByte(0x0E); 
    ili9341WriteDataByte(0x08); 
    ili9341WriteDataByte(0x4E); 
    ili9341WriteDataByte(0xF1); 
    ili9341WriteDataByte(0x37); 
    ili9341WriteDataByte(0x07); 
    ili9341WriteDataByte(0x10); 
    ili9341WriteDataByte(0x03); 
    ili9341WriteDataByte(0x0E); 
    ili9341WriteDataByte(0x09); 
    ili9341WriteDataByte(0x00); 

    ili9341WriteReg(0XE1);    //Set Gamma 
    ili9341WriteDataByte(0x00); 
    ili9341WriteDataByte(0x0E); 
    ili9341WriteDataByte(0x14); 
    ili9341WriteDataByte(0x03); 
    ili9341WriteDataByte(0x11); 
    ili9341WriteDataByte(0x07); 
    ili9341WriteDataByte(0x31); 
    ili9341WriteDataByte(0xC1); 
    ili9341WriteDataByte(0x48); 
    ili9341WriteDataByte(0x08); 
    ili9341WriteDataByte(0x0F); 
    ili9341WriteDataByte(0x0C); 
    ili9341WriteDataByte(0x31); 
    ili9341WriteDataByte(0x36); 
    ili9341WriteDataByte(0x0F); 
#endif
    ili9341WriteReg(0x11);    //Exit Sleep 
    HalWaitMs(120);
			
    ili9341WriteReg(0x29);    //Display on 
    
	ili9341DisplayClear(LCD_COLOR_BLACK); //corlor
	//showqq();
}

static void restartILI9341(void)
{
    HalGPIOSetLevel(LCD_RESET_PIN, 0);
    //delay 50ms
    HalWaitMs(50);
    HalGPIOSetLevel(LCD_RESET_PIN, 1);
    //delay 120ms
    HalWaitMs(120);

    //ILI9341ReadID();
    configILI9341();
}

void ILI9341Initialize(void)
{
    HalGPIOConfig(LCD_LED_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(LCD_RESET_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(LCD_DC_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(LCD_CS_PIN, HAL_IO_OUTPUT);

    HalGPIOSetLevel(LCD_RESET_PIN, 1);
    HalGPIOSetLevel(LCD_LED_PIN, 1);
    HalGPIOSetLevel(LCD_DC_PIN, 0);
    HalGPIOSetLevel(LCD_CS_PIN, 0);
#if 1

    restartILI9341();
#endif
    HalGPIOSetLevel(LCD_LED_PIN, 0);

    //Ili9341LCDDisplayString(0, 0, 8, 16, "FriBoxPAC2", LCD_COLOR_GRAY);
    //Ili9341LCDDisplayString(0, 24, "56789", LCD_COLOR_BLACK, CHAR_SIZE_TYPE_1608);
    //Ili9341LCDAssiiDisplay(0, 0, LCD_ASIIC_SIZE_24X12, "FriBoxPAC2", LCD_COLOR_GRAY);

}

void ILI9341Poll(void)
{
}

