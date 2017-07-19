//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : �����ֽ�6123
//* File Name          : DC_ChangerProcess.c
//* Author             : ����ѩ
//* Version            : V1.0.0
//* Start Date         : 2016,11,30
//* Description        : �������
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"

unsigned char plug_DC_Connect=0;             //CC2�����ź�
static unsigned char sendi1=0;               //��ȫ�ֱ���
static unsigned char sendi2=0;               //��ȫ�ֱ���
unsigned char	m_chmmode = CONSTANT_CURT_WORK;//ȫ�ֱ���
unsigned char fastendflag=0;                 //ȫ�ֱ���,��������־λ
float	m_askvoltage=0;                        //���ݵ�ذ��������
float	m_askcurrent=0;                        //ȫ�ֱ�����ֱ��������
/////////�����,BMSֹͣ����ԭ��////////////////
unsigned char  fastend1=0;                   //ȫ��
unsigned char  fastend2=0;
unsigned char  fastend3=0;
unsigned char  fastend4=0xf0;

/////////����ã�����BSM����ص�ǰ״̬////////////////
unsigned char  fasterror11 =0;
unsigned char  fasterror12 =0x10;            //�����磬0 b 00 01 00 00

unsigned char BROStopState = 0;              //�յ�CRO��,���ٷ�BRO
unsigned int chargeTimes=0;                  //������

//////////��ʱʱ���־///////////////////////////////
unsigned int CRMOverTimeBefore60s = 0;//�ղ���CRO����60s��ϵͳ������
unsigned int CRMOverTimeBefore = 0;//�ղ���CRO����
unsigned int CRMOverTime = 0;//CRM��ʱ����0xaa��ʱ
unsigned int CROOverTime = 0;//5sδ�յ�CRO����
unsigned int CROOverTime60s = 0;//CRO��ʱ����0xaa��ʱ
unsigned int CCSOverTime=0;                 //CCS��ʱʱ��
unsigned int CSTOverTime=0;                 //CST��ʱʱ��
unsigned int CSDOverTime=0;                 //CSD��ʱʱ��
unsigned char OverTimeState=0;               //��ʱ��־

unsigned char CC2ResState = 0; //0λ��״̬ 1����״̬ 2����״̬
float DCTem1=0;//DC1�¶Ȳɼ�
float DCTem2=0;//DC2�¶Ȳɼ�
float CC2VOL = 0;
float PowerVOL = 0;
//unsigned char DC_CC2Count = 0;
unsigned char BROErrorAA = 1;//BRO������0xaa
unsigned char DC_Vesion = 1;//DC���汾1���Ϲ���;2���¹���
unsigned char SelfState3 = 0;     //�Լ���������3��־
unsigned char BEMStop=0;


unsigned char DCStartState=0;//��俪ʼ��־:1�յ�CRM,2�յ�CHM

