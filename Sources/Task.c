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
//* Description        : ���ļ�������ϵͳ�������񻮷ֲ��������й���
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "derivative.h" /* include peripheral declarations */
#include "BMS20.h" 

//**************************************************************************************
//* Variable definition                            
//**************************************************************************************/
//�����б�
static TASK_COMPONENTS TaskComps[] = 
{ 
    {0, 1,    1,    TaskCurrentDetect},         //����������� 1ms���һ�ε���ֵ //0 ��־λ��1��
    //{0, 700,  700,  TaskSocProcess},            //SOC����
    //{0, 297,   300,   TaskBmuProcess},            // BMU���ݴ�������//��ǰ60ms
    {0, 17,   20,   TaskVoltage},               // �ܵ�ѹ����   .
    {0, 297,  300,  TaskInsulation},            // ��Ե������� //�ô�ʱ����С600MS������̫С  80ms
    //{0, 97,  100,   TaskReport2PC},             //���ͱ��ĵ���λ��
    //{0, 5,    5,    TaskStatusMachine},         //״̬������
    //{0, 997, 1000,  TaskFaultProcess},          //100ms���ϴ���
    //{0, 8,    8,    TaskRechargeDC},            //ֱ�����
    //{0, 9,   10,    TaskRecordProcess},        //��ʷ��¼����
    {0, 9,  10, TaskHeatProcess},                 //heat negative relay control process, command received from BMS master board.
    {0, 10, 10, TaskKchgNCtrlProcess},
};

//**************************************************************************************
//* FunctionName   : TaskRemarks()
//* Description    : �����־����
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TaskRemarks(void)
{
    uchar i;

    for (i=0; i<TASKS_MAX; i++)          // �������ʱ�䴦��
    {
         if (TaskComps[i].Timer)          // ʱ�䲻Ϊ0
        {
            TaskComps[i].Timer--;         // ��ȥһ������
            if (TaskComps[i].Timer == 0)       // ʱ�������
            {
                 TaskComps[i].Timer = TaskComps[i].ItvTime;       // �ָ���ʱ��ֵ��������һ��
                 TaskComps[i].Run = 1;           // �����������
            }
        }
   }
}
//**************************************************************************************
//* FunctionName   : TaskProcess()
//* Description    : ������
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TaskProcess(void)
{
    uchar i;

    for (i=0; i<TASKS_MAX; i++)           // �������ʱ�䴦��
    {
        if (TaskComps[i].Run)           // ʱ�䲻Ϊ0
        {
             TaskComps[i].TaskHook();         // ��������
             TaskComps[i].Run = 0;          // ��־��0
        }
    }   
}
//************************************************************************
//* Function name:TaskCurrentDetect
//* Description:ͨ������ADͨ����ȡADֵ���پ�����ʽת���ɵ��������ݷ�Χȡ����һ������
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskCurrentDetect(void)//
{
    unsigned char i=0;
    unsigned char max=0;
    static unsigned char TurnOnFlag=0;
    static unsigned char TurnOffFlag=0; 

    turnOnADC(); //��ADC�ж�
    delay(10);  
    if((Int_Flag&0x01)==0x01) //����32��ADֵ��ż���
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
    
    if((max==2)||(max==3))//2,3�����϶�Ϊ2
    {
        max=2;
    }
    else if(max==4)//4�����϶�Ϊ2
    {
        max=3;
    }
    FireWarning = max;
    
    
    if((g_bms_sbms_ctrl_cmd.InsRelayControl==1)&&(TurnOnFlag==0))
    {
        TurnOn_INBK();//�պϾ�Ե���Ƽ̵���
        TurnOnFlag=1;
        TurnOffFlag=0;//�����Ե�̵����ѶϿ���־λ
          
    }
    if((g_bms_sbms_ctrl_cmd.InsRelayControl==0)&&(TurnOffFlag==0))
    {
        TurnOff_INBK(); //�Ͽ���Ե���Ƽ̵���
        TurnOffFlag=1;
        TurnOnFlag=0;  
    }
}  
//************************************************************************
//* Function name:TaskSocProcess
//* Description:ÿ����㲢����SOC��ÿ���ӱ���һ����ʷ��¼
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
	  	  	  
		I2CReadDate();  //��ȡϵͳʱ��
			         	
		if(CurrentTime[0]!=g_oldTime[0])   //ÿ���Ӽ���һ��SOC��ֵ
		{
				g_oldTime[0] = CurrentTime[0];//��
			  SocIntegral();//������ֵõ���SOCֵ	   
			  g_energyOfUsed = 0;	    		
		    StoreSocRealvalue();//ÿ�뱣��SOCֵ
		    //if(g_errorRecordRead_flag==0)
		        //StoreSysVariable();//for test
		         		    
    } //end of ÿ����
				
		if(CurrentTime[1]!=g_oldTime[1])// ÿ����SOCֵ����ϵͳ��������Ϊ�����ͨ��
	  {
			  g_oldTime[1] = CurrentTime[1];//��
			  
			  g_sysPara[PARA_SOC_VALUE] = g_socValue;
			  
			  if((g_BmsModeFlag == DISCHARGING)&&(StoreAHState==1))
			  {
			      ft=(StoreAHSOC-g_socValue)*C;//�����ۻ���ŵ�AH����
			      if(ft>1)
			      {			        
			          dischargeAH += (unsigned int)ft;
			          StoreAHSOC=First_g_socValue;
			      }
			  } 
			  else if((g_BmsModeFlag == FASTRECHARGING)&&((StoreAHState==1)))
			  {
			      ft=(g_socValue-StoreAHSOC)*C;//�����ۻ���ŵ�AH����
			      if(ft>1)
			      {			        
			          chargeAH += (unsigned int)ft;
			          StoreAHSOC=First_g_socValue;
			      }
			  }
			  
			   
			  //First_g_socValue=StoreAHSOC;      
			    
			   

			  //����״̬����
			  if(g_errorRecordRead_flag==0)
		        StoreSysVariable();//ÿ���ӱ�����ϼ�¼��Ϣ
		    if(StoreAHState==1)    	
		        StoreChargeDischargeAH();//�����ۻ���ŵ�������	  
		    //sendBMSIDtoBMU(); //����BMS�汾ID�Ÿ�BMU
			  //sendRealtimeToBMU(); //����BMSϵͳʱ���BMU
			      
    }

}

//***********************************************************************
//* Function name:   TaskGpioTest
//* Description:     Ӧ�ó�����뵽���Խ׶�ʱ,������A�ڵļ��
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
        if(TimerCP>=500)      //10ms ��ʱ�� 2S
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
    

    if(input6_state()==0)    //��練��     (�պϳ�練��ʱ,ACCҲ��)
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