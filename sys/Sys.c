#include "Sys.h"
#include "SysTimer.h"
#include "Button.h"
#include "PMS7003.h"
#include "ir.h"
#include "Wifi.h"
#include "Motor.h"
#include "LEDBar.h"
#include "Display.h"
#include "APP.h"

static uint8_t g_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const char g_version[] = "1.0.0.4";

//redirect "printf()"
int fputc(int ch, FILE *f)
{
	HalUartWrite(SYS_UART_LOGS_PORT, (const uint8_t *)&ch, 1);
	return ch;
}

const char *SysGetVersion(void)
{
    return g_version;
}

const uint8_t *SysGetMacAddr(void)
{
    return (const uint8_t *)g_mac;
}

void SysSetMacAddr(const uint8_t *mac)
{
    memcpy(g_mac, mac, 6);
    
    SysLog("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void SysNetConfigStart(void)
{
    WifiNetConfigStart();
}

const char *SysGetServerUrl(void)
{
    return "Dev.FriBox.cn";
}

uint16_t SysGetServerPort(void)
{
    return 17380;
}

void SysStatusLedSet(uint8_t blink)
{
    HalCommonStatusLedSet(blink);
}

void SysInterruptSet(bool enable)
{
    static bool irqEnable = true;

    if(irqEnable != enable)
    {
        HalInterruptSet(enable);
    }
    
    irqEnable = enable;
}

static void sysLogOutputInit(void)
{
    HalUartConfig_t config;
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 0;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = NULL;
    HalUartConfig(SYS_UART_LOGS_PORT, &config);
}

void SysInitialize(void)
{
    HalCommonInitialize();
    sysLogOutputInit();
    SysTimerInitialize();
    ButtonInitialize();
    PMS7003Initailize();
    IRInitialize();
    DisplayInitialize();
    WifiInitialize();
    MotorInitialize();
    LEDBarInitialize();

    APPInitialize();
    SysLog("version %s", SysGetVersion());
    //LEDBarModeSet(LED_MODE_LOOPL, LED_COLOR_GREEN);
}

void SysPoll(void)
{
    HalCommonPoll();
    SysTimerPoll();
    ButtonPoll();
    PMS7003Poll();
    IRPoll();
    WifiPoll();
    MotorPoll();
    LEDBarPoll();
    DisplayPoll();
    APPPoll();
}

