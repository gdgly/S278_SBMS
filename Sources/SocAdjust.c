//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : SocAdjust.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2016.2.14
//* Description        : 该文件用于调整在不同情况下SOC值
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

#include "BMS20.h"

//******************************************************************************
//* Function name:   SocRechargeAdjust
//* Description:     充电时SOC修正
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void SocRechargeAdjust(void)//充电末端SOC修正子程序
{
    if(g_highestCellVoltage<3.50)
        return;
    
    if((g_highestCellVoltage >=3.65)&&(g_highestCellVoltage<=4.0))
    {
      
        g_socValue=1;
        g_leftEnergy = g_socValue*(g_realNominalCap*3600);	  
        g_energyOfUsed = 0;
        AdjustQ1Value();
    }
    
}

//******************************************************************************
//* Function name:   SocEndDischargeAdjust
//* Description:     放电末端SOC修正
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void SocEndDischargeAdjust(void)//放电末端SOC修正子程序
{
    //float x1;//存储当前温度下修正SOC的单体电压
    static unsigned int time10S=0;
    static unsigned int counter5S1=0;
    static unsigned int counter5S2=0;
        
    if((g_lowestTemperature>=(40+5))&&(g_lowestCellVoltage!=0))//Tmin<5
    {
        if((g_lowestCellVoltage<=2.6)&&(g_systemCurrent<=180))
        {
            counter5S1++;
            if(counter5S1>=700)   // 5S
            {
                counter5S1=707;
                Can_g_socValue_Start=0; //SOC修正为0
	              CanSocIntegral();
            }
        } 
        else if(g_lowestCellVoltage<=2.9)
        {   
            time10S++;
            counter5S1=0;
            if((time10S>=1400)&&(Can_g_socValue>0.1))   // 10s
            {
                time10S=1400;
                Can_g_socValue_Start=0.1;//SOC修正为10%
	              CanSocIntegral();
            }
        } 
        else
        {          
            time10S=0;
            counter5S1=0;
        }
    } 
    else if((g_lowestTemperature<(40+5))&&(g_lowestCellVoltage!=0)) //g_systemCurrent
    {
        if((g_lowestCellVoltage<=2.5)&&(g_systemCurrent<=180))
        {
            counter5S2++;
            if(counter5S2>=700)   //5S
            {
                counter5S2=707;
                Can_g_socValue_Start=0;
	              CanSocIntegral();
            }
        } 
        else
            counter5S2=0;     
    }
    /*if((g_lowestCellVoltage<=3.0)&&(Can_g_socValue>=0.02))
    {
        counter5S1++;
        if(counter5S1>=700)
        {   
            Can_g_socValue_Start =0.02;
            CanSocIntegral();
            counter5S1=0;
        }
    }
     */
        

   
}
//******************************************************************************
//* Function name:   SocProDischargeAdjust
//* Description:     放电过程中SOC修正
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void SocProDischargeAdjust(void)//放电过程中SOC修正子程序(只有在行车模式，并且)
{
    static unsigned int Time2min;          
    //float Cap1,Cap2;
   
    if(g_systemCurrent>3)
    {
      
        Time2min=0;
        return;
    } 
    else
    {
        Time2min++;
        if(Time2min>100)//2min
        {
            //Cap1=CapacityCalculate(g_lowestCellVoltage);
            //g_socValue=Cap1/g_realNominalCap;
            Time2min=0;
        }
    }
    
}
//******************************************************************************
//* Function name:   SocOCVAdjust
//* Description:     根据当前单体电压状况调整当前的SOC
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
//long timenow=0;
//long timeold=0;
void SocOCVAdjust(unsigned int delaytime)
{
   /* float capMin=0;
    float capMax=0;
    float socOcv =0;
    unsigned char timenow=0;
    unsigned char timeold=0;

   
    ////Read out the previous power-off time, compare it to the current time, 
    ////if the time is over 2 hours, then adjust the SOC 
	  
	  ReadOutErrorRecord(g_errorCounter);//为了得到上次下电时的时间
	  timenow=CurrentTime[4]*1440+CurrentTime[2]*60+CurrentTime[1];
	  timeold=g_storageSysVariableOut[SYS_REALTIME_DAY]*1440+g_storageSysVariableOut[SYS_REALTIME_HOUR]*60+g_storageSysVariableOut[SYS_REALTIME_MINUTE];
	  if(timenow<(timeold+delaytime)) //间隔时间没有超过间隔时间 
	  {
	      OCVState = 0;
	      return;
	  }
	  if((g_lowestCellVoltage==0)||(g_highestCellVoltage>=5.0))//防止单体电压为0或者最高单体检测故障
	      return;
    if((g_lowestCellVoltage>=3.3)&&(g_highestCellVoltage<4.20)&&(g_lowestTemperature>(-35+40))) //平均电压正确才修正SOC    
    {   
        capMin= CapacityCalculate(g_lowestCellVoltage);//算出最低单体电压对应的容量Q2
        capMax= CapacityCalculate(g_highestCellVoltage);//算出最高单体电压对应的容量
        g_realNominalCap= capMin + (g_originalCapacity-capMax);//算出实际可用的总容量Q1
        if((g_realNominalCap>=g_originalCapacity)||(g_realNominalCap<=0))
        {          
            g_realNominalCap=g_originalCapacity;
        }
        StoreQ1value();//存Q1:g_realNominalCap
        g_socValue = capMin/g_realNominalCap; //算出当前可用的SOC
        OCVState = 1;
    }
   */
    
        
    unsigned int timenow=0;
    unsigned int timeold=0;

    ReadOutErrorRecord(g_errorCounter);//为了得到上次下电时的时间
	  timenow=CurrentTime[4]*1440+CurrentTime[2]*60+CurrentTime[1];
	  timeold=g_storageSysVariableOut[SYS_REALTIME_DAY]*1440+g_storageSysVariableOut[SYS_REALTIME_HOUR]*60+g_storageSysVariableOut[SYS_REALTIME_MINUTE];
	  if(timenow<(timeold+delaytime)) //间隔时间没有超过间隔时间 30min
	  {
	      OCVState = 0;
	      return;
	  }
	  if((g_lowestCellVoltage==0)||(g_highestCellVoltage>=4.0))//防止单体电压为0或者最高单体检测故障
	      return;
	  
	  if(g_lowestCellVoltage>=3.34)//
	  {
	      if(g_socValue<0.95)
	      {	        
	          g_socValue=0.95;
	          g_leftEnergy = g_socValue*(g_realNominalCap*3600);	  
	          g_energyOfUsed = 0; 
	          OCVState = 1;
	      }
	  } 
	  else if(g_lowestCellVoltage<2.9)
	  {	    
	      g_socValue=0;
    	  g_leftEnergy = g_socValue*(g_realNominalCap*3600);	  
        g_energyOfUsed = 0;
        OCVState = 1;
	  }
	  else if(g_lowestCellVoltage<3.1)
	  {	    
	      g_socValue=0.05;
     	  g_leftEnergy = g_socValue*(g_realNominalCap*3600);	  
        g_energyOfUsed = 0;
        OCVState = 1;
	  }
    
}
//*******************************************************************************
//***********************the end**************************************************
//********************************************************************************
//********************************************************************************
//********************************************************************************




