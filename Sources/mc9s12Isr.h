//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : 恩驰
//* File Name          : mc9s12Isr.h
//* Author             : Kxy
//* Version            : V1.0.0
//* Start Date         : 2016,07,12
//* Description        : 该文件作为系统头文件，对该项目的系统参数进行定义和对所有全局变量进行全局声明
//----------------------------------------------------------------------------------------------------

#define KC 0.024  //小电流调整系数
#define KX 0.054  //大电流调整系数
#define ADCLC (1)  //adc偏移量
#define ADCLX (1)

extern float ac[12];
extern float ax[12];
extern struct can_msg *msg;
extern unsigned char CHMStep;

extern unsigned char Int_Flag;  //0x01:AD中断 0x02:can0中断 

extern unsigned char g_errorRecordRead_flag;//故障信息读取标志

extern unsigned char TestState;//测试状态机标志位


extern long Priority;
extern unsigned char DC_Start;//接收到0xaa后,开始闭合负极继电器

extern unsigned char BootState ;//boot程序

extern unsigned char TurnOnChangerRelay;//闭合充电继电器

extern unsigned char stateCode; //状态机编码
 
extern unsigned long g_mboxID;
extern unsigned char g_mboxData[1][8];// CAN2接收到的报文数据，每个报文8个字节

extern unsigned int DC_ChargeTime;//累计充电时间

extern unsigned char FlagBRMSend;
extern unsigned char BRMStep; //BRM发送步骤


extern unsigned char FlagBCPSend;
extern unsigned char BCPStep;

extern unsigned char FlagBCSSend;
extern unsigned char BCSStep;


extern unsigned char FireMessage[10]; //接收10个箱体的信息
//extern unsigned char InsRelayControl; //接收BMS板绝缘控制信息
//extern unsigned char Heat_P_Realy_Status;

typedef struct{
    byte InsRelayControl        :1;
    byte Heat_P_Realy_Status    :1;
    byte Kchg_N_ctrl_cmd        :1;
    byte reserved               :5;
}BMS_SBMS_CTRL_CMD;

extern BMS_SBMS_CTRL_CMD g_bms_sbms_ctrl_cmd;


typedef struct{
    byte work_status    :4;//byte0
    byte rsvd_0         :4;
    
    byte smoke_warning  :1;//byte1
    byte fire_warning   :1;
    byte rsvd_1         :6;
    
    byte OT_warning     :1;
    byte T_jump_warning :1;
    byte rsvd_2         :6;
    
    byte Temperature;
    byte rsvd_4;
    byte rsvd_5;
    byte rsvd_6;
    byte lifeCycle;
}s_FireMsg;

typedef union{
    s_FireMsg bits;
    byte data[8];
}FireMsg;

extern FireMsg g_FireMsg[10];
//**************************************************************
//*************************************************************
//***************************************************************
//******************************************************************
