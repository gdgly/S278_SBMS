//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : �����ֽ�6123
//* File Name          : Fault.c
//* Author             : SLX
//* Version            : V1.0.0
//* Start Date         : 2016.11.30
//* Description        : ���ļ� ��ϵͳ�ĸ��������ж�
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"
//#include "Fault.h" 
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
float errvalue[]=
{
    4.25,//1 cell voltage too high
    2.5,//1 cell voltage too low
    400.0,//1 Current too big
    75,//1 temperature too high
    12,//1 BMS CHG communication error
    
    4,//1 BMS internal communication fault
    4,//1 HV link fault
    4,//1 Heat component fault
    4,//1 HV Relay fault
    4,//1 BMS hardware fault
    
    408,//2 Total Voltage more higher
    253,//2 Total Voltage more lower
    4.2,//2 cell voltage more higher
    2.75,//2 cell voltage more lower
    320,//2 Current large charge or Feedback
    
    320,//2 Current large Discharge
    60, //2 Temperature more higher
    -25,//2 Temperature more lower
    20,//2 Temperature diff too large
    0.5,//2 Voltage diff too large
    
    40,//2 Rpn too low
    10,//2 BMS CHG communication fault
    2,//2 BMS internal Communicationfault
    2,//2 HV link fault
    2,//2 Heat component fault
    
    2,//2 HV Relay fault
    2,//2 BMS hardware fault
    377,//3 Total Voltage high
    285,//3 Total Voltage low
    4.1,//3 cell voltage high
    
    3.2,//3 cell voltage low
    180,//3 Current large charge or Feedback
    180,//3 Current large Discharge
    45, //3 Temperature more higher
    0,//3 Temperature more lower
    
    15,//3 Temperature diff too large
    0.1,//3 Voltage diff too large
    200,//3 Rpn low
    0.10//3 SOC low
};

//***********************************************************************************
//***********************************************************************************
unsigned char  g_caution_Flag_1 =0;//BMS����״̬ FOR ��λ��
unsigned char  g_caution_Flag_2 =0;
unsigned char  g_caution_Flag_3 =0;
unsigned char  g_caution_Flag_4 =0;
unsigned int g_errorCounter;
unsigned char ACCha_Flag_BST=0;
unsigned char ACCOverTime=0;//��������ͨ�Ź���
//unsigned char CarErrorLevel = 0;

CUTDISCURT0  CutDisCurt0;
CUTDISCURT50 CutDisCurt50;
CUTDISCURT70 CutDisCurt70;

CUTCHACURT0 CutChaCurt0;
CUTCHACURT50 CutChaCurt50;
CUTCHACURT70 CutChaCurt70;

CUTDCCHACURT0 CutDCChaCurt0;
CUTDCCHACURT50 CutDCChaCurt50;
CUTDCCHACURT70 CutDCChaCurt70;

CUTACCHACURT0 CutACChaCurt0;
CUTACCHACURT50 CutACChaCurt50;
CUTACCHACURT70 CutACChaCurt70;

unsigned char test1=0;//


