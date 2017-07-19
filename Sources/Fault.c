//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : 青年浦江6123
//* File Name          : Fault.c
//* Author             : SLX
//* Version            : V1.0.0
//* Start Date         : 2016.11.30
//* Description        : 该文件 对系统的各级故障判断
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
unsigned char  g_caution_Flag_1 =0;//BMS故障状态 FOR 上位机
unsigned char  g_caution_Flag_2 =0;
unsigned char  g_caution_Flag_3 =0;
unsigned char  g_caution_Flag_4 =0;
unsigned int g_errorCounter;
unsigned char ACCha_Flag_BST=0;
unsigned char ACCOverTime=0;//交流充电机通信故障
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
//* Description:     故障等级判断
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void ErrorLevelTask(unsigned char Errorlevel)//
{
  //////////
    
}
//******************************************************************************
//* Function name:    ErrorToCarPC
//* Description:     不同故障的处理方式
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void ErrorToCarPC(unsigned char Err1,unsigned char Err2,unsigned char Err3,unsigned char Err4,unsigned char *CarError,
                  unsigned char *PCErrorLevel1,unsigned char *PCErrorLevel2, unsigned char *PCErrorLevel3)//
{
    if(Err1==1)//如果1级故障 
    {
        *CarError=1;
        *PCErrorLevel1 |= 0x04;//to PC
    } 
    else//没有1级故障
    {           
        if(Err1==1)//如果有二级故障
        {
            *CarError=2;
            *PCErrorLevel2=1;//to PC
        } 
        else//如果没有二级故障
        {
            *CarError=0;
            *PCErrorLevel2=0;//to PC                      
            if(Err1==1)//如果有三级故障
            {
                *CarError=3;
                *PCErrorLevel3=1;//to PC 
            } 
            else//如果三级无故障
            {
                *CarError=0;
                *PCErrorLevel3=0;//to PC 
                if(Err1==1)//4级故障
                    *CarError = 4;                
                else  //无4级
                    *CarError = 0;                              
            }
            
        }
        
    }
    
}
//******************************************************************************
//* Function name:    errorSystemVoltageOV
//* Description:      电池包总电压过压故障 
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
    ///////////////////上报故障等级数/////////////////////////   
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F0_Bat_Over_V = Level;//整车CAN赋值   
    //1级处理
    Can554Byte2.Bit.F2_systemOV1=Error[1];//内部CAN赋值 
    CutChaCurt0.Bit.F0_Battery_Over_Voltage1=Error[1] ;
    CutDCChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[1] ;
    CutACChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[1]; 
    //2级处理
    Can554Byte0.Bit.F2_systemOV2=Error[2] ;
    CutChaCurt50.Bit.F0_Battery_Over_Voltage2=Error[2] ;
    CutDCChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[2] ;  
    CutACChaCurt0.Bit.F0_Battery_Over_Voltage21=Error[2];
    
    //3级处理
    if(Error[3]==1)
        g_caution_Flag_1 |= 0x04;
    CutChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
    CutDCChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
    CutACChaCurt70.Bit.F0_Battery_Over_Voltage3=Error[3];
                
}
//******************************************************************************
//* Function name:    errorSystemVoltageUV
//* Description:      电池包总电压欠压故障 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorSystemVoltageUV(void) //恢复,上报
{   
    unsigned char i;
    unsigned char Error[4]={0};
    unsigned char Level=0; 
    unsigned char LevelError=0;
    ///////////////////上报故障等级数/////////////////////////
    
    Level= BatteryUnderVoltage_step(g_highVoltageV1); 
    
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F2_Bat_Under_V = Level;//整车CAN赋值
    
    //1级故障处理
    Can554Byte2.Bit.F0_systemUV1=Error[1]; //to PC
    CutDisCurt0.Bit.F0_Battery_Under_Voltage1=Error[1];
    if(Error[1])
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求
    
    //2级处理
    Can554Byte0.Bit.F0_systemUV2=Error[2];//to PC
    CutDisCurt50.Bit.F0_Battery_Under_Voltage2=Error[2];
    
    //3级处理 
    if(Error[3] == 1)
        g_caution_Flag_1 |=0x01;
    CutDisCurt70.Bit.F0_Battery_Under_Voltage3=Error[3];
      
}
//******************************************************************************
//* Function name:    DCChangerComError
//* Description:      与直流充电机通信故障（包括超时故障）
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

