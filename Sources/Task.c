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
    {0, 1,      1,      TaskCurrentDetect},     //电流检测任务 1ms检测一次电流值 //0 标志位，1，
    {0, 17,     20,     TaskVoltage},           // 总电压计算   .
    {0, 297,    300,    TaskInsulation},        // 绝缘电阻计算 //该处时间最小600MS，不能太小  80ms
    {0, 9,      10,     TaskHeatProcess},       //heat negative relay control process, command received from BMS master board.
    {0, 10,     10,     TaskKchgNCtrlProcess},
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
    
    for(i=0; i<10; i++){
        if(g_FireMsg[i].bits.work_status == 4){
            if(g_FireMsg[i].bits.smoke_warning == 1){
                max = 1;
                break;
            }
        }
    }

    for(i=0; i<10; i++){
        if(g_FireMsg[i].bits.work_status == 4){
            if(g_FireMsg[i].bits.OT_warning == 1
            || g_FireMsg[i].bits.T_jump_warning == 1){
                max = 2;
                break;
            }
        }
    }

    for(i=0; i<10; i++){
        if(g_FireMsg[i].bits.work_status == 4){
            if(g_FireMsg[i].bits.fire_warning == 1){
                max = 3;
                break;
            }
        }
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