//******************************************************************************
//* Function name:   ErrorLevelTask
//* Description:     ���ϵȼ��ж�
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void ErrorLevelTask(unsigned char Errorlevel)//
{
  //////////
    
}
//******************************************************************************
//* Function name:    ErrorToCarPC
//* Description:     ��ͬ���ϵĴ���ʽ
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void ErrorToCarPC(unsigned char Err1,unsigned char Err2,unsigned char Err3,unsigned char Err4,unsigned char *CarError,
                  unsigned char *PCErrorLevel1,unsigned char *PCErrorLevel2, unsigned char *PCErrorLevel3)//
{
    if(Err1==1)//���1������ 
    {
        *CarError=1;
        *PCErrorLevel1 |= 0x04;//to PC
    } 
    else//û��1������
    {           
        if(Err1==1)//����ж�������
        {
            *CarError=2;
            *PCErrorLevel2=1;//to PC
        } 
        else//���û�ж�������
        {
            *CarError=0;
            *PCErrorLevel2=0;//to PC                      
            if(Err1==1)//�������������
            {
                *CarError=3;
                *PCErrorLevel3=1;//to PC 
            } 
            else//��������޹���
            {
                *CarError=0;
                *PCErrorLevel3=0;//to PC 
                if(Err1==1)//4������
                    *CarError = 4;                
                else  //��4��
                    *CarError = 0;                              
            }
            
        }
        
    }
    
}
//******************************************************************************
//* Function name:    errorSystemVoltageOV
//* Description:      ��ذ��ܵ�ѹ��ѹ���� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorSystemVoltageOV(void) 
{
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
            
    Level=BatteryOverVoltage_step(g_highVoltageV1);
    ///////////////////�ϱ����ϵȼ���/////////////////////////   
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F0_Bat_Over_V = Level;//����CAN��ֵ   
    //1������
    Can554Byte2.Bit.F2_systemOV1=Error[1];//�ڲ�CAN��ֵ 
    CutChaCurt0.Bit.F0_Battery_Over_Voltage1=Error[1] ;
    CutDCChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[1] ;
    CutACChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[1]; 
    //2������
    Can554Byte0.Bit.F2_systemOV2=Error[2] ;
    CutChaCurt50.Bit.F0_Battery_Over_Voltage2=Error[2] ;
    CutDCChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[2] ;  
    CutACChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[2];
    
    //3������
    if(Error[3]==1)
        g_caution_Flag_1 |= 0x04;
    CutChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
    CutDCChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
    CutACChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
                
}
//******************************************************************************
//* Function name:    errorSystemVoltageUV
//* Description:      ��ذ��ܵ�ѹǷѹ���� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorSystemVoltageUV(void) //�ָ�,�ϱ�
{   
    unsigned char i;
    unsigned char Error[4]={0};
    unsigned char Level=0; 
    unsigned char LevelError=0;
    ///////////////////�ϱ����ϵȼ���/////////////////////////
    
    Level= BatteryUnderVoltage_step(g_highVoltageV1); 
    
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F2_Bat_Under_V = Level;//����CAN��ֵ
    
    //1�����ϴ���
    Can554Byte2.Bit.F0_systemUV1=Error[1]; //to PC
    CutDisCurt0.Bit.F0_Battery_Under_Voltage1=Error[1];
    if(Error[1])
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ�����
    
    //2������
    Can554Byte0.Bit.F0_systemUV2=Error[2];//to PC
    CutDisCurt50.Bit.F0_Battery_Under_Voltage2=Error[2];
    
    //3������ 
    if(Error[3] == 1)
        g_caution_Flag_1 |=0x01;
    CutDisCurt70.Bit.F0_Battery_Under_Voltage3=Error[3];
      
}
//******************************************************************************
//* Function name:    DCChangerComError
//* Description:      ��ֱ������ͨ�Ź��ϣ�������ʱ���ϣ�
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

void DCChangerComError(void)
{
    if(OverTimeState==1)
    {
        CutDCChaCurt0.Bit.F1_Communication_With_Charger=1;//���϶���
        g_caution_Flag_2 |=0x80; //for �ڲ�CAN
    }
    
}

//******************************************************************************
//* Function name:    innerCommOT3
//* Description:      �ڲ�ͨѶ����  ����
//* EntryParameter : None
//* ReturnValue    : Bool���� 0-�ޣ�1-��
//******************************************************************************
void innerCommOT3(void) 
{ 
    if((g_caution_Flag_3 & 0x01)!=0) //�ڲ�ͨѶ����
    {
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ�����
        CutDisCurt0.Bit.F1_Inner_Communiction=1;
        CutChaCurt0.Bit.F1_Inner_Communiction=1;
        CutDCChaCurt0.Bit.F2_Inner_Communiction=1;
        CutACChaCurt0.Bit.F2_Inner_Communiction=1;
        
        Error_Group3.Bit.F0_Sub_Com_Err=1;//����CAN��ֵ 
    } 

}
//******************************************************************************
//* Function name:    ACChangerComError
//* Description:      ���ܵ繭����ͨ�Ź��ϣ�������ʱ���ϣ�
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

