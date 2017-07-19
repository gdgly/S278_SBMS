//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : Fault.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2014.5.7
//* Description        : 该文件 对系统的各级故障判断
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h" 
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 

//***********************************************************************************
//***********************************************************************************
unsigned char  g_caution_Flag_1 =0;//BMS故障状态 FOR 上位机
unsigned char  g_caution_Flag_2 =0;
unsigned char  g_caution_Flag_3 =0;
unsigned char  g_caution_Flag_4 =0;
unsigned int g_errorCounter;

//**********************************************************************
//* Function name:   CarFaultDone
//* Description:     行车过程中故障的处理
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void CarFaultDone()
{
    
    //////////////////////////////*断高压*/////////////////////////////
    //////////////////////3级故障电池包过压,单体过压//////////////////////////////
    if(((CutChaCurt0.word & 0xdd0f)!=0)||(CutDisCurt0.word!=0))  
        OffState=1;//请求下电//去掉充电降低为0的单体过压2级及总电压过压2级
    //////////////////////////////*断高压*/////////////////////////////
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
    if(g_BmsModeFlag != DISCHARGING)
    {      
        if((g_lowestCellVoltage<=2.45)&&(g_lowestCellVoltage!=0))
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
        if(g_highestCellVoltage>=3.7)//10s
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
        if(g_highVoltageV1>=666)//3.7*114=421.8
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
    if(g_highestTemperature>95)//10s
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
   
    //BMSProtect();////////BMS最后断继电器
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