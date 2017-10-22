#include "Wifi.h"
#include "VTList.h"
#include "VTStaticQueue.h"
#include "SysTimer.h"

#define WIFI_RECV_BUFF_LEN  64

typedef enum
{
    CMD_END_FLAG_NONE = 0,
    CMD_END_FLAG_A,
    CMD_END_FLAG_OK,
    CMD_END_FLAG_AT,
}WifiCmdEndFlag_t;


/*
AT+ENTM �л�������ģʽ
AT+WSMAC ��ȡMAC
AT+WMODE=STA 
AT+NETP=TCP,CLIENT,server.com,12345
AT+TCPTO=60/0 ��ʱ
AT+SMTSL=air  airkiss

AT+SMTLK
*/
typedef enum
{
    CMD_ID_NONE = 0,
    CMD_ID_3PLUS,     //+++
    CMD_ID_ENTM,      //������ģʽ�л���͸��ģʽ
    
    CMD_ID_WMODE,     //����/��ѯ WIFI ����ģʽ��AP/STA/APSTA��
    CMD_ID_WAP,       //�������ƣ�SSID��: FriBoxPac2-0001
    CMD_ID_WAKEY,     //�������� (8-63λ): FriBoxPac2
    CMD_ID_LANN,      //����IP: 192.168.150.254
    
    //socket A
    CMD_ID_NETP,      //����/��ѯ����Э�����
    CMD_ID_TCPTO,     //����/��ѯ��ʱʱ��
    //socket B
    CMD_ID_SOCKB,
    CMD_ID_TCPTOB,

    CMD_ID_WEBU,      //ϵͳ�����û���: fribox , ����: fribox
    CMD_ID_SEARCH,    //�����˿ڣ�17381 �� 
    CMD_ID_ASWD,      //���������֣�FriBox

    CMD_ID_WRMID,     //����MID��FriBoxPac2-0001

    CMD_ID_NTPEN,     //��ntpʱ��ͬ��
    CMD_ID_NTPSER,    //����ʱ�������
    CMD_ID_NTPRF,     //30����ͬ��һ��
    CMD_ID_NTPTM,     //�鿴�Ƿ�ͬ���ɹ�
    
    CMD_ID_SMTSL,     //��������������ʽ
    CMD_ID_SMTLK,     //���� Simple Config ����
    
    CMD_ID_WSMAC,     //��ѯ STA �� MAC ��ַ����
    CMD_ID_WSLQ,      //��ѯ STA �������ź�ǿ��
    CMD_ID_WSCAN,     //���� AP
    CMD_ID_Z,         //����ģ��
    CMD_ID_RELD,      //�ָ���������(����)
}WifiCmdID_t;

typedef struct WifiCmdList_st
{
    uint8_t dlen;
    uint8_t sendCount;
    uint8_t retries;
    uint16_t intevalTime;
    WifiCmdID_t id;
    uint8_t *data;
    SysTime_t lastTime;
    VTLIST_ENTRY(struct WifiCmdList_st);
}WifiCmdList_t;

static WifiMode_t g_wifiWorkMode = WIFI_MODE_NONE;
static bool g_netConfigStart = false;
//static WifiMode_t g_wifiSetMode = WIFI_MODE_PASSTHROUGH;
static uint8_t g_buff[WIFI_RECV_BUFF_LEN];
static uint16_t g_buffCount = 0;
static WifiCmdEndFlag_t g_cmdWaitFlag = CMD_END_FLAG_NONE;
static WifiCmdList_t g_cmdListHead;
static WifiCmdID_t g_currentCmdID = CMD_ID_NONE;
static VTSQueueDef(uint8_t, g_queue, 512);

static void cmdSend(const char *cmd, uint8_t retries, uint16_t inteval, WifiCmdID_t id);

static void lowSend(uint8_t *data, uint16_t len)
{
    SysLog("%s", data);
    HalUartWrite(HAL_UART_PORT_3, data, len);
}

static uint8_t *getMacAddr(char *msg, uint8_t *mac)
{
    uint8_t i;
    char val[3] = {0};

    SysLog("%s", msg);
    for(i = 0; i < 6; i++)
    {
        memcpy(val, &msg[i*2], 2);
        mac[i] = (uint8_t)(uint32_t)strtol(val, NULL, 16);
    }
    return mac;
}