void DCChangerComError(void)
{
    if(OverTimeState==1)
    {
        CutDCChaCurt0.Bit.F1_Communication_With_Charger=1;//故障动作
        g_caution_Flag_2 |=0x80; //for 内部CAN
    }
    
}

//******************************************************************************
//* Function name:    innerCommOT3
//* Description:      内部通讯故障  三级
//* EntryParameter : None
//* ReturnValue    : Bool类型 0-无；1-有
//******************************************************************************
void innerCommOT3(void) 
{ 
    if((g_caution_Flag_3 & 0x01)!=0) //内部通讯故障
    {
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求
        CutDisCurt0.Bit.F1_Inner_Communiction=1;
        CutChaCurt0.Bit.F1_Inner_Communiction=1;
        CutDCChaCurt0.Bit.F2_Inner_Communiction=1;
        CutACChaCurt0.Bit.F2_Inner_Communiction=1;
        
        Error_Group3.Bit.F0_Sub_Com_Err=1;//整车CAN赋值 
    } 

}
//******************************************************************************
//* Function name:    ACChangerComError
//* Description:      与受电弓充电机通信故障（包括超时故障）
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

void ACChangerComError(void)
{
    ACCOverTime++;
    if(ACCOverTime>=30)//30s没有接收到充电机报文
    {
        CutACChaCurt0.Bit.F1_Communication_With_Charger=1;
        g_caution_Flag_2 |=0x10; //for 内部CAN
    }
    
}
//******************************************************************************
//* Function name:    errorDischargeOC
//* Description:      放电过流故障  一级
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorDischargeOC(void) //恢复;上报不处理 
{
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
    float curtValue=0;

    curtValue = BiggestDischargeCurt;
    
    ////////////////////////上报故障等级////////////////////////
    Level=DisChargeOverCurrent_step(g_systemCurrent,curtValue);  
    for(i=1;i<4;i++) 
            if(i==Level) 
                Error[i]=1;
     
    Error_Group3.Bit.F6_DisChg_Over_I=Level;//整车CAN赋值         
    //1级故障处理
    Can554Byte3.Bit.F0_DisChaOCurt1=Error[1];
    CutDisCurt0.Bit.F2_DisCharge_Over_Current1=Error[1];
    
    //2级故障处理
    Can554Byte1.Bit.F0_DisChaOCurt2=Error[2];//to PC
    CutDisCurt50.Bit.F1_DisCharge_Over_Current2=Error[2];
     
    //3级故障处理
    if(Error[3])
        g_caution_Flag_2 |= 0x01;//to PC
    CutDisCurt70.Bit.F1_DisCharge_Over_Current3=Error[3];
         
        
}
//******************************************************************************
//* Function name:    errorChargeOC
//* Description:      充电过流故障 
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
    
    if(g_BmsModeFlag == DISCHARGING)//放电模式
    {
        curtValue=BiggestFeedbackCurt; //回馈电流为负值
    }
    else if((g_BmsModeFlag == FASTRECHARGING)||(g_BmsModeFlag == RECHARGING)) //快充模式
    {
        curtValue=m_askcurrent;
    } 
    ///////////////////上报故障等级数/////////////////////////    
    Level=ChargeOverCurrent_step((-g_systemCurrent),curtValue);    
    for(i=1;i<4;i++) 
            if(i==Level) 
                Error[i]=1;         
    
    if(g_BmsModeFlag == DISCHARGING)
        Error_Group5.Bit.F0_FeedB_Over_I=Level;//整车CAN赋值,瞬时功率
    else
        Error_Group1.Bit.F4_Ch_Over_I = Level;//整车CAN赋值,充电功率    
    
    //1级故障处理
    Can554Byte2.Bit.F7_ChangerOCurt1=Error[1];//to PC
    CutChaCurt0.Bit.F2_Charge_Over_Current1=Error[1];
    CutDCChaCurt0.Bit.F3_Charge_Over_Current1=Error[1];
    CutACChaCurt0.Bit.F3_Charge_Over_Current1=Error[1];
    
    //2级故障处理
    Can554Byte0.Bit.F7_ChangerOCurt2=Error[2];//to PC
    CutChaCurt50.Bit.F1_Charge_Over_Current2=Error[2];
    CutDCChaCurt50.Bit.F0_Charge_Over_Current2=Error[2];
    CutACChaCurt50.Bit.F0_Charge_Over_Current2=Error[2];
    
    //3级故障处理
    if(Error[3])
           g_caution_Flag_1 |=0x80;//to PC
    CutChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
    CutDCChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
    CutACChaCurt70.Bit.F1_Charge_Over_Current3=Error[3];
      
}