void ACChangerComError(void)
{
    ACCOverTime++;
    if(ACCOverTime>=30)//30sû�н��յ���������
    {
        CutACChaCurt0.Bit.F1_Communication_With_Charger=1;
        g_caution_Flag_2 |=0x10; //for �ڲ�CAN
    }
    
}
//******************************************************************************
//* Function name:    errorDischargeOC
//* Description:      �ŵ��������  һ��
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorDischargeOC(void) //�ָ�;�ϱ������� 
{
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
    float curtValue=0;

    curtValue = BiggestDischargeCurt;
    
    ////////////////////////�ϱ����ϵȼ�////////////////////////
    Level=DisChargeOverCurrent_step(g_systemCurrent,curtValue);  
    for(i=1;i<4;i++) 
            if(i==Level) 
                Error[i]=1;
     
    Error_Group3.Bit.F6_DisChg_Over_I=Level;//����CAN��ֵ         
    //1�����ϴ���
    Can554Byte3.Bit.F0_DisChaOCurt1=Error[1];
    CutDisCurt0.Bit.F2_DisCharge_Over_Current1=Error[1];
    
    //2�����ϴ���
    Can554Byte1.Bit.F0_DisChaOCurt2=Error[2];//to PC
    CutDisCurt50.Bit.F1_DisCharge_Over_Current2=Error[2];
     
    //3�����ϴ���
    if(Error[3])
        g_caution_Flag_2 |= 0x01;//to PC
    CutDisCurt70.Bit.F1_DisCharge_Over_Current3=Error[3];
         
        
}
//******************************************************************************
//* Function name:    errorChargeOC
//* Description:      ���������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorChargeOC(void)
{

    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
    float curtValue=0;
    
    if(g_BmsModeFlag == DISCHARGING)//�ŵ�ģʽ
    {
        curtValue=BiggestFeedbackCurt; //��������Ϊ��ֵ
    }
    else if((g_BmsModeFlag == FASTRECHARGING)||(g_BmsModeFlag == RECHARGING)) //���ģʽ
    {
        curtValue=m_askcurrent;
    } 
    ///////////////////�ϱ����ϵȼ���/////////////////////////    
    Level=ChargeOverCurrent_step((-g_systemCurrent),curtValue);    
    for(i=1;i<4;i++) 
            if(i==Level) 
                Error[i]=1;         
    
    if(g_BmsModeFlag == DISCHARGING)
        Error_Group5.Bit.F0_FeedB_Over_I=Level;//����CAN��ֵ,˲ʱ����
    else
        Error_Group1.Bit.F4_Ch_Over_I = Level;//����CAN��ֵ,��繦��    
    
    //1�����ϴ���
    Can554Byte2.Bit.F7_ChangerOCurt1=Error[1];//to PC
    CutChaCurt0.Bit.F2_Charge_Over_Current1=Error[1];
    CutDCChaCurt0.Bit.F3_Charge_Over_Current1=Error[1];
    CutACChaCurt0.Bit.F3_Charge_Over_Current1=Error[1];
    
    //2�����ϴ���
    Can554Byte0.Bit.F7_ChangerOCurt2=Error[2];//to PC
    CutChaCurt50.Bit.F1_Charge_Over_Current2=Error[2];
    CutDCChaCurt50.Bit.F0_Charge_Over_Current2=Error[2];
    CutACChaCurt50.Bit.F0_Charge_Over_Current2=Error[2];
    
    //3�����ϴ���
    if(Error[3])
           g_caution_Flag_1 |=0x80;//to PC
    CutChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
    CutDCChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
    CutACChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
      
}