static void cmdNodeDel(WifiCmdID_t id)
{
    WifiCmdList_t *node;
    
    VTListForeach(&g_cmdListHead, node)
    {
        if(node->id == id)
        {
            VTListDel(node);
            if(node->data)
            {
                free(node->data);
            }
            free(node);
            break;
        }
    }
}

static void atCmdParse(const char *atcmd)
{
    bool delCmd = false;
    char *pos;
    uint8_t mac[6];
    char buff[64] = "";

    SysPrintf("%s\n", atcmd);
    
    switch(g_currentCmdID)
    {
        case CMD_ID_NONE:
            break;
        case CMD_ID_ENTM:
            if(strstr(atcmd, "+ok"))
            {
                delCmd = true;
                g_wifiWorkMode = WIFI_MODE_PASSTHROUGH;
                //event(passthrough);
            }
            break;
        case CMD_ID_WMODE:
            if(strstr(atcmd, "+ok"))
            {
                delCmd = true;
                sprintf(buff, "AT+WAP=11BGN,FriBoxPac2-%02x%02x,CH6\r", SysGetMacAddr()[4], SysGetMacAddr()[5]);
                cmdSend(buff, 3, 500, CMD_ID_WAP);
                //sprintf(buff, "AT+NETP=TCP,CLIENT,%d,%s\r", SysGetServerPort(), SysGetServerUrl());
                //cmdSend(buff, 3, 500, CMD_ID_NETP);
            }
            break;
        case CMD_ID_WAP:
            if(strstr(atcmd, "+ok"))
            {
                delCmd = true;
                cmdSend("AT+WAKEY=WPA2PSK,AES,FriBoxPac2\r", 3, 500, CMD_ID_WAKEY);
            }
            break;
        case CMD_ID_WAKEY:
            if(strstr(atcmd, "+ok"))
            {
                delCmd = true;
                cmdSend("AT+LANN=192.168.150.254,255.255.255.0\r", 3, 500, CMD_ID_LANN);
            }
            break;
        case CMD_ID_LANN:
            if(strstr(atcmd, "+ok"))
            {
                delCmd = true;
                cmdSend("AT+NETP=TCP,SERVER,17380,0.0.0.0\r", 3, 500, CMD_ID_NETP);
            }
            break;
        case CMD_ID_NETP:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+TCPTO=0\r", 3, 500, CMD_ID_TCPTO);
                delCmd = true;
            }
            break;
        case CMD_ID_TCPTO:
            if(strstr(atcmd, "+ok"))
            {
                sprintf(buff, "AT+SOCKB=TCP,%d,%s\r", SysGetServerPort(), SysGetServerUrl());
                cmdSend(buff, 3, 500, CMD_ID_SOCKB);
                //cmdSend("AT+SMTSL=air\r", 3, 500, CMD_ID_SMTSL);
                delCmd = true;
            }
            break;
        case CMD_ID_SOCKB:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+TCPTOB=0\r", 3, 500, CMD_ID_TCPTOB);
                delCmd = true;
            }
            break;
        case CMD_ID_TCPTOB:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+WEBU=fribox,fribox\r", 3, 500, CMD_ID_WEBU);
                delCmd = true;
            }
            break;
        case CMD_ID_WEBU:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+SEARCH=17381\r", 3, 500, CMD_ID_SEARCH);
                delCmd = true;
            }
            break;
        case CMD_ID_SEARCH:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+ASWD=FriBox\r", 3, 500, CMD_ID_ASWD);
                delCmd = true;
            }
            break;
        case CMD_ID_ASWD:
            if(strstr(atcmd, "+ok"))
            {
                sprintf(buff, "AT+WRMID=FriBoxPac2-%02x%02x\r", SysGetMacAddr()[4], SysGetMacAddr()[5]);
                cmdSend(buff, 3, 500, CMD_ID_WRMID);
                delCmd = true;
            }
            break;
        case CMD_ID_WRMID:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+NTPEN=on\r", 3, 500, CMD_ID_NTPEN);
                delCmd = true;
            }
            break; 
        case CMD_ID_NTPEN:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+NTPSER=cn.pool.ntp.org,8\r", 3, 500, CMD_ID_NTPSER);
                delCmd = true;
            }
            break;
        case CMD_ID_NTPSER:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+NTPRF=30\r", 3, 500, CMD_ID_NTPRF);
                delCmd = true;
            }
            break;
        case CMD_ID_NTPRF:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+SMTSL=air\r", 3, 500, CMD_ID_SMTSL);
                delCmd = true;
            }
            break;
        case CMD_ID_NTPTM:
            break;
        case CMD_ID_SMTSL:
            if(strstr(atcmd, "+ok"))
            {
                cmdSend("AT+SMTLK\r", 3, 500, CMD_ID_SMTLK);
                delCmd = true;
            }
            break;
        case CMD_ID_SMTLK:
            if(strstr(atcmd, "+ok"))
            {
                //event();//net config start...
                delCmd = true;
            }
            break;
        case CMD_ID_WSMAC:
            pos = strstr(atcmd, "+ok=");
            if(pos)
            {
                SysSetMacAddr(getMacAddr(strchr(pos, '=') + 1, mac));
                cmdSend("AT+WMODE=APSTA\r", 3, 500, CMD_ID_WMODE);
                delCmd = true;
            }
            break;
        case CMD_ID_WSLQ:
            break;
        case CMD_ID_WSCAN:
            break;
        case CMD_ID_RELD:
            break;
          
    }

    if(delCmd)
    {
        cmdNodeDel(g_currentCmdID);
        g_currentCmdID = CMD_ID_NONE;
    }
}

