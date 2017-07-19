//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Heat.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2016.2.14
//* Description        : 该文件用于加热管理
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include  "BMS20.h"
unsigned char st_heating;//动力电池加热状态：0未加热；1预加热中；2边充电边加热中
float HeatCurt;
//***********************************************************************
//* Function name:   HeatManage
//* Description:     状态机为100，110，170的时候进行热管理
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void HeatManage(void)
{
    static unsigned int tt=0;
    
    if(Tavg==0)
    {
        st_heating==0;  
        return;
    }
    
    if(g_highestCellVoltage>HIGHEST_ALLOWED_CHARGE_CV)
        return;
    //************未加热阶段,温度区间是大于等于10度st_heating都为0
    if(st_heating==0)
    {
        //if(Tavg<40)//0+40,小于0度，进入预加热阶段
        if(Tavg<(START_ALLOWED_PreHEAT_CHARGE_TEM+40))//小于-15度，进入预加热阶段
        {
            st_heating=1;
            HeatCurt = HIGHEST_ALLOWED_HEAT_CHARGE_C;        
   
        } 
        else if(Tavg<=(START_ALLOWED_HEAT_CHARGE_TEM+40))//5，进入边加热边充电阶段
        {
            st_heating=2;
            HeatCurt=HIGHEST_ALLOWED_HEAT_CHARGE_C;  
        }
        
    }
    //*************在预加热阶段，只加热，请求电流只请求加热电流，不请求充电电流
    if(st_heating==1)
    {
        if(Tavg>(STOP_ALLOWED_PreHEAT_CHARGE_TEM+40)) //大于等于-10°，预加热结束
        {
            st_heating=2;
            HeatCurt=HIGHEST_ALLOWED_HEAT_CHARGE_C; 
        }
    }
    //*************在边加热边充电阶段   
    if(st_heating==2)
    {
        if(Tavg>(STOP_ALLOWED_HEAT_CHARGE_TEM+40)) //大于等于10°，加热边充电阶段结束
        {
            st_heating=0;
            HeatCurt=0;
        } 
    }

}

//******************************************************************************
//* Function name:   HeatAndChargeControl
//* Description:     边充电边加热加热继电器控制
//* EntryParameter : void
//* ReturnValue    : void
//******************************************************************************
void HeatAndChargeControl(void)
{
    static unsigned int time;
    if(st_heating==0) //
    {
        TurnOff_INHK();//断开加热继电器
    } 
    else if(st_heating==1)  //进行预加热
    {
        TurnOn_INHK(); //闭合加热继电器 
    } 
    else
    {
        if(Tavg>(STOP_ALLOWED_HEAT_CHARGE_TEM+40)) //如果温度大于40+15度，则断开加热继电器
        {
            TurnOff_INHK();  
        }
        else if(Tavg<=(START_ALLOWED_HEAT_CHARGE_TEM+40))//如果温度小于40+10度，则闭合加热继电器
        { 
            TurnOn_INHK();
        }
        if((g_systemCurrent>=2)&&(g_lowestCellVoltage<2.8))//防止用电池进行加热
        {      
            time++;
            if(time>100)//250ms*100=25S
                TurnOff_INHK();        
        } 
        else
            time=0;
    }

}
//***************************************************************************
//***************************************************************************
//***************************the end******************************************
//*******************************************************************************
//*********************************************************************************