//******************************************************************************
//* Function name:    errorCellVoltageOV
//* Description:      �����ѹ����,1�����ϲ��ָ�,2,3,4�����Ͽɻָ�,ֱ������,��ֱ���ϱ�2������,���Ե�1Ҳ���Ե�3,���1��ֱ�Ӳ��ָ�  
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellVoltageOV(void)
{

    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
    //�ϱ����ϵȼ��ȼ��� 
    Level=cellOV_step(g_highestCellVoltage,g_BmsModeFlag);      
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
    
    Error_Group2.Bit.F0_Cell_Over_V=Level;//����CAN��ֵ
    //1������ 
    Can554Byte2.Bit.F3_cellOV1=Error[1];
    CutChaCurt0.Bit.F3_Cell_Over_Voltage1=Error[1];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[1];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[1];
    if((Error[1])&&(g_BmsModeFlag == DISCHARGING))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ�����
    
    //2������     
    Can554Byte0.Bit.F3_cellOV2=Error[2];//to PC
    CutChaCurt50.Bit.F2_Cell_Over_Voltage2=Error[2];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[2];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[2]; 
     
    
    //3������
    if(Error[3])
        g_caution_Flag_1 |= 0x08;//to PC
    CutChaCurt70.Bit.F2_Cell_Over_Voltage3=Error[3];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[3];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[3];  
          
}
//******************************************************************************
//* Function name:   errorCellVoltageUV
//* Description:     �����ѹǷѹ����  
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellVoltageUV(void)//�ϱ�������,��粻�ϱ�,�ָ�
{
    unsigned char i;
    unsigned char Error[4]={0};
    unsigned char Level=0; 
    unsigned char LevelError=0;

    Level=cellUN_step(g_lowestCellVoltage);
    for(i=1;i<4;i++)
      if(i==Level)
        Error[i]=1;
      
    Error_Group2.Bit.F2_Cell_Under_V=Level;//����CAN��ֵ
    //1�����ϴ���
    Can554Byte2.Bit.F1_cellUV1=Error[1];//to PC
    CutDisCurt0.Bit.F3_Cell_Under_Voltage1=Error[1];
    
    if(Error[1])
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ����� 
    //2�����ϴ���
    Can554Byte0.Bit.F1_cellUV2=Error[2];//to PC
    CutDisCurt50.Bit.F2_Cell_Under_Voltage2=Error[2];
     
    
    //3�����ϴ���
    if(Error[3])
        g_caution_Flag_1 |= 0x02;//to PC
    CutDisCurt70.Bit.F2_Cell_Under_Voltage3=Error[3];
     
         

}
//******************************************************************************
//* Function name:    errorCurrSensor
//* Description:      �������������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
/*
void errorCurrSensor(void)         ???????????
{
    static unsigned char Error1=0;
    static unsigned char counter_CurrSensor;
    if(Error1==1)
        return;
    Error1=CurrentSencerError_step(g_systemCurrent);
    if(Error1==1)
    {
        Fault_Group1.Bit.F_Current_Sensor=1;
        g_caution_Flag_3 |=0x02; //to PC    //�����ϱ�����
    }   
}
*/
Bool errorCurrSensorIniatial(void) //�ϵ�ǰ���2��
{
    unsigned char ii = 0;
    unsigned char jj = 0;
    for(ii=0;ii<=10;ii++)
    {

        if((g_systemCurrent>433)||(g_systemCurrent<-433))
        {   
            jj++;    
        } 
        if (jj>=5) 
        {

            g_caution_Flag_3 |=0x02; //to PC      //��������������
            //hardware_error2.Bit.F7_I_Ga_Err = 1;
            if(g_BmsModeFlag == DISCHARGING) 
            {
                CutDisCurt0.Bit.F4_I_Sener_Err = 1;
                CutChaCurt0.Bit.F4_I_Sener_Err = 1;
                status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ����� 
            }
            else if(g_BmsModeFlag == FASTRECHARGING)
                CutDCChaCurt0.Bit.F5_I_Sener_Err =1;
            else if(g_BmsModeFlag == RECHARGING)
                CutACChaCurt0.Bit.F5_I_Sener_Err =1;
               
            jj = 6;
        }

    }    
    return 0;  
    
}
//******************************************************************************
//* Function name:    errorCurrSensor
//* Description:      �¶ȴ��������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

//******************************************************************************
//* Function name:    errorCellUnbalance
//* Description:    �����ѹ��������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellUnbalance(void)//
{
 
  
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0;

    //�ϱ����ϵȼ�     
    Level=CellVolUnbalance_step(g_highestCellVoltage,g_lowestCellVoltage);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
    
    //2�����ϴ���
    Can554Byte1.Bit.F2_CellUnbalance2=Error[2];//to PC
    CutDisCurt50.Bit.F3_CellUnbalance2=Error[2];
    Error_Group5.Bit.F4_Cell_Dif_V=Level;//����CAN��ֵ  
}

