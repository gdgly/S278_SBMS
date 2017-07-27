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
//* Description        : 该文件根据项目通信协议发送相关的报文到整车或交流充电机
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

unsigned int Charge_Times;     //充电次数
unsigned long int chargeAH ;   //实际充电总安时
unsigned long int dischargeAH ;//实际放电总安时
float DC_ChargePower=0;//充电电量（输出电量）


/*futon Logistics use*/

float g_SOH=1;                                //SOH
                                             
float BiggestDischargeCurt = 0;              //最大放电电流
float BiggestFeedbackCurt  = 0;              //最大回馈电流
float BiggestDisCurtContinuous = 0;          //最大持续放电电流
float BiggestFeedbackCurtContinuous = 0;     //最大持续回馈电流

float BiggestDischargePower = 0;              //最大放电功率
float BiggestFeedbackPower  = 0;              //最大回馈功率
float BiggestDisPowerContinuous = 0;          //最大持续放电功率
float BiggestFeedbackContinuous = 0;          //最大持续回馈功率

unsigned int m_askpower = 0;                         //请求功率
unsigned int chargeRemainderTime = 0;          //剩余充电时间

unsigned char Enable_Charger_Output = 0;      //使能充电机输出
unsigned char Enable_Charger_Sleep  = 0;      //使能充电机休眠


unsigned char Box_Num_g_lowestCellVoltage;    //最低单体所在箱号
unsigned char Box_Num_g_highestCellVoltage;   //最高单体所在箱号
unsigned char Box_Num_g_lowestTemperature;    //最低温度所在箱号
unsigned char Box_Num_g_highestTemperature;   //最高温度所在箱号

unsigned char Box_Posi_g_lowestCellVoltage;   //最低单体所在箱内位置
unsigned char Box_Posi_g_highestCellVoltage;  //最高单体所在箱内位置
unsigned char Box_Posi_g_lowestTemperature;   //最低温度所在箱内位置
unsigned char Box_Posi_g_highestTemperature;  //最高温度所在箱内位置

float VehicleSpeed = 0;                         //车速信号
                                                
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


unsigned char FireWarning=0;//火灾预警信号
unsigned char InsRelayState=0;//绝缘控制继电器状态

//*************************************************************************************
//*************************************************************************************
//*****************************以下是BMS给VCU发消息子程序******************************
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
    mg.data[0]= buff>>8;//电池组当前加热电流高字节 
    mg.data[1]= (unsigned char)buff;//电池组当前加热电流低字节 

    buff = (unsigned int)(g_highVoltageV1*50);
    mg.data[2] = buff>>8;//电池组当前电压高字节 V4
    mg.data[3] = (unsigned char)buff;//电池组当前电压低字节

    buff = (unsigned int)(g_highVoltageV2*50);
    mg.data[4] = buff>>8;//电池组当前电压高字节 V5
    mg.data[5] = (unsigned char)buff;//电池组当前电压低字节

    buff = (unsigned int)(g_highVoltageV3*50);
    mg.data[6] = buff>>8;//电池组当前电压高字节 V6
    mg.data[7] = (unsigned char)buff;//电池组当前电压低字节
     
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

    mg.data[0] = FireWarning; // bit0-1火灾预警
    mg.data[1] = InsRelayState;// 绝缘控制继电器状态
    
    buff = (unsigned int)((g_systemCurrent_2+400)*50);	
    mg.data[2]= buff >> 8;//电池组当前加热电流高字节 
    mg.data[3]= (unsigned char)buff;//电池组当前加热电流低字节 

    mg.data[4] = 0xff;
    mg.data[5] = 0xff;
    mg.data[6] = 0xff;
    mg.data[7] = 0xff;

    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
