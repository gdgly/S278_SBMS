//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : Task.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2014.5.7
//* Description        : 该文件将整个系统进行任务划分并整合所有过程
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "derivative.h" /* include peripheral declarations */
#include "BMS20.h" 



//**************************************************************************************
//* Variable definition                            
//**************************************************************************************/
//任务列表
static TASK_COMPONENTS TaskComps[] = 
{ 
    {0, 1,    1,    TaskCurrentDetect},         //电流检测任务 1ms检测一次电流值 //0 标志位，1，
    //{0, 700,  700,  TaskSocProcess},            //SOC处理
    //{0, 297,   300,   TaskBmuProcess},            // BMU数据处理任务//以前60ms
    {0, 17,   20,   TaskVoltage},               // 总电压计算   .
    {0, 297,  300,  TaskInsulation},            // 绝缘电阻计算 //该处时间最小600MS，不能太小  80ms
    //{0, 97,  100,   TaskReport2PC},             //发送报文到上位机
    //{0, 5,    5,    TaskStatusMachine},         //状态机处理
    //{0, 997, 1000,  TaskFaultProcess},          //100ms故障处理
    //{0, 8,    8,    TaskRechargeDC},            //直流充电
    //{0, 9,   10,    TaskRecordProcess},        //历史记录处理
    {0, 9, 10, TaskHeatProcess},                 //heat negative relay control process, command received from BMS master board.
};

