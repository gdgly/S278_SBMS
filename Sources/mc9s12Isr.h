//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : ����
//* File Name          : mc9s12Isr.h
//* Author             : Kxy
//* Version            : V1.0.0
//* Start Date         : 2016,07,12
//* Description        : ���ļ���Ϊϵͳͷ�ļ����Ը���Ŀ��ϵͳ�������ж���Ͷ�����ȫ�ֱ�������ȫ������
//----------------------------------------------------------------------------------------------------

#define KC 0.024  //С��������ϵ��
#define KX 0.054  //���������ϵ��
#define ADCLC (1)  //adcƫ����
#define ADCLX (1)

extern float ac[12];
extern float ax[12];
extern struct can_msg *msg;
extern unsigned char CHMStep;

extern unsigned char Int_Flag;  //0x01:AD�ж� 0x02:can0�ж� 

extern unsigned char g_errorRecordRead_flag;//������Ϣ��ȡ��־

extern unsigned char TestState;//����״̬����־λ


extern long Priority;
extern unsigned char DC_Start;//���յ�0xaa��,��ʼ�պϸ����̵���

extern unsigned char BootState ;//boot����

extern unsigned char TurnOnChangerRelay;//�պϳ��̵���

extern unsigned char stateCode; //״̬������
 
extern unsigned long g_mboxID;
extern unsigned char g_mboxData[1][8];// CAN2���յ��ı������ݣ�ÿ������8���ֽ�

extern unsigned int DC_ChargeTime;//�ۼƳ��ʱ��

extern unsigned char FlagBRMSend;
extern unsigned char BRMStep; //BRM���Ͳ���


extern unsigned char FlagBCPSend;
extern unsigned char BCPStep;

extern unsigned char FlagBCSSend;
extern unsigned char BCSStep;


extern unsigned char FireMessage[10]; //����10���������Ϣ
//extern unsigned char InsRelayControl; //����BMS���Ե������Ϣ
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