//******************************************************************************
//* Function name:    errorCellVoltageOV
//* Description:      单体过压故障,1级故障不恢复,2,3,4级故障可恢复,直流除外,若直流上报2级故障,可以到1也可以到3,如果1级直接不恢复  
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellVoltageOV(void)
{

    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0; 
    unsigned char LevelError=0;
    //上报故障等级等级数 
    Level=cellOV_step(g_highestCellVoltage,g_BmsModeFlag);      
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
    
    Error_Group2.Bit.F0_Cell_Over_V=Level;//整车CAN赋值
    //1级处理 
    Can554Byte2.Bit.F3_cellOV1=Error[1];
    CutChaCurt0.Bit.F3_Cell_Over_Voltage1=Error[1];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[1];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[1];
    if((Error[1])&&(g_BmsModeFlag == DISCHARGING))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求
    
    //2级处理     
    Can554Byte0.Bit.F3_cellOV2=Error[2];//to PC
    CutChaCurt50.Bit.F2_Cell_Over_Voltage2=Error[2];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[2];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[2]; 
     
    
    //3级处理
    if(Error[3])
        g_caution_Flag_1 |= 0x08;//to PC
    CutChaCurt70.Bit.F2_Cell_Over_Voltage3=Error[3];
    CutACChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[3];
    CutDCChaCurt0.Bit.F4_Cell_Over_Voltage321=Error[3];  
          
}
//******************************************************************************
//* Function name:   errorCellVoltageUV
//* Description:     单体电压欠压故障  
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellVoltageUV(void)//上报不处理,充电不上报,恢复
{
    unsigned char i;
    unsigned char Error[4]={0};
    unsigned char Level=0; 
    unsigned char LevelError=0;

    Level=cellUN_step(g_lowestCellVoltage);
    for(i=1;i<4;i++)
      if(i==Level)
        Error[i]=1;
      
    Error_Group2.Bit.F2_Cell_Under_V=Level;//整车CAN赋值
    //1级故障处理
    Can554Byte2.Bit.F1_cellUV1=Error[1];//to PC
    CutDisCurt0.Bit.F3_Cell_Under_Voltage1=Error[1];
    
    if(Error[1])
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求 
    //2级故障处理
    Can554Byte0.Bit.F1_cellUV2=Error[2];//to PC
    CutDisCurt50.Bit.F2_Cell_Under_Voltage2=Error[2];
     
    
    //3级故障处理
    if(Error[3])
        g_caution_Flag_1 |= 0x02;//to PC
    CutDisCurt70.Bit.F2_Cell_Under_Voltage3=Error[3];
     
         

}
//******************************************************************************
//* Function name:    errorCurrSensor
//* Description:      电流传感器故障 
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
        g_caution_Flag_3 |=0x02; //to PC    //仅需上报故障
    }   
}
*/
Bool errorCurrSensorIniatial(void) //上电前检测2次
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

            g_caution_Flag_3 |=0x02; //to PC      //电流传感器故障
            //hardware_error2.Bit.F7_I_Ga_Err = 1;
            if(g_BmsModeFlag == DISCHARGING) 
            {
                CutDisCurt0.Bit.F4_I_Sener_Err = 1;
                CutChaCurt0.Bit.F4_I_Sener_Err = 1;
                status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求 
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
//* Description:      温度传感器故障 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************

//******************************************************************************
//* Function name:    errorCellUnbalance
//* Description:    单体电压不均衡故障 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorCellUnbalance(void)//
{
 
  
    unsigned char Error[4]={0};
    unsigned char i;
    unsigned char Level=0;

    //上报故障等级     
    Level=CellVolUnbalance_step(g_highestCellVoltage,g_lowestCellVoltage);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
    
    //2级故障处理
    Can554Byte1.Bit.F2_CellUnbalance2=Error[2];//to PC
    CutDisCurt50.Bit.F3_CellUnbalance2=Error[2];
    Error_Group5.Bit.F4_Cell_Dif_V=Level;//整车CAN赋值  
}

//******************************************************************************
//* Function name:    errorTemUnbalance
//* Description:    单体温度不均衡故障 
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
    
                           
    //上报故障等级
    Level=CellTempUnbalance_step(Tmp_H,Tmp_L);
    for(i=1;i<4;i++)
        if(i==Level)
            Error[i]=1;
        
    Error_Group5.Bit.F2_Cell_Dif_T=Level;//整车CAN赋值 
    
    //2级故障处理
    Can554Byte1.Bit.F3_tempUnbalance2=Error[2];//to PC
    CutDisCurt50.Bit.F4_tempUnbalance2=Error[2];
    CutChaCurt50.Bit.F3_tempUnbalance2=Error[2];
    CutDCChaCurt50.Bit.F1_tempUnbalance2=Error[2];
    CutACChaCurt50.Bit.F1_tempUnbalance2=Error[2];
    
    //3级故障处理
    if(Error[3])
        g_caution_Flag_2 |= 0x08;//to PC
    CutDisCurt70.Bit.F3_tempUnbalance3=Error[3];
    CutChaCurt70.Bit.F3_tempUnbalance3=Error[3];
    CutDCChaCurt70.Bit.F2_tempUnbalance3=Error[3];
    CutACChaCurt70.Bit.F2_tempUnbalance3=Error[3];
     
} 
//******************************************************************************
//* Function name:    errorCellTemperatureOT
//* Description:      单体温度过高故障  一级
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
    
    //上报故障等级
    Level=OverTemp_step(g_highestTemperature);
    
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;    
    
    Error_Group2.Bit.F4_Temp_Over=Level;//整车CAN赋值
    //1级故障处理
    Can554Byte2.Bit.F5_cellOT1=Error[1];
    CutDisCurt0.Bit.F5_Over_Temp1=Error[1];
    CutChaCurt0.Bit.F5_Over_Temp1=Error[1];
    CutDCChaCurt0.Bit.F6_Over_Temp1=Error[1];
    CutACChaCurt0.Bit.F6_Over_Temp1=Error[1];
    if((Error[1])&&(g_BmsModeFlag == DISCHARGING))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求 
    
    
    //2级故障处理
    Can554Byte0.Bit.F5_cellOT2=Error[2];//to PC
    CutDisCurt50.Bit.F5_Over_Temp2=Error[2];
    CutChaCurt50.Bit.F4_Over_Temp2=Error[2];
    CutDCChaCurt50.Bit.F2_Over_Temp2=Error[2];
    CutACChaCurt50.Bit.F2_Over_Temp2=Error[2];
    
    
    //3级故障处理
    if(Error[3])
        g_caution_Flag_1 |= 0x20;//to PC
    CutDisCurt70.Bit.F4_Over_Temp3=Error[3];
    CutChaCurt70.Bit.F4_Over_Temp3=Error[3];
    CutDCChaCurt70.Bit.F3_Over_Temp3=Error[3];
    CutACChaCurt70.Bit.F3_Over_Temp3=Error[3];
    
     
}
 