static unsigned char SetBSD = 0;//BSD�Ƿ��Ѿ�����
static unsigned char SetBST = 0;//BST�Ƿ��Ѿ�����
unsigned char ChangerStopState = 0;//���ʱ���û�г�������SOCͣ����99.2%
//******************************************************************************
//* Function name:   GetRequestCurrentDC
//* Description:     ���������������� C=120A
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
float GetRequestCurrentDC(void) 
{
    float DCAmp;
    static unsigned char ChanceHappen1 = 0;//��ѹ����3.78�󲻻ָ�
    static unsigned char ChanceHappen2 = 0;//��ѹ����3.92�󲻻ָ�
    
    m_askvoltage = HIGHEST_ALLOWED_CHARGE_V;
    
    if(((g_highestCellVoltage>3.78)||(ChanceHappen1==1))&&(ChanceHappen2==0))
    {
        ChanceHappen1 = 1;
    }
    if((g_highestCellVoltage>3.92)||(ChanceHappen2==1))
    {
        ChanceHappen1 = 2;
        ChanceHappen2 = 1;
    }
    
    if(Tavg>=(54+40))  //����¶ȴ��ڵ���55��[54,)
    {
        m_askvoltage=0;
        DCAmp=0;
    }
    else if(Tavg>(45+40))//����¶�45~54��(45,54)
    {
        if(ChanceHappen1<=1)
            DCAmp=0.5*C;////0.5c
        else
            DCAmp=0.33*C;////0.33c 
    }           
    else if(Tavg >(20+40)) //����¶�45~50��(20,45]
    {
        if(ChanceHappen1<=1)
            DCAmp=115;
        else
            DCAmp=0.33*C;////0.33c     
    }
    else if(Tavg >(15+40)) //����¶�(15,20]
    {
        if(ChanceHappen1==0)
            DCAmp=115;
        else if(ChanceHappen1==1)
            DCAmp=0.5*C;////0.33c
        else if(ChanceHappen1==2)
            DCAmp=0.33*C;////0.33c     
             
    }
    else if(Tavg >(10+40)) //����¶�45~50��(10,15]
    {
        if(ChanceHappen1==0)
            DCAmp=0.6*C;
        else if(ChanceHappen1==1)
            DCAmp=0.5*C;////0.33c
        else if(ChanceHappen1==2)
            DCAmp=0.33*C;////0.33c 
    } 
    else if(Tavg >(5+40)) //����¶�25~30��(5,10]
    {
        if(ChanceHappen1==0)
            DCAmp=0.5*C;
        else if(ChanceHappen1==1)
            DCAmp=0.4*C;////0.4c
        else if(ChanceHappen1==2)
            DCAmp=0.3*C;////0.3c 
    }
    else if(Tavg >(0+40)) //����¶�10~25��(0,5]
    {
        if(ChanceHappen1==0)
            DCAmp=0.4*C;
        else if(ChanceHappen1==1)
            DCAmp=0.3*C;////0.3c
        else if(ChanceHappen1==2)
            DCAmp=0.2*C;////0.2c 
    }
    else if(Tavg >(-10+40))//����¶�5~10��(-10,0]
    {
        DCAmp=0.08*C;////0.08c
    }
    else if(Tavg >=(-15+40))//[-15~-10]
    {
        DCAmp=0.05*C;////0.5c 
    }
    else //����¶�С��0�ȣ��������ѹΪ0���������Ϊ0
    {
        m_askvoltage=0;
        DCAmp=0;
    }   

    if(CutDCChaCurt50.word!=0)
        DCAmp=0.5*DCAmp;
    
    return DCAmp;    
}
//**********************************************************************
//* Function name:   DCFaultDone
//* Description:     �������й��ϵĴ���
//* EntryParameter : None
//* ReturnValue    : 0:�޹���;1:�й���
//**********************************************************************
unsigned char DCFaultDone(void)
{
    /*static unsigned char ReturnValue = 0;//һ����ֵ,���ܸı�
    //////////////////////////////�ϸ�ѹ/////////////////////////////        
    if((Fault_Group1.Bit.F7_Bat_Over_V_2==1)||(Fault_Group6.Bit.F7_Bat_Over_V_3==1)|| //�ܵ�ѹ
       (Fault_Group1.Bit.F1_Cell_Over_V_2==1)||(Fault_Group6.Bit.F5_Cell_Over_V_3==1))//�����ѹ
    {
        
        fastendflag=1;
        fastend4|=0x04;//��ѹ�쳣
        OffState=1;//�����µ�
        ReturnValue = 1;
    }
    else if((Fault_Group4.Bit.F4_P_Ins_Err_2==1)||(Fault_Group4.Bit.F6_N_Ins_Err_2==1))//��Ե2������
    {
        fastendflag=1;
        fastend2|=0x01;//��Ե�쳣
        OffState=1;//�����µ�
        ReturnValue = 1;    
    }
    else if((Fault_Group2.Bit.F3_Ch_Sustain_Over_I_2) || (Fault_Group8.Bit.F2_Ch_Instant_Over_I_3==1)) //������2����3��
    {
        fastendflag=1;
        fastend4|=0x01;//����������
        OffState=1;//�����µ�
        ReturnValue = 1;     
    } 
    else if((DCTem1>=70)||(DCTem2>=70))
    {
        fastendflag=1;
        fastend2|=0x40;//�������¶ȹ���
        OffState=1;//�����µ�
        ReturnValue = 1;     
    }   
    else if(Fault_Group2.Bit.F7_Temp_Over_2 == 1)//���ǳ���3���¶ȹ��߹���
    {
        fastendflag=1;
        fastend3|=0x01;//����¶ȹ���
        OffState=1;//�����µ�
        ReturnValue = 1;
    } 
    else if((Fault_Group7.Bit.F7_IN_CAN_Err == 1)||(Fault_Group6.Bit.F3_Ch_CAN_Err==1)||(ACCha_Flag_BST==1))//���ǳ���3���ڲ�ͨ�Ź��ϡ�����ͨ�Ź��ϡ����ʱ����Ϊ��                                                                                     )
    {
        fastendflag=1;
        fastend3|=0x40;//��������
        OffState=1;//�����µ�
        ReturnValue = 1;
    }
    return ReturnValue;*/    
    //////////////////////////////*�ϸ�ѹ*/////////////////////////////
}