//******************************************************************************
//* Function name:    errorTemUnbalance
//* Description:    �����¶Ȳ�������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorTemUnbalance(void)//
{
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level;
    float Tmp_H,Tmp_L;
    Tmp_H=(float)g_highestTemperature;
    Tmp_L=(float)g_lowestTemperature;
    
                           
    //�ϱ����ϵȼ�
    Level=CellTempUnbalance_step(Tmp_H,Tmp_L);
    for(i=1;i<4;i++)
        if(i==Level)
            Error[i]=1;
        
    Error_Group5.Bit.F2_Cell_Dif_T=Level;//����CAN��ֵ 
    
    //2�����ϴ���
    Can554Byte1.Bit.F3_tempUnbalance2=Error[2];//to PC
    CutDisCurt50.Bit.F4_tempUnbalance2=Error[2];
    CutChaCurt50.Bit.F3_tempUnbalance2=Error[2];
    CutDCChaCurt50.Bit.F1_tempUnbalance2=Error[2];
    CutACChaCurt50.Bit.F1_tempUnbalance2=Error[2];
    
    //3�����ϴ���
    if(Error[3])
        g_caution_Flag_2 |= 0x08;//to PC
    CutDisCurt70.Bit.F3_tempUnbalance3=Error[3];
    CutChaCurt70.Bit.F3_tempUnbalance3=Error[3];
    CutDCChaCurt70.Bit.F2_tempUnbalance3=Error[3];
    CutACChaCurt70.Bit.F2_tempUnbalance3=Error[3];
     
} 
//******************************************************************************
//* Function name:    errorCellTemperatureOT
//* Description:      �����¶ȹ��߹���  һ��
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellTemperatureOT(void)//
{
    unsigned char i;
    unsigned char Error[4]={0}; 
    unsigned char Level=0; 
    unsigned char LevelError=0;
    float t;
    //t= (float)g_highestTemperature;
    
    //�ϱ����ϵȼ�
    Level=OverTemp_step(g_highestTemperature);
    
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;    
    
    Error_Group2.Bit.F4_Temp_Over=Level;//����CAN��ֵ
    //1�����ϴ���
    Can554Byte2.Bit.F5_cellOT1=Error[1];
    CutDisCurt0.Bit.F5_Over_Temp1=Error[1];
    CutChaCurt0.Bit.F5_Over_Temp1=Error[1];
    CutDCChaCurt0.Bit.F6_Over_Temp1=Error[1];
    CutACChaCurt0.Bit.F6_Over_Temp1=Error[1];
    if((Error[1])&&(g_BmsModeFlag == DISCHARGING))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ����� 
    
    
    //2�����ϴ���
    Can554Byte0.Bit.F5_cellOT2=Error[2];//to PC
    CutDisCurt50.Bit.F5_Over_Temp2=Error[2];
    CutChaCurt50.Bit.F4_Over_Temp2=Error[2];
    CutDCChaCurt50.Bit.F2_Over_Temp2=Error[2];
    CutACChaCurt50.Bit.F2_Over_Temp2=Error[2];
    
    
    //3�����ϴ���
    if(Error[3])
        g_caution_Flag_1 |= 0x20;//to PC
    CutDisCurt70.Bit.F4_Over_Temp3=Error[3];
    CutChaCurt70.Bit.F4_Over_Temp3=Error[3];
    CutDCChaCurt70.Bit.F3_Over_Temp3=Error[3];
    CutACChaCurt70.Bit.F3_Over_Temp3=Error[3];
    
     
}
 
//******************************************************************************
//* Function name:    errorCellTemperatureUT
//* Description:      �����¶ȹ��͹��� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellTemperatureUT(void)//
{

    unsigned char i;
    unsigned char Error[4]={0}; 
    unsigned char Level=0;
    float t;
    //t= (float)g_lowestTemperature;
    
    //�ϱ����ϵȼ�
    Level=UnderTemp_step(g_lowestTemperature);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    
    Error_Group5.Bit.F6_Cell_Under_T=Level;//����CAN��ֵ
    //1�����ϴ���
    Can554Byte2.Bit.F6_cellUT1=Error[1];
    CutDisCurt0.Bit.F6_Under_Temp1=Error[1];
    CutChaCurt0.Bit.F6_Under_Temp1=Error[1];
    CutDCChaCurt0.Bit.F7_Under_Temp1=Error[1];
    CutACChaCurt0.Bit.F7_Under_Temp1=Error[1];
    if((Error[1])&&((g_BmsModeFlag == DISCHARGING)))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS��ѹ�µ����� 
    
    //2�����ϴ���
    Can554Byte0.Bit.F6_cellUT2=Error[2];
  
    
     //3�����ϴ���
    if(Error[3])
        g_caution_Flag_1 |= 0x40;//to PC

}
 




//******************************************************************************
//* Function name:    errorSOCLow
//* Description:      SOC�͹��� һ��
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorSOCLow(void)//
{

    unsigned char i;
    unsigned char Error[4]={0}; 
    unsigned char Level=0;
    float t;
   
    
    //�ϱ����ϵȼ�
    Level=SOC_Low_step(g_socValue);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F6_SOC_Low=Level;//����CAN��ֵ 
    //2�����ϴ���
    Can554Byte1.Bit.F1_SOCLow2=Error[2];
    //Error_Group4.Bit.F6_SOC_Low=Error[2];//����CAN��ֵ 
    
     //3�����ϴ���
    if(Error[3])
        g_caution_Flag_2 |= 0x02;//to PC
    //Error_Group4.Bit.F6_SOC_Low=Error[3];//����CAN��ֵ 

}