//******************************************************************************
//* Function name:    errorCellTemperatureUT
//* Description:      单体温度过低故障 
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
    
    //上报故障等级
    Level=UnderTemp_step(g_lowestTemperature);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    
    Error_Group5.Bit.F6_Cell_Under_T=Level;//整车CAN赋值
    //1级故障处理
    Can554Byte2.Bit.F6_cellUT1=Error[1];
    CutDisCurt0.Bit.F6_Under_Temp1=Error[1];
    CutChaCurt0.Bit.F6_Under_Temp1=Error[1];
    CutDCChaCurt0.Bit.F7_Under_Temp1=Error[1];
    CutACChaCurt0.Bit.F7_Under_Temp1=Error[1];
    if((Error[1])&&((g_BmsModeFlag == DISCHARGING)))
        status_group2.Bit.BMS_PowerOff_Req = 2;//BMS高压下电请求 
    
    //2级故障处理
    Can554Byte0.Bit.F6_cellUT2=Error[2];
  
    
     //3级故障处理
    if(Error[3])
        g_caution_Flag_1 |= 0x40;//to PC

}
 




//******************************************************************************
//* Function name:    errorSOCLow
//* Description:      SOC低故障 一级
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void errorSOCLow(void)//
{

    unsigned char i;
    unsigned char Error[4]={0}; 
    unsigned char Level=0;
    float t;
   
    
    //上报故障等级
    Level=SOC_Low_step(g_socValue);
    for(i=1;i<4;i++) 
       if(i==Level) 
           Error[i]=1;
       
    Error_Group4.Bit.F6_SOC_Low=Level;//整车CAN赋值 
    //2级故障处理
    Can554Byte1.Bit.F1_SOCLow2=Error[2];
    //Error_Group4.Bit.F6_SOC_Low=Error[2];//整车CAN赋值 
    
     //3级故障处理
    if(Error[3])
        g_caution_Flag_2 |= 0x02;//to PC
    //Error_Group4.Bit.F6_SOC_Low=Error[3];//整车CAN赋值 

}