//**************************************************************************************
//* FunctionName   : TaskRemarks()
//* Description    : 任务标志处理
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TaskRemarks(void)
{
    uchar i;

    for (i=0; i<TASKS_MAX; i++)          // 逐个任务时间处理
    {
         if (TaskComps[i].Timer)          // 时间不为0
        {
            TaskComps[i].Timer--;         // 减去一个节拍
            if (TaskComps[i].Timer == 0)       // 时间减完了
            {
                 TaskComps[i].Timer = TaskComps[i].ItvTime;       // 恢复计时器值，从新下一次
                 TaskComps[i].Run = 1;           // 任务可以运行
            }
        }
   }
}
//**************************************************************************************
//* FunctionName   : TaskProcess()
//* Description    : 任务处理
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TaskProcess(void)
{
    uchar i;

    for (i=0; i<TASKS_MAX; i++)           // 逐个任务时间处理
    {
        if (TaskComps[i].Run)           // 时间不为0
        {
             TaskComps[i].TaskHook();         // 运行任务
             TaskComps[i].Run = 0;          // 标志清0
        }
    }   
}
//************************************************************************
//* Function name:TaskCurrentDetect
//* Description:通过两个AD通道读取AD值，再经过公式转换成电流，根据范围取其中一个电流
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskCurrentDetect(void)//
{
    unsigned char i=0;
    unsigned char max=0;
    static unsigned char TurnOnFlag=0;
    static unsigned char TurnOffFlag=0; 

    turnOnADC(); //打开ADC中断
    delay(10);  
    if((Int_Flag&0x01)==0x01) //采了32次AD值后才计算
    {
        CurrentCalculation(&g_systemCurrent, &g_systemCurrent_2);
        //g_systemCurrent = CurrentCalculation();
    }

    max = FireMessage[0];
    for(i=0;i<10;i++) 
    {
        if(FireMessage[i]>=max){
            max=FireMessage[i];
        }
    }
    
    if((max==2)||(max==3))//2,3级故障定为2
    {
        max=2;
    }
    else if(max==4)//4级故障定为2
    {
        max=3;
    }
    FireWarning = max;
    
    
    if((InsRelayControl==1)&&(TurnOnFlag==0))
    {
        TurnOn_INBK();//闭合绝缘控制继电器
        TurnOnFlag=1;
        TurnOffFlag=0;//清除绝缘继电器已断开标志位
          
    }
    if((InsRelayControl==0)&&(TurnOffFlag==0))
    {
        TurnOff_INBK(); //断开绝缘控制继电器
        TurnOffFlag=1;
        TurnOnFlag=0;  
    }
}  
//************************************************************************
//* Function name:TaskSocProcess
//* Description:每秒计算并保存SOC，每分钟保存一次历史记录
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskSocProcess(void)
{
    
    static float ahCharge1A=0;
    static float ahDischarge1A=0;
    static unsigned char DCfinish=0;
   
	  unsigned char i=0;
	  float ft=0;
	  	  	  
		I2CReadDate();  //读取系统时间
			         	
		if(CurrentTime[0]!=g_oldTime[0])   //每秒钟计算一次SOC的值
		{
				g_oldTime[0] = CurrentTime[0];//秒
			  SocIntegral();//计算积分得到的SOC值	   
			  g_energyOfUsed = 0;	    		
		    StoreSocRealvalue();//每秒保存SOC值
		    //if(g_errorRecordRead_flag==0)
		        //StoreSysVariable();//for test
		         		    
    } //end of 每秒钟
				
		if(CurrentTime[1]!=g_oldTime[1])// 每分钟SOC值赋给系统参数，作为保存或通信
	  {
			  g_oldTime[1] = CurrentTime[1];//分
			  
			  g_sysPara[PARA_SOC_VALUE] = g_socValue;
			  
			  if((g_BmsModeFlag == DISCHARGING)&&(StoreAHState==1))
			  {
			      ft=(StoreAHSOC-g_socValue)*C;//计算累积充放电AH容量
			      if(ft>1)
			      {			        
			          dischargeAH += (unsigned int)ft;
			          StoreAHSOC=First_g_socValue;
			      }
			  } 
			  else if((g_BmsModeFlag == FASTRECHARGING)&&((StoreAHState==1)))
			  {
			      ft=(g_socValue-StoreAHSOC)*C;//计算累积充放电AH容量
			      if(ft>1)
			      {			        
			          chargeAH += (unsigned int)ft;
			          StoreAHSOC=First_g_socValue;
			      }
			  }
			  
			   
			  //First_g_socValue=StoreAHSOC;      
			    
			   

			  //保存状态数据
			  if(g_errorRecordRead_flag==0)
		        StoreSysVariable();//每分钟保存故障记录信息
		    if(StoreAHState==1)    	
		        StoreChargeDischargeAH();//保存累积充放电总容量	  
		    //sendBMSIDtoBMU(); //发送BMS版本ID号给BMU
			  //sendRealtimeToBMU(); //发送BMS系统时间给BMU
			      
    }

}
//**********************************************************************
//* Function name:    TaskBmuProcess
//* Description:      BMU自检，如果长时间收不全BMU数据，则显示内部通信故障
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
/*void TaskBmuProcess(void) 
{
   static unsigned long t=0;
   
   unsigned char i=0,k=0x01,m=0x02;
   unsigned char ErrorState=0;
   float Value=0;
   unsigned long buff=0;
     
    if(!bmuProcess2()) //BMU自检   	        
    {        
        t++;
        _FEED_COP();   //2s内不喂内狗，则系统复位
    } 
    else 
    {            
        t=0;
        GetTavgProcess();
        //State_Box_Online=0x3f;        
       
    }          
    if(t>=100) //超时故障报告300ms*100=30s
    {
        if(t<=105)//5*300=1500ms
            return;
        else //t>105
        {          
            g_caution_Flag_3 |=0x01;    //to pc  
        }            
    
    }/////end of BMU通信判断
    
}
*/
//***********************************************************************
//* Function name:   TaskReport2PC
//* Description:     将BMS的相关信息通过内部报文0x0c0450,0x0c0451,0x0c0452发给上位机软件
//*                  为防止丢帧，所以报文间隔发送
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
/*void TaskReport2PC(void) 
{
    static unsigned char ct=0;
    ct++;
    if(ct==1)
        bmsToPcInfo450();
    if(ct==2)
			  bmsToPcInfo451();
    if(ct==3)
			  bmsToPcInfo552();
    if(ct==4)
        bmsToPcInfo553();
    if(ct==5)
        bmsToPcInfo554();
    if(ct==6)
    {   
        if(g_BmsModeFlag == RECHARGING)
            bmsToPcInfo750();   
        ct=0;
    }
}
*/ 


