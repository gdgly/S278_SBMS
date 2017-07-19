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
    {0, 9, 10, TaskHeatProcess},                 //heat negative relay control process, command received from BMS master board.
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
    
    
    if((InsRelayControl==1)&&(TurnOnFlag==0))
    {
        TurnOn_INBK();//�պϾ�Ե���Ƽ̵���
        TurnOnFlag=1;
        TurnOffFlag=0;//�����Ե�̵����ѶϿ���־λ
          
    }
    if((InsRelayControl==0)&&(TurnOffFlag==0))
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
//**********************************************************************
//* Function name:    TaskBmuProcess
//* Description:      BMU�Լ죬�����ʱ���ղ�ȫBMU���ݣ�����ʾ�ڲ�ͨ�Ź���
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
     
    if(!bmuProcess2()) //BMU�Լ�   	        
    {        
        t++;
        _FEED_COP();   //2s�ڲ�ι�ڹ�����ϵͳ��λ
    } 
    else 
    {            
        t=0;
        GetTavgProcess();
        //State_Box_Online=0x3f;        
       
    }          
    if(t>=100) //��ʱ���ϱ���300ms*100=30s
    {
        if(t<=105)//5*300=1500ms
            return;
        else //t>105
        {          
            g_caution_Flag_3 |=0x01;    //to pc  
        }            
    
    }/////end of BMUͨ���ж�
    
}
*/
//***********************************************************************
//* Function name:   TaskReport2PC
//* Description:     ��BMS�������Ϣͨ���ڲ�����0x0c0450,0x0c0451,0x0c0452������λ�����
//*                  Ϊ��ֹ��֡�����Ա��ļ������
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
    /*
    if(inputP_state()==0)       // ��������
        gpio_state|=0x20;  
    else
        gpio_state&=0xdf;

    if(inputN_state()==0)       // ��������
        gpio_state|=0x40;
    else
        gpio_state&=0xbf;

    if(inputH_state()==0)       ///INPUT1 �ߵ�ѹ����
        gpio_state|=0x80; 
    else
        gpio_state&=0x7f;  
    */


                
} 