//******************************************************************************
//* Function name:    errorLowIsolation
//* Description:      绝缘电阻过低故障 
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
      
    Error_Group1.Bit.F6_Ins_Err=Level;//整车CAN赋值 
    //1级故障处理
    Can554Byte2.Bit.F4_insulationLow1=Error[1];//to PC
    CutDCChaCurt0.Bit.F8_Low_Isolation1=Error[2];
    CutACChaCurt0.Bit.F8_Low_Isolation1=Error[2];
    
        
    //2级故障处理
    Can554Byte0.Bit.F4_insulationLow2=Error[2];//to PC
    CutDCChaCurt50.Bit.F3_Low_Isolation2=Error[2];
    CutACChaCurt50.Bit.F3_Low_Isolation2=Error[2];
     
   
    //3级故障处理
    if(Error[3])
        g_caution_Flag_1 |= 0x10;//to PC
    CutDCChaCurt70.Bit.F4_Low_Isolation3=Error[3];
    CutACChaCurt70.Bit.F4_Low_Isolation3=Error[3];
     
}

//******************************************************************************
//* Function name:    Charge_Check
//* Description:      电流正负检测 
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
            if(Check_Num>=250) //如果发生了100次，该算法要求必须下电。
                Check_Num=101;
        }
    }
}
 