//******************************************************************************
//* Function name:    errorLowIsolation
//* Description:      ��Ե������͹��� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorLowIsolation(void) 
{
    unsigned char i;
    unsigned char Error[4]={0};
    unsigned char Level=0; 
    float LowInsolation;
    if(Rn_Vpn_Value<=Rp_Vpn_Value)
        LowInsolation = Rn_Vpn_Value;
    else
        LowInsolation = Rp_Vpn_Value;
    
    
    Level=Isolation_step(LowInsolation);
    for(i=1;i<4;i++)
      if(i==Level)
        Error[i]=1;
      
    Error_Group1.Bit.F6_Ins_Err=Level;//����CAN��ֵ 
    //1�����ϴ���
    Can554Byte2.Bit.F4_insulationLow1=Error[1];//to PC
    CutDCChaCurt0.Bit.F8_Low_Isolation1=Error[2];
    CutACChaCurt0.Bit.F8_Low_Isolation1=Error[2];
    
        
    //2�����ϴ���
    Can554Byte0.Bit.F4_insulationLow2=Error[2];//to PC
    CutDCChaCurt50.Bit.F3_Low_Isolation2=Error[2];
    CutACChaCurt50.Bit.F3_Low_Isolation2=Error[2];
     
   
    //3�����ϴ���
    if(Error[3])
        g_caution_Flag_1 |= 0x10;//to PC
    CutDCChaCurt70.Bit.F4_Low_Isolation3=Error[3];
    CutACChaCurt70.Bit.F4_Low_Isolation3=Error[3];
     
}

//******************************************************************************
//* Function name:    Charge_Check
//* Description:      ����������� 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void Charge_Check(void) 
{

    static unsigned char Check_Num=0;
    if(((g_BmsModeFlag == FASTRECHARGING)||(g_BmsModeFlag == RECHARGING))&&(g_systemCurrent > 2)) 
    {
        Check_Num++;
        if(Check_Num>=100) 
        {
            ACCha_Flag_BST=1;
            CutDCChaCurt0.Bit.F9_Charge_Count1=1;
            CutACChaCurt0.Bit.F9_Charge_Count1=1;
            Can554Byte3.Bit.F4_ChargeCurDirError = 1;
            if(Check_Num>=250) //���������100�Σ����㷨Ҫ������µ硣
                Check_Num=101;
        }
    }
}
 

