//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : fotun_Logistics
//* File Name          : VehicleCAN.c
//* Author             : WsM
//* Version            : V1.0.0
//* Start Date         : 2016.7.8
//* Description        : ���ļ�������Ŀͨ��Э�鷢����صı��ĵ�������������
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"


ERROR_GROUP0 Error_Group0;
ERROR_GROUP1 Error_Group1;
ERROR_GROUP2 Error_Group2;
ERROR_GROUP3 Error_Group3;
ERROR_GROUP4 Error_Group4;
ERROR_GROUP5 Error_Group5;
ERROR_GROUP6 Error_Group6;
ERROR_GROUP7 Error_Group7;

unsigned int Charge_Times;     //������
unsigned long int chargeAH ;   //ʵ�ʳ���ܰ�ʱ
unsigned long int dischargeAH ;//ʵ�ʷŵ��ܰ�ʱ
float DC_ChargePower=0;//�����������������


/*futon Logistics use*/

float g_SOH=1;                                //SOH
                                             
float BiggestDischargeCurt = 0;              //���ŵ����
float BiggestFeedbackCurt  = 0;              //����������
float BiggestDisCurtContinuous = 0;          //�������ŵ����
float BiggestFeedbackCurtContinuous = 0;     //��������������

float BiggestDischargePower = 0;              //���ŵ繦��
float BiggestFeedbackPower  = 0;              //����������
float BiggestDisPowerContinuous = 0;          //�������ŵ繦��
float BiggestFeedbackContinuous = 0;          //��������������

unsigned int m_askpower = 0;                         //������
unsigned int chargeRemainderTime = 0;          //ʣ����ʱ��

unsigned char Enable_Charger_Output = 0;      //ʹ�ܳ������
unsigned char Enable_Charger_Sleep  = 0;      //ʹ�ܳ�������


unsigned char Box_Num_g_lowestCellVoltage;    //��͵����������
unsigned char Box_Num_g_highestCellVoltage;   //��ߵ����������
unsigned char Box_Num_g_lowestTemperature;    //����¶��������
unsigned char Box_Num_g_highestTemperature;   //����¶��������

unsigned char Box_Posi_g_lowestCellVoltage;   //��͵�����������λ��
unsigned char Box_Posi_g_highestCellVoltage;  //��ߵ�����������λ��
unsigned char Box_Posi_g_lowestTemperature;   //����¶���������λ��
unsigned char Box_Posi_g_highestTemperature;  //����¶���������λ��

float VehicleSpeed = 0;                         //�����ź�
                                                
VCU_REQUEST VCU_Request;
OBC_REQUEST OBC_Request;
VCU_CELLREQUEST VCU_CellRequest;
VCU_CONTROL VCU_Control;
VCU_PRELSTATE VCU_PRelState;
VCU_CHGCONTROL VCU_ChgControl;
VCU_PARKINGBRAKE VCU_ParkBrake;



STATUS_GROUP1 status_group1;
STATUS_GROUP2 status_group2;
STATUS_GROUP3 status_group3;
STATUS_GROUP4 status_group4;
/*futon Logistics use end*/


unsigned char FireWarning=0;//����Ԥ���ź�
unsigned char InsRelayState=0;//��Ե���Ƽ̵���״̬

//*************************************************************************************
//*************************************************************************************
//*****************************������BMS��VCU����Ϣ�ӳ���******************************
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//* Function name  : BMS_To_VCU_BasicMsg0
//* period         : 5ms
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//*************************************************************************************
void BMS_To_VCU_BasicMsg0(void)
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned int buff;

    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
   
    buff = (unsigned int)((g_systemCurrent+400)*50);	
    mg.data[0]= buff>>8;//����鵱ǰ���ȵ������ֽ� 
    mg.data[1]= (unsigned char)buff;//����鵱ǰ���ȵ������ֽ� 

    buff = (unsigned int)(g_highVoltageV1*50);
    mg.data[2] = buff>>8;//����鵱ǰ��ѹ���ֽ� V4
    mg.data[3] = (unsigned char)buff;//����鵱ǰ��ѹ���ֽ�

    buff = (unsigned int)(g_highVoltageV2*50);
    mg.data[4] = buff>>8;//����鵱ǰ��ѹ���ֽ� V5
    mg.data[5] = (unsigned char)buff;//����鵱ǰ��ѹ���ֽ�

    buff = (unsigned int)(g_highVoltageV3*50);
    mg.data[6] = buff>>8;//����鵱ǰ��ѹ���ֽ� V6
    mg.data[7] = (unsigned char)buff;//����鵱ǰ��ѹ���ֽ�
     
    mg.id= 0x000c0123;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//*************************************************************************************
//* Function name  : BMS_To_VCU_BasicMsg1
//* period         : 5ms
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//*************************************************************************************
void BMS_To_VCU_BasicMsg1(void)
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned int buff;

    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    mg.id= 0x000c0124;

    mg.data[0] = FireWarning; // bit0-1����Ԥ��
    mg.data[1] = InsRelayState;// ��Ե���Ƽ̵���״̬
    
    buff = (unsigned int)((g_systemCurrent_2+400)*50);
    mg.data[2]= (unsigned char)((buff >> 8) & 0x00ff);  //����鵱ǰ���ȵ������ֽ� 
    mg.data[3]= (unsigned char)((buff >> 0) & 0x00ff);  //����鵱ǰ���ȵ������ֽ� 
    mg.data[4] = 0xff;
    mg.data[5] = 0xff;
    mg.data[6] = 0xff;
    mg.data[7] = 0xff;

    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