//**********************************************************************
//* Function name:   TaskRechargeDC
//* Description:     ������̣���Լÿ10ms����һ�θú���
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
/*void TaskRechargeDC(void)
{
    static unsigned char counter_250ms1=0;
    static unsigned char CHMStep1=0;
    
    static unsigned char counter_250ms2=0;
    static unsigned char CHMStep2=0;
    
    static unsigned char counter_250ms3=0;
    static unsigned char CHMStep3=0;
    
    static unsigned char counter_250ms4=0;
    static unsigned char CHMStep4_5=0;
    static unsigned char counter_50ms=0;
    
    static unsigned char counter_250ms5=0;
    static unsigned char CHMStep5=0;
    
    static unsigned char counter_250ms6=0;
    static unsigned char counter_250ms6_1=0;
    static unsigned char CHMStep6=0;
    
    static unsigned char counter_250ms7=0;
    
    
    static unsigned char DCFinish=0; //ֱ�����ʱֻ����Q1һ��
    static unsigned char timer1S=0;//�����ѹ�����,ֹͣ��������ж���ʱ
    static unsigned char timer2S=0;//��һ�ν�������ʱ
    static unsigned char timer3S=0;//�ڶ��ν�������ʱ
    static unsigned char ChanceCurt=0;//���ĩ��,�����仯�󲻻���,
    static unsigned char ChangeTimeState=0;//���������Խ���һ�μӷ�
    float curr=0;
    unsigned char ErrorState = 0;
    if(g_BmsModeFlag != FASTRECHARGING)
        return;
    if((g_highestTemperature>=(HIGHEST_ALLOWED_CHARGE_T+40))||(g_lowestTemperature<(LOWEST_ALLOWED_CHARGE_T+40)))//40+50��40+0
    {
        fastend3|=0x01; //Temperature����ֹ���
        fastendflag=1; //���ֹͣ����־  
    }      
    curr = GetRequestCurrentDC();  
    ErrorState = DCFaultDone();
    if((ErrorState==1)||(CutDCChaCurt0.word!=0))//ֱ��������µ���ϴ���
    {
        m_askcurrent = 0;     
    } 
    else 
    {
        m_askcurrent = curr + HeatCurt;
        if(m_askcurrent>=115)
            m_askcurrent = 115;
    }
    if((g_highestCellVoltage>=CHARGE_CUTDOWN_CV1 )||(ChanceCurt==1))//����Ϊ����������һ��
    {         
        timer2S++;
        if(timer2S>=80)//10*80=800ms
        {    
            timer2S=81;                      
            if(m_askcurrent>0.1*C)
                m_askcurrent=0.1*C;
            else if(m_askcurrent==0)
                m_askcurrent=0;
            ChanceCurt=1;
        }
    } 
    else 
    {
        timer2S=0;
    }
	  if(g_highestCellVoltage>=HIGHEST_ALLOWED_CHARGE_CV)  //4.15Vֹͣ���
    {
        timer1S++;
        if(timer1S>=100)//10ms*100=1000ms
        {          
            if(DCFinish==0)
            {
              
                SocRechargeAdjust(); //��SOC��Q1
        	      StoreParameter();
        	      DCFinish=1;
    	      
            }   
            fastendflag=1;
            fastend1|=0x10;//�����ѹ�ﵽĿ��ֵ��������ֹ���
            timer1S=103; 
                      
        }
    } 
    else 
    {
        timer1S=0; 
    }        
    //////////////////////////////���ʱ���͹���///////////////////////////
    BiggestFeedbackContinuous = m_askcurrent*m_askvoltage/1000;//���ʱ��� 
    
    
    //////////��䷢����Ϣ 
	  if(CHMStep)
		{     
    	  if(CHMStep==0x01)      //1801f456 00,��֡���ģ�����250ms�����ļ��10ms
    	  {

    	      counter_250ms1++;
    	      if(counter_250ms1>=25)
    	      {
        	      if(DCStartState==2)//�¹��꿪ʼ
        	      {
                    cpuToCHMBHM();
                    CRMOverTimeBefore++;
                    if(CRMOverTimeBefore>=120)//���30s���ղ���CRM��ʱ 
                    {
                        OverTimeState=1;//��ʱ��־λ��1                
                        CHMStep=0x07;
                        BEMError1|=0x01;//�ղ���CRM��30s����BEM
                        CRMOverTimeBefore = 0;
                    }
        	      }
        	      if(DCStartState==1)//���0xaa�Ƿ�ʱ
        	      {
        	          CRMOverTime++;
        			      if(CRMOverTime>=20)//�ղ���0xaa5s���ϱ�
        			      {
            			      CHMStep=0x07;
        			          OverTimeState=1;
        			          BEMError1|=0x04;
        			          CRMOverTime = 0; 
        			      } 
        	      }
    	          CHMStep1=0;
    	          counter_250ms1=0;
    	      }    
    	      if(CHMStep1==0)
    	      { 
    			      if(DCStartState==1)//����Ѿ����յ��ɹ��꿪ʼ
    			      {       			          
        			      sendi1++;
        			      if(sendi1==1)
        			          cpuToCHMBRM();
        			      else if(sendi1==2)
        			          cpuToCHMBRMDATA1();
        			      else if(sendi1==3)
        			          cpuToCHMBRMDATA2();
        			      else if(sendi1==4)
        			          cpuToCHMBRMDATA3();
        			      else if(sendi1==5)
        			          cpuToCHMBRMDATA4();
        			      else if(sendi1==6)
        			          cpuToCHMBRMDATA5();
        			      else if(sendi1==7)
        			      {       			            
    			              cpuToCHMBRMDATA6();
    			              if(DC_Vesion==1)
    			              {        			                
        			              sendi1=0;
        			              CHMStep1=1;
    			              }
        			      }
        			      else if(sendi1==8)
        			      {  
        			          cpuToCHMBRMDATA7();
        			          sendi1=0;        			          
        			          CHMStep1=1;
        			      }
        			  }
    	      }
    	  }
    		if(CHMStep==0x02)   //1801f456 aa
			  {
			      counter_250ms2++;
			      if(counter_250ms2>=50)//500ms
			      {
			        
			          CHMStep2=0;
			          counter_250ms2=0;
			      }
			      if(CHMStep2==0)
			      { 
    			      sendi2++;
    			      if(sendi2%3==1)
    			          cpuToCHMBCP();
    			      if(sendi2%3==2)
    			          cpuToCHMBCPDATA1();
    			      else if(sendi2%3==0)
    			      {
    			          sendi2=0;
    			          cpuToCHMBCPDATA2();
    			          CHMStep2=1;
    			      }
    			      sendi1=0;//�����һ�����
			      }
			  }
			  if(CHMStep==0x03)  //1808f456 aa 250ms
  		  {            
            counter_250ms3++;
            if(CHMStep3==0)
            {
                CROOverTime++;
                if(SelfState3==0)
          			    cpuToCHMBRO();  //100956f4 aa
                if((CROOverTime>=20)||(CROOverTime60s>=240))//�յ�CRO,��û���յ�0xaa,5s��ʱ 
                {
                    OverTimeState=1;//��ʱ��־λ��1                
                    CHMStep=0x07;
                    BEMError2|=0x04;
                    CROOverTime = 0;
                    CROOverTime60s = 0;
                }
                if(BROErrorAA==1)//���׮������������
                {
                    CHMStep=0x07;
                    BEMError4|=0x04;//��������                
                } 
                else if(0==BROErrorAA)
                {
                    CROOverTime60s++;
                    TurnOnChangerRelay = 1;
                }
                   
                CHMStep3=1;
            }
            if(counter_250ms3>=25)//ȷ����һ֡���ݷ�������������ʱС��250ms
            {
              
  			        counter_250ms3=0;
  			        CHMStep3=0;
  			        
            }
  		  }
    		if((CHMStep==0x04)||(CHMStep==0x05)) //50ms����
        {
            CCSOverTime++;
            //if(CCSOverTime>=100)//10ms*100=1S
            if(CCSOverTime>=500)//10ms*500=5S//for test
            {
                OverTimeState=1;//��ʱ��־λ��1
                if(CHMStep<=0x06)                
                    CHMStep=0x06;
                BEMError3|=0x01;
                CCSOverTime = 0;    
            }
            if(counter_50ms>=5)//50ms 
            {			            
                cpuToCHMBCL();    // voltage request and current request
                counter_50ms=0;
            }
            counter_50ms++;
  		      counter_250ms4++;      //250ms
  		      if(counter_250ms4>=25)
  		      {
  		        
  		          CHMStep4_5=0;
  		          counter_250ms4=0;
  		      }
  		      if(CHMStep4_5==0)
  		      {			        
    			      sendi1++;
    			      if(sendi1%3==1)
    			          cpuToCHMBCS();
    			      else if(sendi1%3==2)
    			      {
    			          cpuToCHMBCSDATA1();
    			      }
    			      else if(sendi1%3==0)
    			      {
    			          sendi1=0;
    			          cpuToCHMBCSDATA2();
    			          CHMStep4_5=1;
    			      }
  		      }
  		      if(CHMStep==0x05) 
  		      {
      		      counter_250ms5++;
                if(CHMStep5==0)
                {
                    cpuToCHMBSM();
                    CHMStep5=1;
                }
                if(counter_250ms5>=25)//ȷ����һ֡���ݷ�������������ʱС��250ms
                {
                  
      			        counter_250ms5=0;
      			        CHMStep5=0;
      			        
                }
            }
  		  }	  
			  if((CHMStep==0x06)||((CHMStep==0x05)&&(fastendflag==1)))//10ms����һ��,��ʼ����ʱ��	
    		{
    			  //����յ������������ֹ���Ļ��ߵ����ѹ���ܵ�ѹ��������ֵ
    			  
    			  if(OverTimeState==1)
    			  {
    			      if(counter_250ms6_1%25==0)
    			      {    			        
    			          counter_250ms6_1 = 0;
    			          cpuToCHMBEM();
    			      }
    			      counter_250ms6_1++;
    			  }
    			  if(SetBSD==0)//���BSD�Ѿ�����,��BST���ٷ���
    			  {   			    
    			      cpuToCHMBST();
    			      m_askcurrent=0;//�������Ϊ0
        			  CSTOverTime++;//10ms*200
    			      if(CSTOverTime>=500)//��ֹ�ղ���CST����5s��ʱ,10ms*500 
    			      {
        			      CHMStep=0x07;
        			      OverTimeState = 1;
        			      BEMError3|=0x02;
        			      CSTOverTime = 0;
    			      }
    			  }
        		if(CHMStep==0x06)   
    			  {
                if(CHMStep6==0)
                {   
                    cpuToCHMBSD();  // report BMS status: the highest and the lowest cell voltage, temperature and SOC.
                    SetBSD = 1;
                    CHMStep6=1;
                }
                counter_250ms6++;
    			      if(counter_250ms6>=25)
    			      {
      			        counter_250ms6=0;
      			        CHMStep6=0;			                       
    			      }
    			      CSDOverTime++;
    			      if(CSDOverTime>=1000)//���ղ�������ͳ�Ʊ���ʱ,��ʱ10s�µ�
    			      {
    			          CHMStep=0x07;
    			          OverTimeState = 1;
    			          BEMError4|=0x01;
    			          CSDOverTime = 0;     			          
    			      }    			      
    			      
    			  }
			  }        
			  if(CHMStep==0x07) //�������� 
			  {     
	          if(counter_250ms7%25==0)
	          {
	              if((OverTimeState==1)||(BROErrorAA==1))//����г�ʱ���ϲ��ϱ�,û�в��ϱ�
	              {	                
	                  if(BEMStop==0)
	                      cpuToCHMBEM();
	              }
	              counter_250ms7 = 0;
	          }
	          counter_250ms7++;
	          OffState=1;//״̬����170��ת40�ı�־λ               
			  }
												
		}/////////END��䷢����Ϣ

		
}
*/
void TaskRechargeDC(void)
{
    static unsigned char counter_250ms1=0;
    static unsigned char CHMStep1=0;
    
    static unsigned char counter_250ms2=0;
    static unsigned char CHMStep2=0;
    
    static unsigned char counter_250ms3=0;
    static unsigned char CHMStep3=0;
    
    static unsigned char counter_250ms4=0;
    static unsigned char CHMStep4_5=0;
    static unsigned char counter_50ms=0;
    
    static unsigned char counter_250ms5=0;
    static unsigned char CHMStep5=0;
    
    static unsigned char counter_250ms6=0;
    static unsigned char counter_250ms6_1=0;
    static unsigned char CHMStep6=0;
    
    static unsigned char counter_250ms7=0;
    
    
    static unsigned char DCFinish=0; //ֱ�����ʱֻ����Q1һ��
    static unsigned char timer1S=0;//�����ѹ�����,ֹͣ��������ж���ʱ
    static unsigned char timer2S=0;//��һ�ν�������ʱ
    static unsigned char timer3S=0;//�ڶ��ν�������ʱ
    static unsigned char ChanceCurt=0;//���ĩ��,�����仯�󲻻���,
    static unsigned char ChangeTimeState=0;//���������Խ���һ�μӷ�
    float curr=0;
    unsigned char ErrorState = 0;
    if(g_BmsModeFlag != FASTRECHARGING)
        return;
    if((g_highestTemperature>=(HIGHEST_ALLOWED_CHARGE_T+40))||(g_lowestTemperature<(LOWEST_ALLOWED_CHARGE_T+40)))//40+50��40+0
    {
        fastend3|=0x01; //Temperature����ֹ���
        fastendflag=1; //���ֹͣ����־  
    }      
    curr = GetRequestCurrentDC();  
    ErrorState = DCFaultDone();
    if((ErrorState==1)||(CutDCChaCurt0.word!=0))//ֱ��������µ���ϴ���
    {
        m_askcurrent = 0;     
    } 
    else 
    {
        m_askcurrent = curr + HeatCurt;
        if(m_askcurrent>=115)
            m_askcurrent = 115;
    }
    if((g_highestCellVoltage>=CHARGE_CUTDOWN_CV1 )||(ChanceCurt==1))//����Ϊ����������һ��
    {         
        timer2S++;
        if(timer2S>=80)//10*80=800ms
        {    
            timer2S=81;                      
            if(m_askcurrent>0.1*C)
                m_askcurrent=0.1*C;
            else if(m_askcurrent==0)
                m_askcurrent=0;
            ChanceCurt=1;
        }
    } 
    else 
    {
        timer2S=0;
    }
	  if(g_highestCellVoltage>=HIGHEST_ALLOWED_CHARGE_CV)  //4.15Vֹͣ���
    {
        timer1S++;
        if(timer1S>=100)//10ms*100=1000ms
        {          
            if(DCFinish==0)
            {
                ChangerStopState = 1;
                SocRechargeAdjust(); //��SOC��Q1
        	      StoreParameter();
        	      DCFinish=1;
    	      
            }   
            fastendflag=1;
            fastend1|=0x10;//�����ѹ�ﵽĿ��ֵ��������ֹ���
            timer1S=103; 
                      
        }
    } 
    else 
    {
        timer1S=0; 
    }        
    //////////////////////////////���ʱ���͹���///////////////////////////
    BiggestFeedbackContinuous = m_askcurrent*m_askvoltage/1000;//���ʱ��� 
    
    
    ///*******///////��䷢����Ϣ 
	  if(CHMStep)
		{     
    	  if(CHMStep==0x01)      //1801f456 00,��֡���ģ�����250ms�����ļ��10ms
    	  {

    	      counter_250ms1++;
    	      if(counter_250ms1>=25)
    	      {
        	      if(DCStartState==2)//�¹��꿪ʼ
        	      {
                    cpuToCHMBHM();
                    CRMOverTimeBefore++;
                    if(CRMOverTimeBefore>=120)//���30s���ղ���CRM��ʱ 
                    {
                        OverTimeState=1;//��ʱ��־λ��1                
                        CHMStep=0x07;
                        BEMError1|=0x01;//�ղ���CRM��30s����BEM
                        CRMOverTimeBefore = 0;
                    }
        	      }
        	      if(DCStartState==1)//���0xaa�Ƿ�ʱ
        	      {
        	          CRMOverTime++;
        			      if(CRMOverTime>=20)//�ղ���0xaa5s���ϱ�
        			      {
            			      CHMStep=0x07;
        			          OverTimeState=1;
        			          BEMError1|=0x04;
        			          CRMOverTime = 0; 
        			      } 
        	      }
    	          counter_250ms1=0;
    	          CHMStep1 = 0;
    	      }    
    	      if(CHMStep1==0)
    	      { 
    			      if(DCStartState==1)//����Ѿ����յ��ɹ��꿪ʼ
    			      {       			          
		      
        			      if(BRMStep == 0)//������Է���BRM,���������
        			      {
        			          BRMStep = 1;//�յ�CTS��,��������;������BRM����1
        			          cpuToCHMBRM();
        			          counter_250ms1=0;
        			          sendi1++;
        			      }
        			      else if((BRMStep == 1)&&(FlagBRMSend==1))
        			      {
    			              sendi1++; 
            			      if(sendi1==2)
            			          cpuToCHMBRMDATA1();
            			      else if(sendi1==3)
            			          cpuToCHMBRMDATA2();
            			      else if(sendi1==4)
            			          cpuToCHMBRMDATA3();
            			      else if(sendi1==5)
            			          cpuToCHMBRMDATA4();
            			      else if(sendi1==6)
            			          cpuToCHMBRMDATA5();
            			      else if(sendi1==7)
            			      {       			            
        			              cpuToCHMBRMDATA6();
        			              if(DC_Vesion==1)
        			              {        			                
            			              sendi1=0;
            			              FlagBRMSend = 0;
            			              BRMStep = 0;//���յ�CRM��ʼ����BRM�Լ��´η���BRM��ʱ��
            			              CHMStep1=1;
        			              }
            			      }
            			      else if(sendi1==8)
            			      {  
            			          cpuToCHMBRMDATA7();
            			          sendi1=0;
            			          FlagBRMSend = 0;
            			          BRMStep = 0;//���յ�CRM��ʼ����BRM�Լ��´η���BRM��ʱ��        			          
            			          CHMStep1=1;
            			      }
        			      }
        			  }
    	      }
    	  }
    		if(CHMStep==0x02)   //1801f456 aa
			  {
			      if(BRMStep == 1)//�Ѿ�������BRM��ͷ�ļ�
			      {
			          PGN[1] = 0x02;
			          J1939_TP_CM_Abort();
			          BRMStep = 0; 
			      }
			      counter_250ms2++;
			      if(counter_250ms2>=50)//500ms
			      {  
			          CHMStep2=0;
			          counter_250ms2=0;
			      }
			      if(CHMStep2==0)
			      { 
    			      if(BCPStep==0)
    			      {
    			          BCPStep = 1;
    			          sendi2++;
    			          cpuToCHMBCP();
    			          counter_250ms2=0;
    			      } 
    			      else if((FlagBCPSend)&&(BCPStep==1))
    			      {
        			      sendi2++;
        			      if(sendi2==2)
        			          cpuToCHMBCPDATA1();
        			      else if(sendi2==3)
        			      {
        			          sendi2=0;
        			          cpuToCHMBCPDATA2();
        			          CHMStep2=1;
        			          BCPStep = 0;
        			          FlagBCPSend = 0;
        			      }
    			      
    			      }
    			        

			      }
			  }
			  if(CHMStep==0x03)  //1808f456 aa 250ms
  		  {            
            
            if(BCPStep == 1)
            {
                PGN[1] = 0x06;
			          J1939_TP_CM_Abort();
                BCPStep = 0;
            }
            counter_250ms3++;
            if(CHMStep3==0)
            {
                CROOverTime++;
                if(SelfState3==0)
          			    cpuToCHMBRO();  //100956f4 aa
                if((CROOverTime>=21)||(CROOverTime60s>=240))//�յ�CRO,��û���յ�0xaa,5s��ʱ 
                {
                    OverTimeState=1;//��ʱ��־λ��1                
                    CHMStep=0x07;
                    BEMError2|=0x04;
                    CROOverTime = 0;
                    CROOverTime60s = 0;
                }
                if(BROErrorAA==1)//���׮������������
                {
                    CHMStep=0x07;
                    BEMError4|=0x04;//��������                
                } 
                else if(0==BROErrorAA)
                {
                    CROOverTime60s++;
                    TurnOnChangerRelay = 1;
                }
                   
                CHMStep3=1;
            }
            if(counter_250ms3>=25)//ȷ����һ֡���ݷ�������������ʱС��250ms
            {
              
  			        counter_250ms3=0;
  			        CHMStep3=0;
  			        
            }
  		  }
    		if((CHMStep==0x04)||(CHMStep==0x05)) //50ms����
        {
            CCSOverTime++;
            if(CCSOverTime>=100)//10ms*100=1S
            //if(CCSOverTime>=500)//10ms*500=5S//for test
            {
                OverTimeState=1;//��ʱ��־λ��1
                if(CHMStep<=0x07)                
                    CHMStep=0x07;
                BEMError3|=0x01;
                CCSOverTime = 0;    
            }
            
            if(SetBST)
                CCSOverTime = 0;      //��ֹ���ڹ��ϣ�����BST�󣬳������CCS��ʱ
            
            if(counter_50ms>=5)//50ms 
            {			            
                cpuToCHMBCL();    // voltage request and current request
                counter_50ms=0;
            }
            counter_50ms++;
  		      counter_250ms4++;      //250ms
  		      if(counter_250ms4>=25)
  		      {
  		        
  		          CHMStep4_5=0;
  		          counter_250ms4=0;
  		      }
  		      if(CHMStep4_5==0)
  		      {			        
    			      
    			      if(BCSStep==0)
    			      { 
    			          sendi1++;
    			          cpuToCHMBCS();
    			          BCSStep = 1;
    			          counter_250ms4 = 0;
    			      } 
    			      else if((BCSStep == 1)&&(FlagBCSSend==1))
    			      {
    			          sendi1++;
        			      if(sendi1==2)
        			      {
        			          cpuToCHMBCSDATA1();
        			      }
        			      else if(sendi1==3)
        			      {
        			          sendi1=0;
        			          cpuToCHMBCSDATA2();
        			          CHMStep4_5=1;
        			          BCSStep = 0;     //�ϸ�,����Ӧ�����յ�ֹͣ���ĺ�
        			          FlagBCSSend = 0; //�ϸ�,����Ӧ�����յ�ֹͣ���ĺ�
        			      }
    			      }
  		      }
  		      if(CHMStep==0x05) 
  		      {

      		      counter_250ms5++;
                if(CHMStep5==0)
                {
                    cpuToCHMBSM();
                    CHMStep5=1;
                }
                if(counter_250ms5>=25)//ȷ����һ֡���ݷ�������������ʱС��250ms
                {
                  
      			        counter_250ms5=0;
      			        CHMStep5=0;
      			        
                }
            }
  		  }	  
			  if((CHMStep==0x06)||((CHMStep==0x05)&&(fastendflag==1)))//10ms����һ��,��ʼ����ʱ��	
    		{
    			  //����յ������������ֹ���Ļ��ߵ����ѹ���ܵ�ѹ��������ֵ
			      if(BCSStep == 1)
    		      {
    		          PGN[1] = 0x11;
  	              J1939_TP_CM_Abort();
    		          BCSStep = 0;
    		      }
    			  if(OverTimeState==1)
    			  {
    			      if(counter_250ms6_1%25==0)
    			      {    			        
    			          counter_250ms6_1 = 0;
    			          cpuToCHMBEM();
    			      }
    			      counter_250ms6_1++;
    			  }
    			  if(SetBSD==0)//���BSD�Ѿ�����,��BST���ٷ���
    			  {   			    
    			      cpuToCHMBST();
    			      SetBST=1;
    			      m_askcurrent=0;//�������Ϊ0
        			  CSTOverTime++;//10ms*200
    			      if(CSTOverTime>=500)//��ֹ�ղ���CST����5s��ʱ,10ms*500 
    			      {
        			      CHMStep=0x07;
        			      OverTimeState = 1;
        			      BEMError3|=0x04;
        			      CSTOverTime = 0;
    			      }
    			  }
        		if(CHMStep==0x06)   
    			  {
                if(CHMStep6==0)
                {   
                    cpuToCHMBSD();  // report BMS status: the highest and the lowest cell voltage, temperature and SOC.
                    SetBSD = 1;
                    CHMStep6=1;
                }
                counter_250ms6++;
    			      if(counter_250ms6>=25)
    			      {
      			        counter_250ms6=0;
      			        CHMStep6=0;			                       
    			      }
    			      CSDOverTime++;
    			      if(CSDOverTime>=1000)//���ղ�������ͳ�Ʊ���ʱ,��ʱ10s�µ�
    			      {
    			          CHMStep=0x07;
    			          OverTimeState = 1;
    			          BEMError4|=0x01;
    			          CSDOverTime = 0;     			          
    			      }    			      
    			      
    			  }
			  }        
			  if(CHMStep==0x07) //�������� 
			  {     
	          if(counter_250ms7%25==0)
	          {
	              if((OverTimeState==1)||(BROErrorAA==1))//����г�ʱ���ϲ��ϱ�,û�в��ϱ�
	              {	                
	                  if(BEMStop==0)
	                      cpuToCHMBEM();
	              }
	              counter_250ms7 = 0;
	          }
	          counter_250ms7++;
	          OffState=1;//״̬����170��ת40�ı�־λ               
			  }
												
		}/////////END��䷢����Ϣ

		
}
//******************************************************************************
//* Function name:   TaskDC
//* Description:     10ms�ж��е��øú���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void TaskDC(void)
{  
    if(FASTRECHARGING==g_BmsModeFlag)
    {
        if(DCStartState == 0)//�����յ�֮�󣬲��ټ�ʱ
            CRMOverTimeBefore60s++;        
        if(CRMOverTimeBefore60s>=6000)//10ms*6000
        {
            OverTimeState=1;//��ʱ��־λ��1                
            CHMStep=0x07;
            BEMError1|=0x01;//�ղ���CRM��30s����BEM
            CRMOverTimeBefore60s = 0;      
        }
        TaskRechargeDC();
    }
}