//**********************************************************************
//* Function name:   CarFaultDone
//* Description:     �г������й��ϵĴ���
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void CarFaultDone()
{
    
    //////////////////////////////*ֱ��2.5s��ϸ�ѹ*/////////////////////////////
    

    if((Error_Group3.Bit.F6_DisChg_Over_I)||(Error_Group5.Bit.F0_FeedB_Over_I)
       ||(Error_Group2.Bit.F4_Temp_Over))//���������ŵ����.�¶ȹ��߹���
    {
        HighVolPowerOff=1;//2.5S��ѹ�µ��־λ
    }
    //////////////////////////////*�г��ķ��͸�ѹ�µ�����*/////////////////////////////
    else if((Error_Group4.Bit.F2_Bat_Under_V)||(Error_Group3.Bit.F0_Sub_Com_Err)//�ܵ�ѹǷѹ1��,�ڲ�ͨѶ����
            ||(Error_Group3.Bit.F6_DisChg_Over_I)||(Error_Group5.Bit.F0_FeedB_Over_I)//�ŵ������1������������1
            ||(Error_Group2.Bit.F0_Cell_Over_V)||(Error_Group2.Bit.F2_Cell_Under_V)//�����ѹ������Ƿѹ
            ||(Error_Group2.Bit.F4_Temp_Over)||(Error_Group5.Bit.F6_Cell_Under_T))//�¶ȸߡ��¶ȵ�        
    {      
        OffState=1;//�����µ�
    }
}
//******************************************************************************
//* Function name:   BMSProtect
//* Description:     BMS���ı���,����״̬�����ƣ�ǿ�ƶϸ�ѹ
//* EntryParameter : None
//* ReturnValue    : Bool���� 0-�ޣ�1-��
//******************************************************************************
void BMSProtect(void)//�з��գ����OffState�ж�ʧ��,�����ʵ�ʵ�ѹ�Ƚ�********** 
{
    static unsigned char LCDelaytime=0;
    static unsigned char HCDelaytime=0;
    static unsigned char HTDelaytime=0;
    static unsigned char HBaDelaytime=0;
    if(g_BmsModeFlag == DISCHARGING)
    {      
        if((g_lowestCellVoltage<=LOWEST_CELL_VOL  )&&(g_lowestCellVoltage!=0))
        {
            LCDelaytime++;
            if(LCDelaytime>20)
            {
                LCDelaytime=27;
                openNegRelay();
                delay(25000); //19ms
                TurnOff_INA2K();//���̵���
                
            }
        } 
        else
        {
            LCDelaytime=0;
        }
    }
    if((g_BmsModeFlag == RECHARGING)||(g_BmsModeFlag == FASTRECHARGING))
    {      
        if(g_highestCellVoltage>=HIGHEST_CELL_VOL )//10s
        {
            HCDelaytime++;
            if(HCDelaytime>30)
            {
                HCDelaytime=37;
                openNegRelay();
                delay(25000); //19ms
                TurnOff_INA2K();//���̵���
            }
        } 
        else
        {
            HCDelaytime=0;
        }
        if(g_highVoltageV1>=HIGHEST_BATT_VOL )//
        {
            HBaDelaytime++;
            if(HBaDelaytime>30)
            {
                HBaDelaytime=37;
                openNegRelay();
                delay(25000); //19ms
                TurnOff_INA2K();//���̵���
            }
        } 
        else
        {
            HBaDelaytime=0;
        }
        
    }
    if(g_highestTemperature>(HIGHEST_TEM+40) )//10s
    {
        HTDelaytime++;
        if(HTDelaytime>30)
        {
            HTDelaytime=37;
            openNegRelay();
            delay(25000); //19ms
            TurnOff_INA2K();//���̵���
        }
    } 
    else
    {
        HTDelaytime=0;
    }
  
}

//******************************************************************************
//* Function name:   FaultLevel
//* Description:     ���ϵȼ�����
//* EntryParameter : None
//* ReturnValue    : ���޹��ϣ�0-�ޣ�1-��
//******************************************************************************
void FaultLevel(void)
{
    if((Error_Group4.Bit.F0_Bat_Over_V)||(Error_Group4.Bit.F2_Bat_Under_V)||(g_caution_Flag_2 |=0x80)
    ||(Error_Group3.Bit.F0_Sub_Com_Err)||(g_caution_Flag_2 |=0x10)||(Error_Group3.Bit.F6_DisChg_Over_I)
    ||(Error_Group5.Bit.F0_FeedB_Over_I)||(Error_Group1.Bit.F4_Ch_Over_I)||(Error_Group2.Bit.F0_Cell_Over_V)
    ||(Error_Group2.Bit.F2_Cell_Under_V)||(g_caution_Flag_3 |=0x02)||(Error_Group2.Bit.F4_Temp_Over)
    ||(Error_Group5.Bit.F6_Cell_Under_T)||(Error_Group1.Bit.F6_Ins_Err)||(Can554Byte3.Bit.F4_ChargeCurDirError)) 
    {
        status_group3.Bit.Fault_Level = 1;//���ع��ϣ�������Ϊ0
    } 
    else if((Error_Group4.Bit.F0_Bat_Over_V == 2)||(Error_Group4.Bit.F2_Bat_Under_V == 2)||(Error_Group3.Bit.F6_DisChg_Over_I == 2)
    ||(Error_Group5.Bit.F0_FeedB_Over_I == 2)||(Error_Group1.Bit.F4_Ch_Over_I == 2)||(Error_Group2.Bit.F0_Cell_Over_V == 2)
    ||(Error_Group2.Bit.F2_Cell_Under_V == 2)||(Error_Group5.Bit.F4_Cell_Dif_V == 2)||(Error_Group5.Bit.F2_Cell_Dif_T == 2)
    ||(Error_Group2.Bit.F4_Temp_Over == 2)||(Error_Group5.Bit.F6_Cell_Under_T == 2)||(Error_Group4.Bit.F6_SOC_Low == 2)
    ||(Error_Group1.Bit.F6_Ins_Err == 2)) 
    {
        status_group3.Bit.Fault_Level = 2;//��΢����������50%
    }
    else if((Error_Group4.Bit.F0_Bat_Over_V == 3)||(Error_Group4.Bit.F2_Bat_Under_V == 3)||(Error_Group3.Bit.F6_DisChg_Over_I == 3)
    ||(Error_Group5.Bit.F0_FeedB_Over_I == 3)||(Error_Group1.Bit.F4_Ch_Over_I == 3)||(Error_Group2.Bit.F0_Cell_Over_V == 3)
    ||(Error_Group2.Bit.F2_Cell_Under_V == 3)||(Error_Group5.Bit.F2_Cell_Dif_T == 2)||(Error_Group2.Bit.F4_Temp_Over == 2)
    ||(Error_Group5.Bit.F6_Cell_Under_T == 2)||(Error_Group4.Bit.F6_SOC_Low == 2)||(Error_Group1.Bit.F6_Ins_Err == 2)) 
    {
        status_group3.Bit.Fault_Level = 1;//һ�㣬��������70%
    }    
}


