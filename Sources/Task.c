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
    {0, 9,  10, TaskHeatProcess},                 //heat negative relay control process, command received from BMS master board.
    {0, 10, 10, TaskKchgNCtrlProcess},
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
    
    
    if((g_bms_sbms_ctrl_cmd.InsRelayControl==1)&&(TurnOnFlag==0))
    {
        TurnOn_INBK();//闭合绝缘控制继电器
        TurnOnFlag=1;
        TurnOffFlag=0;//清除绝缘继电器已断开标志位
          
    }
    if((g_bms_sbms_ctrl_cmd.InsRelayControl==0)&&(TurnOffFlag==0))
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
} 

void TaskHeatProcess(void){
    if(g_bms_sbms_ctrl_cmd.Heat_P_Realy_Status == 1){
        TurnOn_INA1K();
    }
    else{
        TurnOff_INA1K();
    }
}

void TaskKchgNCtrlProcess(void) {
    if(g_bms_sbms_ctrl_cmd.Kchg_N_ctrl_cmd == 1){
        TurnOn_INFK();
    }
    else{
        TurnOff_INFK();
    }
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************