//***********************************************************************
//* Function name:   TaskGpioTest
//* Description:     应用程序进入到调试阶段时,对主板A口的检测
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskGpioTest(void) 
{
    static unsigned int TimerCP=0; 
    if((input2_state()==0)&&(input3_state()==1))   //16A 680R
        C0552_0 |= 0x01; 
    else
        C0552_0 &= 0xfe;
    
    if((input2_state()==0)&&(input3_state()==0))   //32A 220R
        C0552_0 |= 0x02; 
    else
        C0552_0 &= 0xfd;

    if(input6_state()==0)       //CP
    {
        C0552_0|=0x04;
        if(TimerCP>=500)      //10ms 定时器 2S
        {
            //TurnOn_CP();
            TimerCP=501;
        }
    }
    else 
    {
        C0552_0&=0xfb;
        //TurnOff_CP();
        TimerCP=0;
    }
    
    if(input4_state()==0)      //ACC
        C0552_0|=0x08; 
    else
        C0552_0&=0xf7;
    

    if(input6_state()==0)    //充电反馈     (闭合充电反馈时,ACC也亮)
        C0552_0|=0x10; 
    else
        C0552_0&=0xef;

    if(input5_state()==0)       //CC2
        C0552_0|=0x20; 
    else
        C0552_0&=0xdf;
    /*
    if(inputP_state()==0)       // 正极反馈
        gpio_state|=0x20;  
    else
        gpio_state&=0xdf;

    if(inputN_state()==0)       // 负极反馈
        gpio_state|=0x40;
    else
        gpio_state&=0xbf;

    if(inputH_state()==0)       ///INPUT1 高低压互锁
        gpio_state|=0x80; 
    else
        gpio_state&=0x7f;  
    */


                
} 