//***********************************************************************
//* Function name:   TaskStatusMachine
//* Description:     BMS����������������״̬�����������Ӧ����
//* EntryParameter : None
//* ReturnValue    : None
//************************************************************************
void TaskStatusMachine(void)//5ms����һ�� 
{
    static unsigned char counter1_10ms=0;
    static unsigned char counter1_500ms=0;
    static unsigned char counter2_500ms=0;
    static unsigned char counter3_500ms=0;
    static unsigned char clearSelfCountAC=0;
    static unsigned char state46=0;
    static unsigned char TestDelay=0;//���뵽����ģʽ�µ�ʱ����ʱ
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
                
    BiggestChaCurrentTmin =PulseRechargePowerAdjust1(Can_g_socValue,(g_lowestTemperature-40)); //�ƶ���������30s 
    BiggestChaCurrentTmax =PulseRechargePowerAdjust1(Can_g_socValue,(g_highestTemperature-40)); //�ƶ���������30s 
   
    if(BiggestChaCurrentTmin<=BiggestChaCurrentTmax)
        BiggestFeedbackCurt = BiggestChaCurrentTmin;
    else
        BiggestFeedbackCurt = BiggestChaCurrentTmax;
   
   //BiggestFeedbackCurt =PulseRechargePowerAdjust1(Can_g_socValue,(Tavg-40)); //�ƶ���������30s 
   
    BiggestChaCurrent5Tmin =ContinueRechargeCurt(Can_g_socValue,(g_lowestTemperature-40)); //�ƶ���������30s 
    BiggestChaCurrent5Tmax =ContinueRechargeCurt(Can_g_socValue,(g_highestTemperature-40)); //�ƶ���������30s 
   
    if(BiggestChaCurrent5Tmin<=BiggestChaCurrent5Tmax)
        BiggestFeedbackCurtContinuous = BiggestChaCurrent5Tmin;
    else
        BiggestFeedbackCurtContinuous = BiggestChaCurrent5Tmax;
   
   //BiggestFeedbackCurtContinuous = ContinueRechargeCurt(Can_g_socValue,(Tavg-40)); //�ƶ���������5min  
    
    
    switch(stateCode) //״̬���ж�
    {
         //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        //////////////////****************�ϵ����********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////// 
        case 11:
        case 81:
        case 141:
            state46=0;
            turnOnSW_Power();//���������
            TurnOff_INFK();//�Ͽ������̵���
            TurnOff_INZK();//�Ͽ������̵���
            break;
        case 12:  //********************��ѹ�Լ�*************//////////////
        case 82:
        case 142:
            tmr_p5=0;//146�޷�����
            HighVoltDetectPart1();//MSD�븺��ճ��
            break;
       
        case 14:  //***********************�պ�����************/////////////
        case 84:
        case 144:
            closeNegRelay();
            break;
        case 17:   //***********************��ظ�ѹ���*****//////////////
        case 87:
        case 147:
            HighVoltDetectPart2();//������·������ճ��
            break;
        case 20:   //*********************�����̵����պ�*******////////////
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
        //////////////////****************��������********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////      
        case 30:    //*********************�г�״̬***********/////////////
            
            //YoungMan_LT_step();
            if(plug_AC_CP_Connect==0)
            {
              
                BiggestDischargeCurt = BigDischargePowerAdjust((Tavg-40),Can_g_socValue);//SOF//30s
                BiggestDischargePower = BiggestDischargeCurt*g_highVoltageV1/1000;
                
                BiggestDisCurtContinuous = BigDischargePowerContinuous((Tavg-40),Can_g_socValue);//SOF//5min
                BiggestDisPowerContinuous = BiggestDisCurtContinuous*g_highVoltageV1/1000;
                
                BiggestFeedbackCurt = PulseRechargePowerAdjust1(Can_g_socValue,(Tavg-40)); //�ƶ���������30s 
                BiggestFeedbackPower = BiggestFeedbackCurt*g_highVoltageV1/1000; 
                
                BiggestFeedbackContinuous = 0;
                //BiggestFeedbackCurtContinuous = ContinueRechargeCurt(Can_g_socValue,(Tavg-40)); //�ƶ���������5min         
                //BiggestFeedbackContinuous = BiggestFeedbackCurtContinuous*g_highVoltageV1/1000;  
            }
            else if((plug_AC_CP_Connect==1)||(OffState==1))
            {

                BiggestDischargeCurt =0;//SOF//30s
                BiggestDischargePower = 0;
                
                BiggestDisCurtContinuous = 0;//SOF//5min
                BiggestDisPowerContinuous = 0;
                
                BiggestFeedbackCurt = 0; //�ƶ���������30s 
                BiggestFeedbackPower = 0; 
                
                BiggestFeedbackCurtContinuous = 0; //�ƶ���������5min         
                BiggestFeedbackContinuous = 0;


            }
            if((plug_DC_Connect==1)||(OffState==1)||(HighVolPowerOff==1))//״̬���л��������µ���ϻ����й���
            {
                //state_group4.Bit.Request_Power_Off = 1;//BMS��ѹ�µ�����
            } 
            SocEndDischargeAdjust(); //�ŵ�ĩ��SOC����
            CarFaultDone();//�г����̹��ϴ���,����Ϊѭ���ϱ�,��ֹ����ʱ����
            break;

        case 110:  //*********************������***********//////////////
            counter1_500ms++; 
            //state_group1.Bit.St_Charge_indicator = 1; //�����
            //state_group2.Bit.St_Charge_Achieve = 1;   //�����        
            if(counter1_500ms>=70) //7*70=490ms
            {
                counter1_500ms=0;
                TaskRechargeAC(); 
                HeatAndChargeControl();   
            }    
            break;
            
        case 170:   //********************����� *************///////////
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
        //////////////////****************�µ����********************///////////////// 
        //////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////                         
        case 40:   //*****************�Ͽ������̵���***********////////
        case 120:
            if(stateCode == 40)  
                openPosRelay(); //�Ͽ������̵���
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
            //    pc750_group.Bit.LockAction = 1; //�򿪵�����
            break;
        case 180:
            TurnOff_INA2K();//�Ͽ����̵���
            BiggestFeedbackContinuous = 0;
            bmsSelfcheckCounter=2;
            break;
             
        case 44:   //********************�Ͽ�����***************//////////      
        case 124:
        case 184: 
            TurnOff_INFK();//�Ͽ������̵���
            BiggestFeedbackContinuous = 0;
            delay(25000); //19ms
            delay(25000); //19ms
            break;  
            
        case 46:   //*****************��ѹ������************//////////////
        case 126:
        case 186:
            /*PRelayConnectTest();   ?????????
            if(state46==0)
            {
              
                delay(25000); //20ms
                delay(25000); //20ms
                TurnOff_INHK();//�رռ��ȼ̵��� 
                preChargeON=0;
                tmr_p1=0;           
                tmr_p2=0;
                tmr_p3=0; 
                tmr_p4=0;
                
                TurnOff_INBK();//�Ͽ�Ԥ��,��ֹ״̬����12��ת��46ʱԤ��̵���û�жϿ�
                StoreSysVariable();//                
                StoreSocRealvalue();
                state46=1;
                SelfCheck = 0;//�Լ�����,��ֹ���ϵ����Լ�ʱ����������
            }
            */
            delay(25000); //19ms
            delay(25000); //19ms
            bmsSelfcheckCounter=1;
            status_group1.Bit.St_BMS = 2;//��ѹ�ر�  //
            break;
            
        case 47:  //******************BMS�ϵ�***************////////////////
        case 127:
        case 187:
            bmsSelfcheckCounter=0;
            delay(25000); //20ms
            delay(25000); //20ms 
            delay(25000); //20ms
            delay(25000); //20ms
            turnOffSW_Power();//close�ܵ�Դ����  
            break;
        case 177://���Խ׶�

            openNegRelay();//�Ͽ������̵���
            delay(25000); //20ms
            
            _FEED_COP();   //2s�ڲ�ι�ڹ�����ϵͳ��λ
            
            TurnOff_INHK();//�Ͽ����ȼ̵���
            delay(25000); //19ms
            delay(25000); //19ms
            TurnOff_INA2K();//�Ͽ����̵���
            turnOffSW_Power();//close�ܵ�Դ���� 
            State177End=1;//��ת��179�ȴ�
            break;
        case 179://���Խ׶�
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