static void cmdSendTimeoutHandle(WifiCmdID_t id)
{
    SysLog("id = %d", id);
}

static void cmdListPoll(void)
{
    WifiCmdList_t *node = VTListFirst(&g_cmdListHead);
    
    if(node != NULL && SysTimeHasPast(node->lastTime, node->intevalTime))
    {
        if(node->sendCount >= node->retries)
        {
            VTListDel(node);
            if(node->data)
            {
                free(node->data);
            }
            free(node);
            cmdSendTimeoutHandle(node->id);
        }
        else
        {
            g_currentCmdID = node->id;
            lowSend(node->data, node->dlen);
            node->lastTime = SysTime();
            node->sendCount++;
        }
    }
}

static void cmdSend(const char *cmd, uint8_t retries, uint16_t inteval, WifiCmdID_t id)
{
    WifiCmdList_t *node;
    uint8_t len = strlen(cmd);

    if(retries)
    {
        node = (WifiCmdList_t *)malloc(sizeof(WifiCmdList_t));
        if(node)
        {
            node->data = (uint8_t *)malloc(len);
            if(node->data)
            {
                memcpy(node->data, cmd, len);
                node->dlen = len;
                node->retries = retries;
                node->sendCount = 0;
                node->intevalTime = inteval;
                node->lastTime = 0;
                node->id = id;
                VTListAdd(&g_cmdListHead, node);
            }
        }
    }
    else
    {
        lowSend((uint8_t *)cmd, len);
    }
}

static void cmdRecv(uint8_t byte)
{
    uint8_t chrA;
    
    g_buff[g_buffCount++] = byte;
    //SysPrintf("%c", byte);
    
    if(g_cmdWaitFlag == CMD_END_FLAG_A)
    {
        if(byte ==  'a')
        {
            chrA = 'a';
            lowSend(&chrA, 1);
            g_cmdWaitFlag = CMD_END_FLAG_OK;
        }
        g_buffCount = 0;
    }
    else if(g_cmdWaitFlag == CMD_END_FLAG_OK)
    {
        if(g_buffCount ==  3)
        {
            if(g_buff[0] == '+' && g_buff[1] == 'o' && g_buff[2] == 'k')
            {
                cmdNodeDel(CMD_ID_3PLUS);
                cmdSend("AT+WSMAC\r", 3, 500, CMD_ID_WSMAC);
                g_cmdWaitFlag = CMD_END_FLAG_AT;
                g_wifiWorkMode = WIFI_MODE_COMMAND;
            }
            g_buffCount = 0;
        }
    }
    else if(g_cmdWaitFlag == CMD_END_FLAG_AT)
    {
        if(byte == 0x0a)
        {
            g_buff[g_buffCount] = '\0';
            atCmdParse((const char *)g_buff);
            g_buffCount = 0;
        }
    }

    if(g_buffCount >= WIFI_RECV_BUFF_LEN)
    {
        g_buffCount = 0;
    }
}