//***********************************************************************
//* Function name:   TaskStatusMachine
//* Description:     BMS根据整车发过来的状态机编码进行相应动作
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskStatusMachine(void)//5ms调用一次 
{
    static unsigned char counter1_10ms=0;
    static unsigned char counter1_500ms=0;
    static unsigned char counter2_500ms=0;
    static unsigned char counter3_500ms=0;
    static unsigned char clearSelfCountAC=0;
    static unsigned char state46=0;
    static unsigned char TestDelay=0;//进入到调试模式下的时间延时
    float BiggestDisCurrent=0;
    float BiggestChaCurrentTmin=0;
    float BiggestChaCurrentTmax=0;
    float BiggestChaCurrent5Tmin=0;
    float BiggestChaCurrent5Tmax=0;
    SignalOnOffJudge(); 
    HeatManage();  
    stateCodeTransfer();
    
    
    BiggestDischargeCurt = BigDischargePowerAdjust((Tavg-40),Can_g_socValue);//SOF//30s
               
    BiggestDisCurtContinuous = BigDischargePowerContinuous((Tavg-40),Can_g_socValue);//SOF//5min
   
    if(BiggestDisCurtContinuous>BiggestDischargeCurt)
        BiggestDisCurtContinuous = BiggestDischargeCurt; 
                
    BiggestChaCurrentTmin =PulseRechargePowerAdjust1(Can_g_socValue,(g_lowestTemperature-40)); //制动能量回收30s 
    BiggestChaCurrentTmax =PulseRechargePowerAdjust1(Can_g_socValue,(g_highestTemperature-40)); //制动能量回收30s 
   
    if(BiggestChaCurrentTmin<=BiggestChaCurrentTmax)
        BiggestFeedbackCurt = BiggestChaCurrentTmin;
    else
        BiggestFeedbackCurt = BiggestChaCurrentTmax;
   
   //BiggestFeedbackCurt =PulseRechargePowerAdjust1(Can_g_socValue,(Tavg-40)); //制动能量回收30s 
   
    BiggestChaCurrent5Tmin =ContinueRechargeCurt(Can_g_socValue,(g_lowestTemperature-40)); //制动能量回收30s 
    BiggestChaCurrent5Tmax =ContinueRechargeCurt(Can_g_socValue,(g_highestTemperature-40)); //制动能量回收30s 
   
    if(BiggestChaCurrent5Tmin<=BiggestChaCurrent5Tmax)
        BiggestFeedbackCurtContinuous = BiggestChaCurrent5Tmin;
    else
        BiggestFeedbackCurtContinuous = BiggestChaCurrent5Tmax;
   
   //BiggestFeedbackCurtContinuous = ContinueRechargeCurt(Can_g_socValue,(Tavg-40)); //制动能量回收5min  
    
    
    switch(stateCode) //状态机判断
    {
         //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        //////////////////****************上电过程********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////// 
        case 11:
        case 81:
        case 141:
            state46=0;
            turnOnSW_Power();//打开软件开关
            TurnOff_INFK();//断开负极继电器
            TurnOff_INZK();//断开正极继电器
            break;
        case 12:  //********************低压自检*************//////////////
        case 82:
        case 142:
            tmr_p5=0;//146无法清零
            HighVoltDetectPart1();//MSD与负极粘连
            break;
       
        case 14:  //***********************闭合主负************/////////////
        case 84:
        case 144:
            closeNegRelay();
            break;
        case 17:   //***********************电池高压检测*****//////////////
        case 87:
        case 147:
            HighVoltDetectPart2();//负极断路与正极粘连
            break;
        case 20:   //*********************正极继电器闭合*******////////////
        case 90:
        case 150:
            if(stateCode == 20)
                TurnOn_INZK();
            else if(stateCode == 90)
                TurnOn_INA1K();
            else if(stateCode == 150)
                TurnOn_INA2K();
            HighVoltDetectPart3();               
            break;
        //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        //////////////////****************工作过程********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////      
        case 30:    //*********************行车状态***********/////////////
            
            //YoungMan_LT_step();
            if(plug_AC_CP_Connect==0)
            {
              
                BiggestDischargeCurt = BigDischargePowerAdjust((Tavg-40),Can_g_socValue);//SOF//30s
                BiggestDischargePower = BiggestDischargeCurt*g_highVoltageV1/1000;
                
                BiggestDisCurtContinuous = BigDischargePowerContinuous((Tavg-40),Can_g_socValue);//SOF//5min
                BiggestDisPowerContinuous = BiggestDisCurtContinuous*g_highVoltageV1/1000;
                
                BiggestFeedbackCurt = PulseRechargePowerAdjust1(Can_g_socValue,(Tavg-40)); //制动能量回收30s 
                BiggestFeedbackPower = BiggestFeedbackCurt*g_highVoltageV1/1000; 
                
                BiggestFeedbackContinuous = 0;
                //BiggestFeedbackCurtContinuous = ContinueRechargeCurt(Can_g_socValue,(Tavg-40)); //制动能量回收5min         
                //BiggestFeedbackContinuous = BiggestFeedbackCurtContinuous*g_highVoltageV1/1000;  
            }
            else if((plug_AC_CP_Connect==1)||(OffState==1))
            {

                BiggestDischargeCurt =0;//SOF//30s
                BiggestDischargePower = 0;
                
                BiggestDisCurtContinuous = 0;//SOF//5min
                BiggestDisPowerContinuous = 0;
                
                BiggestFeedbackCurt = 0; //制动能量回收30s 
                BiggestFeedbackPower = 0; 
                
                BiggestFeedbackCurtContinuous = 0; //制动能量回收5min         
                BiggestFeedbackContinuous = 0;


            }
            if((plug_DC_Connect==1)||(OffState==1)||(HighVolPowerOff==1))//状态机切换或者有下电故障或者有故障
            {
                //state_group4.Bit.Request_Power_Off = 1;//BMS高压下电请求
            } 
            SocEndDischargeAdjust(); //放电末端SOC修正
            CarFaultDone();//行车过程故障处理,功率为循环上报,防止由于时序误报
            break;

        case 110:  //*********************慢充充电***********//////////////
            counter1_500ms++; 
            //state_group1.Bit.St_Charge_indicator = 1; //充电中
            //state_group2.Bit.St_Charge_Achieve = 1;   //充电中        
            if(counter1_500ms>=70) //7*70=490ms
            {
                counter1_500ms=0;
                TaskRechargeAC(); 
                HeatAndChargeControl();   
            }    
            break;
            
        case 170:   //********************快充充电 *************///////////
            counter1_500ms++;
         //   state_group1.Bit.St_Charge_indicator = 1;  
            if(counter1_500ms>=70)
            {
                counter1_500ms=0;
                HeatAndChargeControl();
                GetDCTem();              
            }
            break;
 
        //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        //////////////////****************下电过程********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////                         
        case 40:   //*****************断开主正继电器***********////////
        case 120:
            if(stateCode == 40)  
                openPosRelay(); //断开正极继电器
            else if(stateCode == 120)
                TurnOff_INA1K();    
            bmsSelfcheckCounter=2;
            delay(25000); //19ms
            delay(25000); //19ms
            BiggestDischargePower=0;
            BiggestDisPowerContinuous=0;
            BiggestFeedbackPower=0;
            BiggestFeedbackContinuous=0;
            //if(g_BmsModeFlag == RECHARGING)
            //    pc750_group.Bit.LockAction = 1; //打开电子锁
            break;
        case 180:
            TurnOff_INA2K();//断开快充继电器
            BiggestFeedbackContinuous = 0;
            bmsSelfcheckCounter=2;
            break;
             
        case 44:   //********************断开主负***************//////////      
        case 124:
        case 184: 
            TurnOff_INFK();//断开主负继电器
            BiggestFeedbackContinuous = 0;
            delay(25000); //19ms
            delay(25000); //19ms
            break;  
            
        case 46:   //*****************高压掉电检测************//////////////
        case 126:
        case 186:
            /*PRelayConnectTest();   ?????????
            if(state46==0)
            {
              
                delay(25000); //20ms
                delay(25000); //20ms
                TurnOff_INHK();//关闭加热继电器 
                preChargeON=0;
                tmr_p1=0;           
                tmr_p2=0;
                tmr_p3=0; 
                tmr_p4=0;
                
                TurnOff_INBK();//断开预充,防止状态机由12跳转到46时预充继电器没有断开
                StoreSysVariable();//                
                StoreSocRealvalue();
                state46=1;
                SelfCheck = 0;//自检清零,防止不断电在自检时程序不起作用
            }
            */
            delay(25000); //19ms
            delay(25000); //19ms
            bmsSelfcheckCounter=1;
            status_group1.Bit.St_BMS = 2;//高压关闭  //
            break;
            
        case 47:  //******************BMS断电***************////////////////
        case 127:
        case 187:
            bmsSelfcheckCounter=0;
            delay(25000); //20ms
            delay(25000); //20ms 
            delay(25000); //20ms
            delay(25000); //20ms
            turnOffSW_Power();//close总电源开关  
            break;
        case 177://调试阶段

            openNegRelay();//断开负极继电器
            delay(25000); //20ms
            
            _FEED_COP();   //2s内不喂内狗，则系统复位
            
            TurnOff_INHK();//断开加热继电器
            delay(25000); //19ms
            delay(25000); //19ms
            TurnOff_INA2K();//断开快充继电器
            turnOffSW_Power();//close总电源开关 
            State177End=1;//跳转到179等待
            break;
        case 179://调试阶段
            TaskGpioTest();
            TestDelay++;
            if(TestDelay>80)//5*80=400
            {
                TestDelay=0;
            }  
            break;
        default:
            break;                        
    }

}

void TaskHeatProcess(void){
    if(Heat_P_Realy_Status == 1){
        TurnOn_INA1K();
    }
    else{
        TurnOff_INA1K();
    }
}

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************