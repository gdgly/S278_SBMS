
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : 
//* File Name          : main.C
//* Author             : 孙丽雪
//* Software Version   : B00F.1704.010
//* Start Date         : 2017年4月6日                                   
//* Description        : BCU主板程序
//* Processor:         : Freescale MC2S12XEP100
//* Compiler:          : CodeWarriorV5.0 or higher
//* Details:           : 作为系统的副主板使用，监控V4.5.6三路总电压，接收创为灭火器报文
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
 
#include "BMS20.h"

/* global variables definitions */
//BMS Software Version define
unsigned char BMS_SW_Version[8]={0x0F,0x00,0x1F,0x17,0x04,0x00,0x01,0x00};//bC0F.1611.00.00.05
unsigned char BMUOK=0;
//**********************************************************************
//**********************************************************************
//**********************************************************************
//**********************************************************************
//**********************************************************************
//**********************************************************************
void main(void) 
{
    /* put your own code here */
    unsigned long t=0;
    
    //////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    IVBR = 0x7f;  //向量表重新定位因为之前的空间被BOOTLOADER程序占用了  
    ////////////////////////////////////////////////////
    CLKSEL = 0x80;//PLLCLK = 1 选则PLL作为时钟  bus clock = pll clock/2 ,,16M晶振，BUSCLOCK = 16M
    while(CLKSEL&0x80==0);
    //////////////////////////////////////////////////////////////    
    COPCTL |=0x07; //看门狗初始化，分频时钟为2的24次方; 大约2秒钟
    //////////////////////////////////////////////////////////////
    InitialHc595(); //Relay初始化                             
    gpioInit(); //IO port initialize   
    ADCInitial(); //ADC初始化
    ads1015_init();
    read_adc_config();
    MSCANInit();  //CAN初始化        
    PIT0_Init();
    RTIInit();  //设置定时器间隔时间    
    InitialAtM95M02();//M95M02 Flash初始化
    DFlash_Init(); //Data Flash初始化 
    Task02_BMN_GlobalVariables_Init(); 
    Task11_Vpn_StartAD_Polling(); //计算总电压值；此处不能删除，用于激活ADS1110
    
    ///////////////////////////////////////////////////
    delay(65000);  //19ms
    delay(65000); //cant delete
    ///////////////////////////////////////////////////
    InitialBMS();   //BCU初始化，包括参数，SOC，时间的初始化
    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
 
    turnOnLED0(); 	     // for debug 
    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    Interrupt_Priority_Set();
    EnableInterrupts; //开中断，接收状态机编码 
    TurnOnCan2Rx(); //打开内部中断，接收BMU数据
    ///////////////////////////////////////////////////////
    //BMU_initial();//////BMU自检
    //GetDCTem();/////此处调用是充电初始时检测插座温度异常和CC2电压用
    //bmsModeAwake();//系统模式判断
    
    //if(g_highestTemperature>45+40)
    //    Tavg=g_highestTemperature;
    //else
    //    Tavg=g_lowestTemperature;
    //BMUOK = 1;
    //****************************************************************               
    //SocOCVAdjust(30); //SOC OCV修正120min
    //InitialSoc();   //SOC，初始容量，剩余容量初始化，必须要在BMU数据都收到后才读取，否则温度不对，额定容量计算会错       
    //*******************************************************************
    while(1) 
    {
        _FEED_COP();   //2s内不喂狗，则系统复位
        TaskProcess();
        //注意：计算和控制在TaskCurrentDetect函数里
    }  
    //****************************************************************** 
    //////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
  /* please make sure that you never leave main */
}
//*********************************************************************
//*********************************************************************
//**********************the end ****************************************
//*********************************************************************
//*********************************************************************
//*********************************************************************