//******************************************************************************
//* Function name:    FaultProcess
//* Description:      �����ֹ���: ������������Ϊ�����ع���
//* EntryParameter : None
//* ReturnValue    : ���޹��ϣ�0-�ޣ�1-��
//******************************************************************************
unsigned char TaskFaultProcess(void) 
{
    unsigned char lever = 0;
    
    unsigned char i,j,k=0;
    static unsigned char caution1=0;
    static unsigned char caution2=0;
    static unsigned char caution3=0;
    static unsigned char caution4=0;
   
    //FaultLevel();
    errorSystemVoltageOV();//�ܵ�ѹ��ѹ

    if((g_BmsModeFlag == DISCHARGING)&&(status_group3.Bit.St_N_Relay==1))
        errorSystemVoltageUV();//�ܵ�ѹǷѹ

    if((g_BmsModeFlag == FASTRECHARGING)||(g_BmsModeFlag == RECHARGING))
    {
        DCChangerComError();//ֱ�����ͨ�Ź���
        errorLowIsolation();//��Ե����
        Charge_Check();//�������������
    }
    innerCommOT3();//�ڲ�ͨ�Ź��� 
    if(stateCode==30) 
    {
        errorDischargeOC();//�ŵ����
    }
    else if(g_BmsModeFlag == RECHARGING) //����ģʽ
    {
        ACChangerComError();
    }
    
    if((stateCode==30)||(stateCode==110)||(stateCode==170))//
    {
        errorChargeOC();   //������    
    }
    errorCellVoltageOV(); //�����ѹ���� 
    if(g_BmsModeFlag == DISCHARGING) 
    {
        errorCellVoltageUV();//�����ѹ����
        errorSOCLow();//SOC��
    }
    errorCurrSensorIniatial(); //��������������
    errorCellUnbalance();//�����ѹ������
    errorTemUnbalance();//�����¶Ȳ�����
      
    errorCellTemperatureOT();//�����¶ȹ���
    errorCellTemperatureUT();//�����¶ȹ���
    
    BMSProtect();////////BMS���ϼ̵���
    //*****************************************************************************************
    //******************************************************************************************
    ////////////��������֮ǰ��ȣ�û�б仯ʱ�������д洢��ֻ��1���ӲŴ洢������и��£��Ž��д洢
    if((caution1^g_caution_Flag_1)||(caution2^g_caution_Flag_2)||(caution3^g_caution_Flag_3)||(caution4^g_caution_Flag_4))
        StoreSysVariable();//����SOCֵ�͹�����Ϣ
    caution1 =  g_caution_Flag_1;
    caution2 =  g_caution_Flag_2;
    caution3 =  g_caution_Flag_3;
    caution4 =  g_caution_Flag_4;
    //////////////////////////////////////////////////////////////////
    ////Caution_Flag_4��Bit 0����ѹĸ�����ӹ��ϣ�Bit 1����������ϣ�    
    ///////////////////  
    g_storageSysVariable[PARA_ERROR_LEVER] = lever;
    g_storageSysVariable[CAUTION_FLAG_1] = g_caution_Flag_1;	
    g_storageSysVariable[CAUTION_FLAG_2] = g_caution_Flag_2;	
    g_storageSysVariable[CAUTION_FLAG_3] = g_caution_Flag_3;	
    g_storageSysVariable[CAUTION_FLAG_4] = g_caution_Flag_4;	
  
	  return g_storageSysVariable[PARA_ERROR_LEVER];   
}
//********************************************************************************************
//***********************************the end*************************************************
//********************************************************************************************
//********************************************************************************************