//**********************************************************************
//* Function name:   CarFaultDone
//* Description:     行车过程中故障的处理
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void CarFaultDone()
{
    
    //////////////////////////////*直接2.5s后断高压*/////////////////////////////
    

    if((Error_Group3.Bit.F6_DisChg_Over_I)||(Error_Group5.Bit.F0_FeedB_Over_I)
       ||(Error_Group2.Bit.F4_Temp_Over))//充电过流、放电过流.温度过高故障
    {
        HighVolPowerOff=1;//2.5S高压下电标志位
    }
    //////////////////////////////*行车的发送高压下电请求*/////////////////////////////
    else if((Error_Group4.Bit.F2_Bat_Under_V)||(Error_Group3.Bit.F0_Sub_Com_Err)//总电压欠压1级,内部通讯故障
            ||(Error_Group3.Bit.F6_DisChg_Over_I)||(Error_Group5.Bit.F0_FeedB_Over_I)//放电电流大1级，回馈过流1
            ||(Error_Group2.Bit.F0_Cell_Over_V)||(Error_Group2.Bit.F2_Cell_Under_V)//单体过压、单体欠压
            ||(Error_Group2.Bit.F4_Temp_Over)||(Error_Group5.Bit.F6_Cell_Under_T))//温度高。温度低        
    {      
        OffState=1;//请求下电
    }
}
//******************************************************************************
//* Function name:   BMSProtect
//* Description:     BMS最后的保护,不受状态机控制，强制断高压
//* EntryParameter : None
//* ReturnValue    : Bool类型 0-无；1-有
//******************************************************************************
void BMSProtect(void)//有风险，如果OffState判断失败,最好用实际电压比较********** 
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
                TurnOff_INA2K();//快充继电器
                
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
                TurnOff_INA2K();//快充继电器
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
                TurnOff_INA2K();//快充继电器
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
            TurnOff_INA2K();//快充继电器
        }
    } 
    else
    {
        HTDelaytime=0;
    }
  
}

//******************************************************************************
//* Function name:   FaultLevel
//* Description:     故障等级处理
//* EntryParameter : None
//* ReturnValue    : 有无故障：0-无；1-有
//******************************************************************************
void FaultLevel(void)
{
    if((Error_Group4.Bit.F0_Bat_Over_V)||(Error_Group4.Bit.F2_Bat_Under_V)||(g_caution_Flag_2 |=0x80)
    ||(Error_Group3.Bit.F0_Sub_Com_Err)||(g_caution_Flag_2 |=0x10)||(Error_Group3.Bit.F6_DisChg_Over_I)
    ||(Error_Group5.Bit.F0_FeedB_Over_I)||(Error_Group1.Bit.F4_Ch_Over_I)||(Error_Group2.Bit.F0_Cell_Over_V)
    ||(Error_Group2.Bit.F2_Cell_Under_V)||(g_caution_Flag_3 |=0x02)||(Error_Group2.Bit.F4_Temp_Over)
    ||(Error_Group5.Bit.F6_Cell_Under_T)||(Error_Group1.Bit.F6_Ins_Err)||(Can554Byte3.Bit.F4_ChargeCurDirError)) 
    {
        status_group3.Bit.Fault_Level = 1;//严重故障，降功率为0
    } 
    else if((Error_Group4.Bit.F0_Bat_Over_V == 2)||(Error_Group4.Bit.F2_Bat_Under_V == 2)||(Error_Group3.Bit.F6_DisChg_Over_I == 2)
    ||(Error_Group5.Bit.F0_FeedB_Over_I == 2)||(Error_Group1.Bit.F4_Ch_Over_I == 2)||(Error_Group2.Bit.F0_Cell_Over_V == 2)
    ||(Error_Group2.Bit.F2_Cell_Under_V == 2)||(Error_Group5.Bit.F4_Cell_Dif_V == 2)||(Error_Group5.Bit.F2_Cell_Dif_T == 2)
    ||(Error_Group2.Bit.F4_Temp_Over == 2)||(Error_Group5.Bit.F6_Cell_Under_T == 2)||(Error_Group4.Bit.F6_SOC_Low == 2)
    ||(Error_Group1.Bit.F6_Ins_Err == 2)) 
    {
        status_group3.Bit.Fault_Level = 2;//轻微，降功率至50%
    }
    else if((Error_Group4.Bit.F0_Bat_Over_V == 3)||(Error_Group4.Bit.F2_Bat_Under_V == 3)||(Error_Group3.Bit.F6_DisChg_Over_I == 3)
    ||(Error_Group5.Bit.F0_FeedB_Over_I == 3)||(Error_Group1.Bit.F4_Ch_Over_I == 3)||(Error_Group2.Bit.F0_Cell_Over_V == 3)
    ||(Error_Group2.Bit.F2_Cell_Under_V == 3)||(Error_Group5.Bit.F2_Cell_Dif_T == 2)||(Error_Group2.Bit.F4_Temp_Over == 2)
    ||(Error_Group5.Bit.F6_Cell_Under_T == 2)||(Error_Group4.Bit.F6_SOC_Low == 2)||(Error_Group1.Bit.F6_Ins_Err == 2)) 
    {
        status_group3.Bit.Fault_Level = 1;//一般，降功率至70%
    }    
}