//******************************************************************************
//* Function name:   GetDCTem
//* Description:     ���ֱ�������¶�
//* EntryParameter : None
//* ReturnValue    : None
	
	//	TurnOn_CC2();//12V   ��ѹ�л��ڳ�ʼ��BMS��
//******************************************************************************
void GetDCTem (void)
{

    float getAD1=0;//ADֵ1
    float getAD2=0;//ADֵ2
    unsigned int GetADVol1 = 0;
    unsigned int GetADVol2 = 0;
    float VolValue1=0;//ת����ĵ�ѹֵ1 �¶�1
    float VolValue2=0;//ת����ĵ�ѹֵ2 �¶�2
    float VolValue3=0;//ת����ĵ�ѹֵ1 CC2��ѹ
    float VolValue4=0;//ת����ĵ�ѹֵ2 ��Դ��ѹ
    
    
    float R1=0;//�¸�ͷ1��ֵ
    float R2=0;//�¸�ͷ2��ֵ  

    getAD1 = readADC_SingleEnded(0); //T1
    getAD2 = readADC_SingleEnded(3); //T2
    
    GetADVol1 = readADC_SingleEnded(1);//Vol of CC2
    GetADVol2 = readADC_SingleEnded(2); //Vol of ��Դ
    //EnableInterrupts; //���жϣ�����״̬������ 
    VolValue1=4.096*getAD1/2048.0;     //(2048-0)/(4.096-0)=(AD1-0)/(ADX-0)
    VolValue2=4.096*getAD2/2048.0;
    
    VolValue3=4.096*GetADVol1/2048.0; //cc2
    VolValue4=4.096*GetADVol2/2048.0; //��Դ��ѹ
    
    CC2VOL = (75.0+12.0)*VolValue3/12.0; //CC2��ѹ
    PowerVOL = (75.0+12.0)*VolValue4/12.0; //��Դ��ѹ
 
    R1=VolValue1/(5-VolValue1)*10;
    R2=VolValue2/(5-VolValue2)*10;
    
    DCTem1=LookupTem(VolValue1);
    if(DCTem1<0)
        DCTem1=0;
    DCTem2=LookupTem(VolValue2);
    if(DCTem2<0)
        DCTem2=0;
    
}
//***********************************************************************
//************************************************************************
//*************************the end*************************************
//************************************************************************