static void recvDataPoll(void)
{
    uint8_t byte;
    
    if(!VTSQueueIsEmpty(g_queue))
    {
        HalInterruptSet(false);
        byte = VTSQueueFront(g_queue);
        VTSQueuePop(g_queue);
        HalInterruptSet(true);
        
        if(g_currentCmdID != CMD_ID_NONE)
        {
            cmdRecv(byte);
        }
        else
        {
            //ProtocolDataRecv(byte);
            SysPrintf("%c", byte);
        }
    }
}

static void wifiRecvByte(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_queue))
        {
            VTSQueuePush(g_queue, data[i]);
        }
    }
#if 0
    for(i = 0; i < len; i++)
    {
        if(g_currentCmdID != CMD_ID_NONE)
        {
            cmdRecv(data[i]);
        }
        else
        {
            //ProtocolDataRecv(byte);
            SysPrintf("%c", data[i]);
        }
    }
#endif
}

static void startCommandMode(void)
{
    char cmd[3] = "+++"; 
    cmdSend(cmd, 3, 2000, CMD_ID_3PLUS);
    g_cmdWaitFlag = CMD_END_FLAG_A;
}

static void stopCommandMode(void)
{
    char *cmd = "AT+ENTM\r";
    cmdSend(cmd, 3, 1000, CMD_ID_ENTM);
}

static void wifiChangeMode(WifiMode_t mode)
{
    if(g_wifiWorkMode != mode)
    {
        if(mode == WIFI_MODE_COMMAND)
        {
            startCommandMode();
        }
        else
        {
            stopCommandMode();
        }
    }
    //g_wifiSetMode = mode;
}

WifiMode_t WifiGetWorkMode(void)
{
    return g_wifiWorkMode;
}

void WifiNetConfigStop(void)
{
}

static void netConfigDone(void *args)
{
    SysLog("");
    g_netConfigStart = false;
}

void WifiNetConfigStart(void)
{
    if(!g_netConfigStart)
    {
        SysLog("");
        g_netConfigStart = true;
        startCommandMode();
        SysTimerSet(netConfigDone, 60000, 0, NULL);
    }
}

static void ioInit(void)
{
    HalGPIOConfig(HAL_WIFI_RESET_PIN, HAL_IO_OUTPUT); //reset
    HalGPIOConfig(HAL_WIFI_RELOAD_PIN, HAL_IO_OUTPUT); //reload
    HalGPIOConfig(HAL_WIFI_READY_PIN, HAL_IO_INPUT); //ready
    HalGPIOSetLevel(HAL_WIFI_RELOAD_PIN, HAL_IO_HIGH_LEVEL);
    HalGPIOSetLevel(HAL_WIFI_RESET_PIN, HAL_IO_HIGH_LEVEL);
}

static void uartInit(void)
{
    HalUartConfig_t config;
        
    config.baudrate = HAL_WIFI_DEFAULT_BAUDRATE;
    config.flowControl = 0;
    config.parity = 0;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = wifiRecvByte;
    HalUartConfig(HAL_UART_PORT_3, &config);
}

static void resetDone(void *args)
{
    SysLog("");
    HalGPIOSetLevel(HAL_WIFI_RESET_PIN, HAL_IO_HIGH_LEVEL);
}

static void startReset(void)
{
    SysLog("");
    g_wifiWorkMode = WIFI_MODE_NONE;
    HalGPIOSetLevel(HAL_WIFI_RESET_PIN, HAL_IO_LOW_LEVEL);
    SysTimerSet(resetDone, 600, 0, NULL);
}

static void moduleIsReady(void)
{
    if(g_wifiWorkMode == WIFI_MODE_NONE)
    {
        if(HalGPIOGetLevel(HAL_WIFI_READY_PIN) == HAL_IO_LOW_LEVEL)
        {
            g_wifiWorkMode = WIFI_MODE_PASSTHROUGH;
        }
    }
    
}

void WifiInitialize(void)
{
    VTListInit(&g_cmdListHead);
    ioInit();
    uartInit();
    startReset();
}

void WifiPoll(void)
{
    cmdListPoll();
    moduleIsReady();
    recvDataPoll();
}