//******************************************************************************
//* Function name:    FaultProcess
//* Description:      检测各种故障: 其中三级故障为最严重故障
//* EntryParameter : None
//* ReturnValue    : 有无故障：0-无；1-有
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
    errorSystemVoltageOV();//总电压过压

    if((g_BmsModeFlag == DISCHARGING)&&(status_group3.Bit.St_N_Relay==1))
        errorSystemVoltageUV();//总电压欠压

    if((g_BmsModeFlag == FASTRECHARGING)||(g_BmsModeFlag == RECHARGING))
    {
        DCChangerComError();//直流充电通信故障
        errorLowIsolation();//绝缘过低
        Charge_Check();//充电电流正负检测
    }
    innerCommOT3();//内部通信故障 
    if(stateCode==30) 
    {
        errorDischargeOC();//放电过流
    }
    else if(g_BmsModeFlag == RECHARGING) //慢充模式
    {
        ACChangerComError();
    }
    
    if((stateCode==30)||(stateCode==110)||(stateCode==170))//
    {
        errorChargeOC();   //充电过流    
    }
    errorCellVoltageOV(); //单体电压过高 
    if(g_BmsModeFlag == DISCHARGING) 
    {
        errorCellVoltageUV();//单体电压过低
        errorSOCLow();//SOC低
    }
    errorCurrSensorIniatial(); //电流传感器故障
    errorCellUnbalance();//单体电压不均衡
    errorTemUnbalance();//单体温度不均衡
      
    errorCellTemperatureOT();//单体温度过高
    errorCellTemperatureUT();//单体温度过低
    
    BMSProtect();////////BMS最后断继电器
    //*****************************************************************************************
    //******************************************************************************************
    ////////////当故障与之前相比，没有变化时，不进行存储，只有1分钟才存储；如果有更新，才进行存储
    if((caution1^g_caution_Flag_1)||(caution2^g_caution_Flag_2)||(caution3^g_caution_Flag_3)||(caution4^g_caution_Flag_4))
        StoreSysVariable();//保存SOC值和故障信息
    caution1 =  g_caution_Flag_1;
    caution2 =  g_caution_Flag_2;
    caution3 =  g_caution_Flag_3;
    caution4 =  g_caution_Flag_4;
    //////////////////////////////////////////////////////////////////
    ////Caution_Flag_4：Bit 0：高压母线连接故障；Bit 1：烟雾检测故障；    
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