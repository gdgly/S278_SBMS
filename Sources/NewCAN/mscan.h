
struct can_msg 
{
    unsigned long id;
    Bool RTR;
    unsigned char data[8];
    unsigned char len;
    unsigned char prty;
};

extern unsigned long g_mboxID;
extern unsigned char g_mboxData[3][8];// CAN2接收到的报文数据，每个报文8个字节
extern unsigned char length;
extern unsigned char cammand;
extern unsigned long longAddr1;		// 地址
extern unsigned long longAddr2;		// 地址
extern unsigned char msgin[8];

extern void MSCAN0Init(void);
extern void MSCAN2Init(void);
extern void MSCAN0Rec(void);
extern void MSCAN2Rec(void);






